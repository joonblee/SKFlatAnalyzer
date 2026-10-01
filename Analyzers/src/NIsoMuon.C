#include "NIsoMuon.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace {

const double kMuonJetDR = 0.3;
const double kTagJetDR = 0.4;
const double kLeadingMuonPt = 52.0;
const double kSubleadingMuonPt = 10.0;
const double kMuonEtaMax = 2.4;
const double kJetPtMin = 30.0;
const double kJetEtaMax = 2.4;
const double kPairMassMin = 1.8;
const double kHistogramMassMin = 2.0;

const double kMuonIDSFMinPt = 15.0;
const double kMuonIDSFMaxPt = 200.0;

double ClampSFPt(double pt, double low, double high) {
  if(pt < low) return low;
  if(pt >= high) return std::nextafter(high, low);
  return pt;
}

bool PassLoosePileupJetID(
  const Jet& jet,
  int dataYear
) {

  const double pt = jet.Pt();
  const double absEta = std::fabs(jet.Eta());

  // JetVetoMap probe prescription:
  // for pT > 50 GeV, no Pileup Jet ID requirement is imposed.
  if(pt > 50.0) return true;

  if(pt < 10.0) return false;
  if(absEta >= 5.0) return false;

  int ptBin = -1;

  if(pt < 20.0) {
    ptBin = 0;
  }
  else if(pt < 30.0) {
    ptBin = 1;
  }
  else if(pt < 40.0) {
    ptBin = 2;
  }
  else if(pt <= 50.0) {
    ptBin = 3;
  }

  int etaBin = -1;

  if(absEta < 2.5) {
    etaBin = 0;
  }
  else if(absEta < 2.75) {
    etaBin = 1;
  }
  else if(absEta < 3.0) {
    etaBin = 2;
  }
  else if(absEta < 5.0) {
    etaBin = 3;
  }

  if(ptBin < 0 || etaBin < 0) return false;

  // CMSSW 106X UL16 / UL16APV Loose Pileup Jet ID working points.
  // Rows: pT = 10-20, 20-30, 30-40, 40-50 GeV.
  // Columns: |eta| = 0-2.5, 2.5-2.75, 2.75-3.0, 3.0-5.0.
  static const double cut2016[4][4] = {
    {-0.95, -0.70, -0.52, -0.49},
    {-0.90, -0.57, -0.43, -0.42},
    {-0.71, -0.36, -0.29, -0.23},
    {-0.42, -0.09, -0.14, -0.02}
  };

  // CMSSW 106X UL17 / UL18 Loose Pileup Jet ID working points.
  static const double cut2017and2018[4][4] = {
    {-0.95, -0.72, -0.68, -0.47},
    {-0.88, -0.55, -0.60, -0.43},
    {-0.63, -0.18, -0.43, -0.24},
    {-0.19,  0.22, -0.13, -0.03}
  };

  double cut = 999.0;

  if(dataYear == 2016) {
    cut = cut2016[ptBin][etaBin];
  }
  else if(dataYear == 2017 || dataYear == 2018) {
    cut = cut2017and2018[ptBin][etaBin];
  }
  else {
    return false;
  }

  return jet.PileupJetId() > cut;
}

string BTagSystematicName(const TString &suffix) {
  // MCCorrection::GetBTaggingReweight_1a already implements the fixed-WP
  // BTV heavy/light and correlated/uncorrelated source masking.
  if(suffix == "_Syst_BTagHFCorrUp")      return "SystUpHTagCorr";
  if(suffix == "_Syst_BTagHFCorrDown")    return "SystDownHTagCorr";
  if(suffix == "_Syst_BTagHFUncorrUp")    return "SystUpHTagUnCorr";
  if(suffix == "_Syst_BTagHFUncorrDown")  return "SystDownHTagUnCorr";
  if(suffix == "_Syst_BTagLFCorrUp")      return "SystUpLTagCorr";
  if(suffix == "_Syst_BTagLFCorrDown")    return "SystDownLTagCorr";
  if(suffix == "_Syst_BTagLFUncorrUp")    return "SystUpLTagUnCorr";
  if(suffix == "_Syst_BTagLFUncorrDown")  return "SystDownLTagUnCorr";
  return "central";
}

bool IsBTagVariation(const TString &suffix) {
  return suffix.Contains("_Syst_BTag");
}

bool IsL1PrefireVariation(const TString &suffix) {
  return suffix == "_Syst_L1PrefireUp" ||
         suffix == "_Syst_L1PrefireDown";
}

} // namespace


NIsoMuon::NIsoMuon()
  : analysisMode(AnalysisMode::NIsoDimuon),
    RunSyst(false),
    RunXSecSyst(false),
    MCAnalysis(false),
    Leading_Muon_Pt(kLeadingMuonPt),
    Subleading_Muon_Pt(kSubleadingMuonPt),
    weight_Prefire(1.0) {
}

NIsoMuon::~NIsoMuon() {
}


void NIsoMuon::initializeAnalyzer() {

  const bool RunMuonIDMode = HasFlag("MuonIDEfficiency");
  const bool RunTriggerMode = HasFlag("TriggerEfficiency");

  if(RunMuonIDMode && RunTriggerMode) {
    cerr << "[NIsoMuon::initializeAnalyzer] Select only one efficiency mode: "
         << "MuonIDEfficiency or TriggerEfficiency" << endl;
    exit(EXIT_FAILURE);
  }

  analysisMode = AnalysisMode::NIsoDimuon;
  if(RunMuonIDMode) analysisMode = AnalysisMode::MuonIDEfficiency;
  if(RunTriggerMode) analysisMode = AnalysisMode::TriggerEfficiency;

  RunSyst = HasFlag("RunSyst");
  RunXSecSyst = HasFlag("RunXSecSyst");

  if(analysisMode != AnalysisMode::NIsoDimuon && (RunSyst || RunXSecSyst)) {
    cerr << "[NIsoMuon::initializeAnalyzer] Efficiency modes are central-only. "
         << "Do not combine MuonIDEfficiency/TriggerEfficiency with "
         << "RunSyst or RunXSecSyst." << endl;
    exit(EXIT_FAILURE);
  }

  if(IsDATA && (RunSyst || RunXSecSyst)) {
    cerr << "[NIsoMuon::initializeAnalyzer] RunSyst and RunXSecSyst are "
         << "MC-only modes; refusing to run on data." << endl;
    exit(EXIT_FAILURE);
  }

  // Stock SKFlatAnalyzer/MCCorrection in the Run2UltraLegacy setup is Run-2
  // specific.  Do not silently run this port for a Run-3 era.
  if(DataYear > 2018) {
    cerr << "[NIsoMuon::initializeAnalyzer] This NIsoMuon.C port targets "
         << "the Run-2 UL SKFlatAnalyzer setup.  Run-3 requires the "
         << "corresponding Run-3 SKFlat correction/trigger infrastructure."
         << endl;
    exit(EXIT_FAILURE);
  }

  TString modeName = "NIsoDimuon";
  if(analysisMode == AnalysisMode::MuonIDEfficiency) modeName = "MuonIDEfficiency";
  if(analysisMode == AnalysisMode::TriggerEfficiency) modeName = "TriggerEfficiency";

  cout << "[NIsoMuon::initializeAnalyzer] Analysis mode = " << modeName << endl;
  cout << "[NIsoMuon::initializeAnalyzer] RunSyst = " << RunSyst << endl;
  cout << "[NIsoMuon::initializeAnalyzer] RunXSecSyst = " << RunXSecSyst << endl;

  MuonID1s = {"POGMedium"};
  MuonIDSFKey = "NUM_MediumID_DEN_TrackerMuons";
  MuonISOSFKey = "";

  DileptonSigns = {"OS", "SS"};
  JetIDs = {"tight"};

  // DeepJet Medium defines the independent tag jet in the b-enriched region.
  // DeepJet Loose defines the event-level veto in the light-jet region.
  jtps.clear();
  BTagNames.clear();

  jtps.push_back(
    JetTagging::Parameters(
      JetTagging::DeepJet,
      JetTagging::Medium,
      JetTagging::incl,
      JetTagging::comb
    )
  );
  BTagNames.push_back("BJet");

  jtps.push_back(
    JetTagging::Parameters(
      JetTagging::DeepJet,
      JetTagging::Loose,
      JetTagging::incl,
      JetTagging::comb
    )
  );
  BTagNames.push_back("LightJet");

  mcCorr->SetJetTaggingParameters(jtps);

  METCuts = {0.0};
  METCutNames = {""};

  // TriggerSetup is the native SKFlat trigger configuration.
  // For TriggerEfficiency the denominator must use an isolated single-muon
  // trigger rather than the target high-pT trigger.
  TriggerName.clear();
  Leading_Muon_Pt = 999.0;
  Subleading_Muon_Pt = kSubleadingMuonPt;

  TString RequestedTriggerInput = TriggerInput;
  if(analysisMode == AnalysisMode::TriggerEfficiency) {
    TriggerInput = "SingleMuon";
  }

  TriggerSetup(TriggerName, Leading_Muon_Pt, Subleading_Muon_Pt);
  TriggerInput = RequestedTriggerInput;

  // The nominal search and the muon-ID efficiency mode use the same
  // offline high-pT requirement as the NanoAOD implementation.
  if(analysisMode != AnalysisMode::TriggerEfficiency) {
    Leading_Muon_Pt = kLeadingMuonPt;
  }
  Subleading_Muon_Pt = kSubleadingMuonPt;

  TriggerNameForSF_Muon = "Mu50";

  MCAnalysis = false;

  cout << "[NIsoMuon::initializeAnalyzer] Run-2 event cleaning uses "
       << "PassMETFilter() and the official JME JetVetoMap payload." << endl;

  cout << "[NIsoMuon::initializeAnalyzer] NOTE: Method-1a uses the MC tagging "
       << "efficiency map configured in MCCorrection::SetupMCJetTagEff(). "
       << "For final analysis parity this must point to the NIsoMuon-specific "
       << "pretag efficiency map." << endl;
}


void NIsoMuon::executeEvent() {

  AllMuons = JBGetAllMuons();
  AllJets = GetAllJets();
  AllFatJets = GetAllFatJets();

  // Apply the standard Run-2 event-quality filters for every event,
  // irrespective of whether MET itself is used by the analysis.
  if(!PassMETFilter()) return;

  // --------------------------------------------------------------------------
  // Run-2 JME JetVetoMap cleaning.
  //
  // The veto-map probe is intentionally looser than the nominal analysis-jet
  // selection.  A jet is checked against the map only if it satisfies:
  //   * pT > 15 GeV and |eta| < 5;
  //   * Tight Jet ID;
  //   * charged-EM + neutral-EM fraction < 0.9;
  //   * pT > 50 GeV OR Loose Pileup Jet ID;
  //   * no reconstructed-muon overlap within DeltaR < 0.2.
  //
  // For Run 2, a problematic jet is removed from the jet collection; the
  // whole event is not rejected.  Jets overlapping a muon are not used as
  // veto-map probes, which is important for the dimuon-in-jet signal topology.
  // --------------------------------------------------------------------------
  vector<Jet> cleanedJets;
  cleanedJets.reserve(AllJets.size());

  for(const auto& jet : AllJets) {

    bool checkVetoMap = true;

    if( jet.Pt() < 15.0 || std::fabs(jet.Eta()) > 5.0 || !jet.PassID("tight") || (jet.chargedEmFraction() + jet.neutralEmFraction()) >= 0.9 )
      checkVetoMap = false;
    // Exact logical form: (pT > 50 GeV) OR (passes Loose Pileup Jet ID).
    if( jet.Pt() <= 50.0 && !PassLoosePileupJetID(jet, DataYear) )
      checkVetoMap = false;

    if(checkVetoMap) {
      for(const auto& muon : AllMuons) {
        if(jet.DeltaR(muon) < 0.2) {
          checkVetoMap = false;
          break;
        }
      }
    }

    if( checkVetoMap && mcCorr->IsJetVetoZone( jet.Eta(), jet.Phi(), "jetvetomap" ) )
      continue;

    cleanedJets.push_back(jet);
  }

  AllJets.swap(cleanedJets);

  AnalyzerParameter param;
  JBAnalyzerParameter JBparam;

  param.Clear();
  param.Muon_Tight_ID = "POGMedium";
  param.Muon_ID_SF_Key = MuonIDSFKey;
  param.Muon_ISO_SF_Key = MuonISOSFKey;
  param.Muon_Trigger_SF_Key = "POGHighPtLooseTrkIso";
  param.Jet_ID = "tight";
  param.syst_ = AnalyzerParameter::Central;

  // The dedicated efficiency modes are central-only and intentionally do not
  // use a b-tagging requirement or the muon SF that they are measuring.
  if(analysisMode != AnalysisMode::NIsoDimuon) {
    JBparam.Clear();
    JBparam.DileptonSign = "OS";
    JBparam.BTagName = "";
    JBparam.SystName = "";
    param.Name =
      analysisMode == AnalysisMode::MuonIDEfficiency
        ? "MuonIDEfficiency"
        : "TriggerEfficiency";

    executeEventFromParameter(param, JBparam);
    return;
  }

  for(unsigned int itMuonID = 0; itMuonID < MuonID1s.size(); ++itMuonID) {

    param.Muon_Tight_ID = MuonID1s.at(itMuonID);

    for(unsigned int itJetID = 0; itJetID < JetIDs.size(); ++itJetID) {

      param.Jet_ID = JetIDs.at(itJetID);

      for(unsigned int itBTag = 0; itBTag < BTagNames.size(); ++itBTag) {

        JBparam.Clear();
        JBparam.BTagParameter = jtps.at(itBTag);
        JBparam.BTagName = BTagNames.at(itBTag);

        for(unsigned int itSign = 0; itSign < DileptonSigns.size(); ++itSign) {

          JBparam.DileptonSign = DileptonSigns.at(itSign);

          // Systematic production is restricted to the OS b-jet region.
          // Keep the nominal job's light-jet and same-sign control regions.
          if((RunSyst || RunXSecSyst) &&
             (JBparam.BTagName != "BJet" ||
              JBparam.DileptonSign != "OS")) {
            continue;
          }

          // Light-jet SS is not used by the background strategy.
          if(JBparam.BTagName == "LightJet" &&
             JBparam.DileptonSign == "SS") {
            continue;
          }

          // Signal is generated only for the OS b-jet category.
          if(MCSample.Index("Zp") != kNPOS &&
             (JBparam.BTagName == "LightJet" || JBparam.DileptonSign == "SS")) {
            continue;
          }

          param.Name =
            JBparam.DileptonSign + "_" +
            param.Muon_Tight_ID + "_" +
            param.Jet_ID + "_" +
            JBparam.BTagName;

          // Always write the central histogram.  This prevents RunSyst and
          // RunXSecSyst jobs from depending on mutable state from a previous
          // variation.
          param.syst_ = AnalyzerParameter::Central;
          JBparam.SystName = "";
          executeEventFromParameter(param, JBparam);

          if(IsDATA) continue;

          if(RunSyst) {

            struct VariationConfig {
              AnalyzerParameter::Syst syst;
              TString suffix;
            };

            const vector<VariationConfig> standardVariations = {
              {AnalyzerParameter::JetResUp,          "_Syst_JetResUp"},
              {AnalyzerParameter::JetResDown,        "_Syst_JetResDown"},
              {AnalyzerParameter::JetEnUp,           "_Syst_JetEnUp"},
              {AnalyzerParameter::JetEnDown,         "_Syst_JetEnDown"},
              {AnalyzerParameter::MuonEnUp,          "_Syst_MuonEnUp"},
              {AnalyzerParameter::MuonEnDown,        "_Syst_MuonEnDown"},
              {AnalyzerParameter::MuonIDSFUp,        "_Syst_MuonIDSFUp"},
              {AnalyzerParameter::MuonIDSFDown,      "_Syst_MuonIDSFDown"},
              {AnalyzerParameter::MuonTriggerSFUp,   "_Syst_MuonTriggerSFUp"},
              {AnalyzerParameter::MuonTriggerSFDown, "_Syst_MuonTriggerSFDown"},
              {AnalyzerParameter::PUUp,              "_Syst_PUUp"},
              {AnalyzerParameter::PUDown,            "_Syst_PUDown"}
            };

            for(const auto &variation : standardVariations) {
              param.syst_ = variation.syst;
              JBparam.SystName = variation.suffix;
              executeEventFromParameter(param, JBparam);
            }

            // L1 ECAL & muon prefiring is relevant for 2016 and 2017.
            if(DataYear == 2016 || DataYear == 2017 || DataYear == 2018) {
              param.syst_ = AnalyzerParameter::Central;

              JBparam.SystName = "_Syst_L1PrefireUp";
              executeEventFromParameter(param, JBparam);

              JBparam.SystName = "_Syst_L1PrefireDown";
              executeEventFromParameter(param, JBparam);
            }

            // BTV fixed-WP recommendation:
            // heavy/light x correlated/uncorrelated, each with up/down.
            const vector<VariationConfig> bTagVariations = {
              {AnalyzerParameter::BTagUp,   "_Syst_BTagHFCorrUp"},
              {AnalyzerParameter::BTagDown, "_Syst_BTagHFCorrDown"},
              {AnalyzerParameter::BTagUp,   "_Syst_BTagHFUncorrUp"},
              {AnalyzerParameter::BTagDown, "_Syst_BTagHFUncorrDown"},
              {AnalyzerParameter::BTagUp,   "_Syst_BTagLFCorrUp"},
              {AnalyzerParameter::BTagDown, "_Syst_BTagLFCorrDown"},
              {AnalyzerParameter::BTagUp,   "_Syst_BTagLFUncorrUp"},
              {AnalyzerParameter::BTagDown, "_Syst_BTagLFUncorrDown"}
            };

            for(const auto &variation : bTagVariations) {
              param.syst_ = variation.syst;
              JBparam.SystName = variation.suffix;
              executeEventFromParameter(param, JBparam);
            }
          }

          if(RunXSecSyst) {

            param.syst_ = AnalyzerParameter::Central;

            for(unsigned int index = 0; index < 9; ++index) {
              JBparam.SystName =
                "_Syst_PDFScale" + TString::Itoa(index, 10);
              executeEventFromParameter(param, JBparam);
            }

            for(unsigned int index = 0; index < 100; ++index) {
              JBparam.SystName =
                "_Syst_PDFError" + TString::Itoa(index, 10);
              executeEventFromParameter(param, JBparam);
            }

            for(unsigned int index = 0; index < 2; ++index) {
              JBparam.SystName =
                "_Syst_PDFAlphaS" + TString::Itoa(index, 10);
              executeEventFromParameter(param, JBparam);
            }
          }
        }
      }
    }
  }
}

void NIsoMuon::executeEventFromParameter(
  AnalyzerParameter param,
  JBAnalyzerParameter JBparam
) {

  Event ev = GetEvent();

  // Use independent copies for every variation.  The historical implementation
  // modified the member collections in-place, which can make consecutive
  // variations depend on the order in which they are evaluated.
  vector<Muon> thisMuons = AllMuons;
  vector<Jet> thisJets = AllJets;

  if(!IsDATA && RunSyst) {

    if(param.syst_ == AnalyzerParameter::JetResUp) {
      thisJets = SmearJets(AllJets, +1);
    }
    else if(param.syst_ == AnalyzerParameter::JetResDown) {
      thisJets = SmearJets(AllJets, -1);
    }
    else if(param.syst_ == AnalyzerParameter::JetEnUp) {
      thisJets = ScaleJets(AllJets, +1);
    }
    else if(param.syst_ == AnalyzerParameter::JetEnDown) {
      thisJets = ScaleJets(AllJets, -1);
    }
    else if(param.syst_ == AnalyzerParameter::MuonEnUp) {
      thisMuons = ScaleMuons(AllMuons, +1);
    }
    else if(param.syst_ == AnalyzerParameter::MuonEnDown) {
      thisMuons = ScaleMuons(AllMuons, -1);
    }
  }

  if(!(ev.PassTrigger(TriggerName))) return;

  vector<Jet> alljets =
    SelectJets(thisJets, param.Jet_ID, kJetPtMin, kJetEtaMax);

  if(alljets.empty()) return;
  std::sort(alljets.begin(), alljets.end(), PtComparing);

  // --------------------------------------------------------------------------
  // Dedicated efficiency modes: no b-tag requirement and no measured muon SF.
  // --------------------------------------------------------------------------
  if(analysisMode == AnalysisMode::MuonIDEfficiency) {

    vector<Muon> denominatorMuons =
      SelectMuons(thisMuons, "isTrackerMuon", kSubleadingMuonPt, kMuonEtaMax);

    if(denominatorMuons.size() < 2) return;
    std::sort(denominatorMuons.begin(), denominatorMuons.end(), PtComparing);

    MuonIDEfficiency(
      ev,
      param,
      JBparam,
      denominatorMuons,
      alljets,
      alljets
    );
    return;
  }

  if(analysisMode == AnalysisMode::TriggerEfficiency) {

    vector<Muon> muons =
      SelectMuons(
        thisMuons,
        "POGMedium",
        kSubleadingMuonPt,
        kMuonEtaMax
      );

    if(muons.size() < 2) return;
    std::sort(muons.begin(), muons.end(), PtComparing);

    TriggerEfficiency(
      ev,
      param,
      JBparam,
      muons,
      alljets,
      alljets
    );
    return;
  }

  // --------------------------------------------------------------------------
  // Nominal NIsoDimuon analysis categories.
  // --------------------------------------------------------------------------
  vector<Jet> categoryJets;

  if(JBparam.BTagName == "BJet") {
    categoryJets = SelectBJets(alljets, JBparam.BTagParameter);
    if(categoryJets.empty()) return;
  }
  else if(JBparam.BTagName == "LightJet") {
    // The light-jet CR is an event-level Loose DeepJet veto.
    vector<Jet> looseBJets =
      SelectBJets(
        alljets,
        JetTagging::Parameters(
          JetTagging::DeepJet,
          JetTagging::Loose,
          JetTagging::incl,
          JetTagging::comb
        )
      );

    if(!looseBJets.empty()) return;

    // All selected jets are therefore Loose-failing jets.  Keep the complete
    // collection for the Method-1a failing-jet event weight.
    categoryJets = alljets;
  }
  else {
    return;
  }

  vector<Muon> muons =
    SelectMuons(
      thisMuons,
      param.Muon_Tight_ID,
      kSubleadingMuonPt,
      kMuonEtaMax
    );

  if(muons.size() < 2) return;
  std::sort(muons.begin(), muons.end(), PtComparing);

  NIsoDimuon(
    ev,
    param,
    JBparam,
    muons,
    categoryJets,
    alljets
  );
}

double NIsoMuon::MC_Weight(
  Event ev,
  AnalyzerParameter param,
  JBAnalyzerParameter JBparam
) {

  if(IsDATA) return 1.0;

  double out = 1.0;

  out *= MCweight();
  out *= ev.GetTriggerLumi("Full");

  int prefireVariation = 0;
  if(JBparam.SystName == "_Syst_L1PrefireUp") prefireVariation = +1;
  if(JBparam.SystName == "_Syst_L1PrefireDown") prefireVariation = -1;
  out *= GetPrefireWeight(prefireVariation);

  if(param.syst_ == AnalyzerParameter::PUUp) {
    out *= GetPileUpWeight(nPileUp, +1);
  }
  else if(param.syst_ == AnalyzerParameter::PUDown) {
    out *= GetPileUpWeight(nPileUp, -1);
  }
  else {
    out *= GetPileUpWeight(nPileUp, 0);
  }

  if(RunXSecSyst) {

    if(JBparam.SystName.BeginsWith("_Syst_PDFScale")) {
      TString indexString = JBparam.SystName;
      indexString.ReplaceAll("_Syst_PDFScale", "");
      const int index = indexString.Atoi();
      if(weight_Scale &&
         index >= 0 &&
         index < static_cast<int>(weight_Scale->size())) {
        out *= weight_Scale->at(index);
      }
    }

    if(JBparam.SystName.BeginsWith("_Syst_PDFError")) {
      TString indexString = JBparam.SystName;
      indexString.ReplaceAll("_Syst_PDFError", "");
      const int index = indexString.Atoi();
      if(weight_PDF &&
         index >= 0 &&
         index < static_cast<int>(weight_PDF->size())) {
        out *= weight_PDF->at(index);
      }
    }

    if(JBparam.SystName.BeginsWith("_Syst_PDFAlphaS")) {
      TString indexString = JBparam.SystName;
      indexString.ReplaceAll("_Syst_PDFAlphaS", "");
      const int index = indexString.Atoi();
      if(weight_AlphaS &&
         index >= 0 &&
         index < static_cast<int>(weight_AlphaS->size())) {
        out *= weight_AlphaS->at(index);
      }
    }
  }

  return out;
}


double NIsoMuon::Muon_Weight(
  AnalyzerParameter param,
  vector<Lepton*> leptons
) {

  if(IsDATA) return 1.0;

  vector<Muon*> muons;
  for(auto lep : leptons) {
    muons.push_back(static_cast<Muon*>(lep));
  }

  double out = 1.0;

  // Apply the high-pT trigger SF to the leading selected muon, matching the
  // analysis-level use of the high-pT single-muon trigger.
  if(!muons.empty()) {
    vector<Muon*> triggerMuon = {muons.at(0)};

    int triggerSyst = 0;
    if(param.syst_ == AnalyzerParameter::MuonTriggerSFUp) triggerSyst = +1;
    if(param.syst_ == AnalyzerParameter::MuonTriggerSFDown) triggerSyst = -1;

    out *= mcCorr->MuonTrigger_SF(
      param.Muon_Trigger_SF_Key,
      TriggerNameForSF_Muon,
      triggerMuon,
      triggerSyst
    );
  }

  for(auto muon : muons) {

    int idSyst = 0;
    if(param.syst_ == AnalyzerParameter::MuonIDSFUp) idSyst = +1;
    if(param.syst_ == AnalyzerParameter::MuonIDSFDown) idSyst = -1;

    const double sfPt =
      ClampSFPt(
        muon->MiniAODPt(),
        kMuonIDSFMinPt,
        kMuonIDSFMaxPt
      );

    out *= mcCorr->MuonID_SF(
      param.Muon_ID_SF_Key,
      muon->Eta(),
      sfPt,
      idSyst
    );
  }

  return out;
}


void NIsoMuon::NIsoDimuon(
  Event ev,
  AnalyzerParameter param,
  JBAnalyzerParameter JBparam,
  vector<Muon> muons,
  vector<Jet> jets,
  vector<Jet> alljets
) {

  JBparam.AnalysisName = "NIsoDimuon";

  const TString this_region =
    param.Name + JBparam.SystName + "_" + JBparam.AnalysisName;

  if(muons.size() < 2 || alljets.empty()) return;
  if(!(muons.at(0).Pt() > kLeadingMuonPt)) return;

  vector<Lepton*> dimuon;
  vector<Jet> selectedJets;

  // selectedJets[0] = jet containing the two selected muons
  // selectedJets[1] = independent DeepJet-Medium tag jet (BJet only)
  for(auto &dimuonJet : alljets) {

    for(unsigned int iMuon = 0; iMuon < muons.size(); ++iMuon) {

      if(!(muons.at(iMuon).Pt() > kLeadingMuonPt)) break;
      if(!(dimuonJet.DeltaR(muons.at(iMuon)) < kMuonJetDR)) continue;

      for(unsigned int jMuon = iMuon + 1; jMuon < muons.size(); ++jMuon) {

        if(!DimuonCharge(JBparam, muons.at(iMuon), muons.at(jMuon))) continue;
        if(!(dimuonJet.DeltaR(muons.at(jMuon)) < kMuonJetDR)) continue;

        const double mass =
          (muons.at(iMuon) + muons.at(jMuon)).M();

        if(!(mass > kPairMassMin)) continue;

        if(JBparam.BTagName == "BJet") {

          bool foundTagJet = false;
          Jet tagJet;

          for(const auto &candidateTagJet : jets) {
            if(!(candidateTagJet.DeltaR(dimuonJet) > kTagJetDR)) continue;
            tagJet = candidateTagJet;
            foundTagJet = true;
            break;
          }

          if(!foundTagJet) continue;

          dimuon.push_back(&muons.at(iMuon));
          dimuon.push_back(&muons.at(jMuon));
          selectedJets.push_back(dimuonJet);
          selectedJets.push_back(tagJet);
        }
        else {
          dimuon.push_back(&muons.at(iMuon));
          dimuon.push_back(&muons.at(jMuon));
          selectedJets.push_back(dimuonJet);
        }

        break;
      }

      if(!selectedJets.empty()) break;
    }

    if(!selectedJets.empty()) break;
  }

  if(dimuon.size() != 2 || selectedJets.empty()) return;

  double weight = 1.0;

  if(!IsDATA) {

    weight *= MC_Weight(ev, param, JBparam);
    weight *= Muon_Weight(param, dimuon);

    const string bTagSyst = BTagSystematicName(JBparam.SystName);

    if(JBparam.BTagName == "BJet") {
      // The b-tag requirement belongs to the independent tag jet, not to the
      // jet containing the dimuon.
      vector<Jet> tagJets = {selectedJets.at(1)};
      weight *= mcCorr->GetBTaggingReweight_1a(
        tagJets,
        JBparam.BTagParameter,
        bTagSyst
      );
    }
    else if(JBparam.BTagName == "LightJet") {
      // Every selected AK4 jet fails the Loose WP by construction.
      // Method 1a therefore applies the failing-jet correction to all of them.
      weight *= mcCorr->GetBTaggingReweight_1a(
        alljets,
        JBparam.BTagParameter,
        bTagSyst
      );
    }
  }

  const double dimuonMass = (*dimuon.at(0) + *dimuon.at(1)).M();
  if(!(dimuonMass > kHistogramMassMin)) return;

  const bool outsideUpsilon =
    dimuonMass < 9. || dimuonMass > 11.;

  if(
    JBparam.DileptonSign == "SS" ||
    JBparam.BTagName == "LightJet" ||
    outsideUpsilon
  ) {
    FillHist(
      this_region + "/Dilepton_Mass___" + this_region,
      dimuonMass,
      weight,
      7500,
      0.0,
      150.0
    );
  }

  // Object-validation histograms are kept in the search-side mass region,
  // matching the NanoAOD analyzer and the current plotter naming convention.
  if(!(11. < dimuonMass && dimuonMass < 80.)) return;

  FillHist(
    this_region + "/Dilepton_pT___" + this_region,
    (*dimuon.at(0) + *dimuon.at(1)).Pt(),
    weight,
    100,
    0.0,
    1000.0
  );

  for(unsigned int index = 0; index < dimuon.size(); ++index) {

    const TString label = TString::Itoa(index, 10);

    FillHist(
      this_region + "/Lepton_" + label + "_Pt___" + this_region,
      dimuon.at(index)->Pt(),
      weight,
      500,
      0.0,
      5000.0
    );

    FillHist(
      this_region + "/Lepton_" + label + "_Eta___" + this_region,
      dimuon.at(index)->Eta(),
      weight,
      600,
      -3.0,
      3.0
    );

    FillHist(
      this_region + "/Lepton_" + label + "_Phi___" + this_region,
      dimuon.at(index)->Phi(),
      weight,
      640,
      -3.2,
      3.2
    );
  }

  FillHist(
    this_region + "/Jet_0_Pt___" + this_region,
    selectedJets.at(0).Pt(),
    weight,
    100,
    0.0,
    500.0
  );

  FillHist(
    this_region + "/Jet_0_Eta___" + this_region,
    selectedJets.at(0).Eta(),
    weight,
    60,
    -3.0,
    3.0
  );

  FillHist(
    this_region + "/Jet_0_Phi___" + this_region,
    selectedJets.at(0).Phi(),
    weight,
    60,
    -3.0,
    3.0
  );

  if(JBparam.BTagName == "BJet" && selectedJets.size() > 1) {

    FillHist(
      this_region + "/Jet_1_Pt___" + this_region,
      selectedJets.at(1).Pt(),
      weight,
      100,
      0.0,
      500.0
    );

    FillHist(
      this_region + "/Jet_1_Eta___" + this_region,
      selectedJets.at(1).Eta(),
      weight,
      60,
      -3.0,
      3.0
    );

    FillHist(
      this_region + "/Jet_1_Phi___" + this_region,
      selectedJets.at(1).Phi(),
      weight,
      60,
      -3.0,
      3.0
    );
  }
}


void NIsoMuon::MuonIDEfficiency(
  Event ev,
  AnalyzerParameter param,
  JBAnalyzerParameter JBparam,
  vector<Muon> denominatorMuons,
  vector<Jet> jets,
  vector<Jet> alljets
) {

  JBparam.AnalysisName = "MuonIDEfficiency";

  const vector<double> ptEdges = {
    10.0, 15.0, 20.0, 25.0, 30.0, 40.0,
    50.0, 60.0, 120.0, 200.0, 2000.0
  };
  const vector<double> etaEdges = {
    0.0, 0.9, 1.2, 2.1, 2.4
  };

  auto edgeLabel = [](double value) -> TString {
    TString out = Form("%.1f", value);
    out.ReplaceAll(".", "p");
    out.ReplaceAll("-", "m");
    return out;
  };

  auto binTag = [&](const Muon &probe) -> TString {

    const double pt = probe.Pt();
    const double absEta = fabs(probe.Eta());

    for(unsigned int iEta = 0; iEta + 1 < etaEdges.size(); ++iEta) {
      if(!(etaEdges.at(iEta) <= absEta &&
           absEta < etaEdges.at(iEta + 1))) continue;

      for(unsigned int iPt = 0; iPt + 1 < ptEdges.size(); ++iPt) {
        if(!(ptEdges.at(iPt) <= pt &&
             pt < ptEdges.at(iPt + 1))) continue;

        return
          "Pt" +
          edgeLabel(ptEdges.at(iPt)) +
          "to" +
          edgeLabel(ptEdges.at(iPt + 1)) +
          "_AbsEta" +
          edgeLabel(etaEdges.at(iEta)) +
          "to" +
          edgeLabel(etaEdges.at(iEta + 1));
      }
    }

    return "";
  };

  for(const auto &jet : alljets) {

    for(const auto &tagMuon : denominatorMuons) {

      if(!(tagMuon.Pt() > kLeadingMuonPt)) break;
      if(!(fabs(tagMuon.Eta()) < kMuonEtaMax)) continue;
      if(!(tagMuon.PassID("POGMedium"))) continue;

      bool passHighPtTag = false;

      if(DataYear == 2016) {
        passHighPtTag =
          tagMuon.PassPath("HLT_Mu50_v") ||
          tagMuon.PassPath("HLT_TkMu50_v");
      }
      else if(DataYear == 2017 || DataYear == 2018) {
        passHighPtTag =
          tagMuon.PassPath("HLT_Mu50_v") ||
          tagMuon.PassPath("HLT_OldMu100_v") ||
          tagMuon.PassPath("HLT_TkMu100_v");
      }

      if(!passHighPtTag) continue;
      if(!(jet.DeltaR(tagMuon) < kMuonJetDR)) continue;

      for(const auto &probe : denominatorMuons) {

        if(tagMuon.Pt() == probe.Pt() &&
           tagMuon.Eta() == probe.Eta() &&
           tagMuon.Phi() == probe.Phi()) {
          continue;
        }

        if(!(probe.Pt() > kSubleadingMuonPt)) continue;
        if(!(fabs(probe.Eta()) < kMuonEtaMax)) continue;
        if(!(probe.isTrackerMuon())) continue;
        if(!(jet.DeltaR(probe) < kMuonJetDR)) continue;
        if(!(tagMuon.DeltaR(probe) > 0.05)) continue;
        if(!DimuonCharge(JBparam, tagMuon, probe)) continue;

        const double mass = (tagMuon + probe).M();
        if(!(2.0 < mass && mass < 5.0)) continue;

        const TString tag = binTag(probe);
        if(tag == "") continue;

        const bool pass = probe.PassID("POGMedium");
        const TString region =
          "MuonIDEfficiency_" +
          tag +
          (pass ? "_Pass" : "_Fail");

        double weight = 1.0;
        if(!IsDATA) {
          weight = MC_Weight(ev, param, JBparam);
        }

        FillHist(
          region + "/DileptonJPsi_Mass___" + region,
          mass,
          weight,
          300,
          2.0,
          5.0
        );

        return;
      }
    }
  }
}


void NIsoMuon::TriggerEfficiency(
  Event ev,
  AnalyzerParameter param,
  JBAnalyzerParameter JBparam,
  vector<Muon> muons,
  vector<Jet> jets,
  vector<Jet> alljets
) {

  JBparam.AnalysisName = "TriggerEfficiency";

  // Store the trigger tag-and-probe counts in the same (|eta|, pT) binning
  // consumed by SKPlotMaker/trig_eff.py. Keep the historical 1D histograms
  // below for backwards compatibility.
  double triggerEtaEdges[] = {
    0.0, 0.9, 1.2, 2.1, 2.4
  };
  double triggerPtEdges[] = {
    0.0, 10.0, 20.0, 30.0, 35.0,
    40.0, 42.0, 44.0, 46.0, 48.0, 50.0, 52.0, 55.0,
    60.0, 70.0, 80.0, 120.0, 200.0, 500.0, 2000.0
  };
  const int nTriggerEtaBins =
    sizeof(triggerEtaEdges) / sizeof(triggerEtaEdges[0]) - 1;
  const int nTriggerPtBins =
    sizeof(triggerPtEdges) / sizeof(triggerPtEdges[0]) - 1;

  unsigned int tagIndex = muons.size();

  for(unsigned int index = 0; index < muons.size(); ++index) {

    const Muon &muon = muons.at(index);

    double tagPtThreshold = 26.0;
    if(DataYear == 2017) tagPtThreshold = 29.0;

    if(!(muon.Pt() > tagPtThreshold)) continue;
    if(!(muon.PassID("POGTightWithTightIso"))) continue;

    bool passIsolatedTag = false;

    if(DataYear == 2016) {
      passIsolatedTag =
        muon.PassPath("HLT_IsoMu24_v") ||
        muon.PassPath("HLT_IsoTkMu24_v");
    }
    else if(DataYear == 2017) {
      passIsolatedTag =
        muon.PassPath("HLT_IsoMu27_v");
    }
    else if(DataYear == 2018) {
      passIsolatedTag =
        muon.PassPath("HLT_IsoMu24_v");
    }

    if(!passIsolatedTag) continue;

    bool jetClean = true;
    for(const auto &jet : alljets) {
      if(jet.DeltaR(muon) < 0.5) {
        jetClean = false;
        break;
      }
    }

    if(!jetClean) continue;

    tagIndex = index;
    break;
  }

  if(tagIndex == muons.size()) return;

  for(const auto &jet : alljets) {

    for(unsigned int index = 0; index < muons.size(); ++index) {

      if(index == tagIndex) continue;

      const Muon &probe = muons.at(index);

      if(!(probe.Pt() > kSubleadingMuonPt)) continue;
      if(!(probe.PassID("POGMedium"))) continue;
      if(!(jet.DeltaR(probe) < kMuonJetDR)) continue;

      double weight = 1.0;
      if(!IsDATA) {
        // Efficiency-mode MC weights intentionally exclude the muon ID/trigger
        // SFs that are being measured.
        weight = MC_Weight(ev, param, JBparam);
      }

      const TString denominatorRegion = "TriggerEfficiency_DENOM";
      const TString numeratorRegion = "TriggerEfficiency_NUM";

      FillHist(
        denominatorRegion + "/Probe_Pt___" + denominatorRegion,
        probe.Pt(),
        weight,
        500,
        0.0,
        500.0
      );

      FillHist(
        denominatorRegion + "/Probe_absEta___" + denominatorRegion,
        fabs(probe.Eta()),
        weight,
        48,
        0.0,
        2.4
      );

      FillHist(
        denominatorRegion + "/Probe_absEta_Pt___" + denominatorRegion,
        fabs(probe.Eta()),
        probe.Pt(),
        weight,
        nTriggerEtaBins,
        triggerEtaEdges,
        nTriggerPtBins,
        triggerPtEdges
      );

      bool passTarget = false;

      if(DataYear == 2016) {
        passTarget =
          probe.PassPath("HLT_Mu50_v") ||
          probe.PassPath("HLT_TkMu50_v");
      }
      else if(DataYear == 2017 || DataYear == 2018) {
        passTarget =
          probe.PassPath("HLT_Mu50_v") ||
          probe.PassPath("HLT_OldMu100_v") ||
          probe.PassPath("HLT_TkMu100_v");
      }

      if(passTarget) {
        FillHist(
          numeratorRegion + "/Probe_Pt___" + numeratorRegion,
          probe.Pt(),
          weight,
          500,
          0.0,
          500.0
        );

        FillHist(
          numeratorRegion + "/Probe_absEta___" + numeratorRegion,
          fabs(probe.Eta()),
          weight,
          48,
          0.0,
          2.4
        );

        FillHist(
          numeratorRegion + "/Probe_absEta_Pt___" + numeratorRegion,
          fabs(probe.Eta()),
          probe.Pt(),
          weight,
          nTriggerEtaBins,
          triggerEtaEdges,
          nTriggerPtBins,
          triggerPtEdges
        );
      }

      return;
    }
  }
}


// Compatibility stubs for methods retained in the historical NIsoMuon.h.
// They are not part of the current nominal analysis path.
void NIsoMuon::IsoEfficiency(
  Event,
  AnalyzerParameter,
  JBAnalyzerParameter,
  vector<Muon>,
  vector<Jet>,
  vector<Jet>
) {
}

void NIsoMuon::SingleMuonIsoEfficiency(
  Event,
  AnalyzerParameter,
  JBAnalyzerParameter,
  vector<Muon>,
  vector<Jet>,
  vector<Jet>
) {
}

void NIsoMuon::GenLevelAnalysis(
  Event,
  AnalyzerParameter,
  JBAnalyzerParameter
) {
}
