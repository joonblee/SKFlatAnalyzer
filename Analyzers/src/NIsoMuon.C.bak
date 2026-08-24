#include "NIsoMuon.h"

const double dRmax = .3;

void NIsoMuon::initializeAnalyzer(){
  //////////////////////////
  // --- Analysis mode --- //
  //////////////////////////
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

  TString AnalysisModeName = "NIsoDimuon";
  if(analysisMode == AnalysisMode::MuonIDEfficiency)
    AnalysisModeName = "MuonIDEfficiency";
  else if(analysisMode == AnalysisMode::TriggerEfficiency)
    AnalysisModeName = "TriggerEfficiency";

  cout << "[NIsoMuon::initializeAnalyzer] Analysis mode = "
       << AnalysisModeName << endl;
  cout << "[NIsoMuon::initializeAnalyzer] RunSyst = "
       << RunSyst << endl;
  cout << "[NIsoMuon::initializeAnalyzer] RunXSecSyst = "
       << RunXSecSyst << endl;

  //////////////////
  // --- Muon --- //
  //////////////////
  MuonID1s = {
    //"POGLoose",
    "POGMedium",
    //"POGTight",
    /*
    "NonIsolatedMuon_NoisGlobalMuon",
    "NonIsolatedMuon_NoisPFMuon",
    "NonIsolatedMuon_NoChi2",
    "NonIsolatedMuon_NoValidMuonHits",
    "NonIsolatedMuon_NoMatchedStations",
    "NonIsolatedMuon_NodXY",
    "NonIsolatedMuon_NodZ",
    "NonIsolatedMuon_NoValidPixelHits",
    "NonIsolatedMuon_NoTrackerLayers",
    */
    /*
    "POGTight_NoisGlobalMuon",
    "POGTight_NoisPFMuon",
    "POGTight_NoChi2",
    "POGTight_NoValidMuonHits",
    "POGTight_NoMatchedStations",
    "POGTight_NodXY",
    "POGTight_NodZ",
    "POGTight_NoValidPixelHits",
    "POGTight_NoTrackerLayers",
    "POGTight_NoIP3D",
    */
    //"NonIsolatedMuon",
    //"NonIsolatedLooseMuon",
    //"NonIsolatedMuonNoIP3D",
    //"POGTightWithTightTrkIso",
    //"POGTightWithTightIso"
  };

  MuonIDSFKey = "NUM_MediumID_DEN_TrackerMuons"; // "NUM_TightID_DEN_TrackerMuons"; // old SF name "NUM_TightID_DEN_genTracks";
  MuonISOSFKey = ""; // "NUM_TightRelIso_DEN_TightIDandIPCut";

  //DileptonSigns = {"OS","SS","PP","MM"};
  DileptonSigns = {"OS","SS"};
  //DileptonSigns = {"OS"};

  /////////////////
  // --- Jet --- //
  /////////////////
  JetIDs = {"tight"}; // "tightLepVeto", "tightLepCleaning": tightLepVeto + MuFrac < 0.5

  ///////////////////////
  // --- b tagging --- //
  ///////////////////////
  jtps={}; BTagNames = {};
  // For test //
  //jtps.push_back(JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Tight, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("BJet_DCT");
  //jtps.push_back(JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Medium, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("BJet_DCM"); // FIXME
  //jtps.push_back(JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Loose, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("BJet_DCL"); // FIXME
  //jtps.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("BJet_DJL"); // FIXME
  //jtps.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("BJet_DJT"); // FIXME
  //jtps.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("VetoBJet");
  // For analysis //
  jtps.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("BJet"); // FIXME
  jtps.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::comb)); BTagNames.push_back("LightJet");
  mcCorr->SetJetTaggingParameters(jtps);

  /////////////////
  // --- MET --- //
  /////////////////
  //METCuts     = {0, 40};
  //METCutNames = {"", "HighMET"};
  METCuts     = {0};
  METCutNames = {""};

  /////////////////////
  // --- Trigger --- //
  /////////////////////
  TriggerName = {};
  Leading_Muon_Pt = 999.;
  Subleading_Muon_Pt = 10.; // *** CHANGE SUBLEADING MUON PT *** //

  // Follow the SKNano efficiency-mode logic:
  //   * nominal NIsoDimuon and MuonIDEfficiency use the requested HighPtMuon OR;
  //   * TriggerEfficiency uses an isolated single-muon reference trigger for
  //     the event denominator.  The target HighPtMuon OR is checked on the
  //     probe inside TriggerEfficiency().
  TString RequestedTriggerInput = TriggerInput;
  if(analysisMode == AnalysisMode::TriggerEfficiency)
    TriggerInput = "SingleMuon";

  TriggerSetup(TriggerName, Leading_Muon_Pt, Subleading_Muon_Pt);
  TriggerInput = RequestedTriggerInput;
  //TriggerName = { 
  //  "HLT_PFHT300_PFMET110_v",
  //  "HLT_PFMET110_PFMHT110_IDTight_v",
  //  "HLT_PFMET200_v",
  //};  
  //Leading_Muon_Pt = 26.;
  //Subleading_Muon_Pt = 10.;

  TriggerNameForSF_Muon = "Mu50";

  ////////////////////////
  // --- Additional --- //
  ////////////////////////
  MCAnalysis = 0;
}

void NIsoMuon::executeEvent(){
  AllMuons = JBGetAllMuons();
  AllJets = GetAllJets();
  AllFatJets = GetAllFatJets();

  weight_Prefire = GetPrefireWeight(0);

  //cout<<endl;
  //cout<<"run: "<<run<<", event: "<<event<<endl;

  AnalyzerParameter param;
  JBAnalyzerParameter JBparam;

  for(unsigned int it_MuonID1=0; it_MuonID1<MuonID1s.size(); it_MuonID1++) {
    TString MuonID1 = MuonID1s.at(it_MuonID1);

    param.Clear();

    param.Muon_Tight_ID = MuonID1;

    param.Muon_ID_SF_Key = MuonIDSFKey;
    param.Muon_ISO_SF_Key = MuonISOSFKey;
    param.Muon_Trigger_SF_Key = "POGHighPtLooseTrkIso"; //"POGTight";

    param.syst_ = AnalyzerParameter::Central;

    for(unsigned it_JetID=0; it_JetID<JetIDs.size(); it_JetID++) {
      param.Jet_ID = JetIDs[it_JetID];

      for(unsigned it_BJet=0; it_BJet<BTagNames.size(); it_BJet++) {
        JBparam.Clear();
  
        //JBparam.BTagger    = BTagger;
        //JBparam.BTaggingWP = BTaggingWPs[it_BJet];
        JBparam.BTagParameter = jtps[it_BJet];
        JBparam.BTagName      = BTagNames[it_BJet];
  
        for(unsigned it_Sign=0; it_Sign<DileptonSigns.size(); it_Sign++) {
          JBparam.DileptonSign = DileptonSigns[it_Sign];
  
          for(unsigned it_METCut=0; it_METCut<METCutNames.size(); it_METCut++) {
            JBparam.METCut= METCuts[it_METCut];
            JBparam.METCutName = METCutNames[it_METCut];
  
            param.Name = "";
            // if( JBparam.METCutName != "" ) param.Name += JBparam.METCutName+"_";          
            param.Name += JBparam.DileptonSign+"_"+param.Muon_Tight_ID+"_"+param.Jet_ID;
            if( JBparam.BTagName != "" ) param.Name += "_"+JBparam.BTagName;
            if( JBparam.METCutName != "" ) param.Name += "_"+JBparam.METCutName; 
            JBparam.SystName = "";
            //cout<<"[NIsoMuon::executeEvent] Parameter name: "<<param.Name<<endl;
 
            /////////////////////////////
            // --- Run Systematics --- //
            /////////////////////////////
            if(RunSyst) {
              for(unsigned it_syst=1; it_syst<AnalyzerParameter::NSyst; it_syst++) {
                param.syst_ = AnalyzerParameter::Syst(it_syst);
                JBparam.SystName = "_Syst_"+param.GetSystType();
                executeEventFromParameter(param, JBparam);
              }
            }
            else if(RunXSecSyst && !IsDATA) {
              //for(unsigned i=0; i<weight_Scale->size(); i++) {
              for(unsigned i=0; i<9; i++) {
                JBparam.SystName = "_Syst_PDFScale"+TString::Itoa(i,10);
                executeEventFromParameter(param, JBparam);
              }
              //for(unsigned i=0; i<weight_PDF->size(); i++) {
              for(unsigned i=0; i<100; i++) {
                JBparam.SystName = "_Syst_PDFError"+TString::Itoa(i,10);
                executeEventFromParameter(param, JBparam);
              }
              //for(unsigned i=0; i<weight_AlphaS->size(); i++) {
              for(unsigned i=0; i<2; i++) {
                JBparam.SystName = "_Syst_PDFAlphaS"+TString::Itoa(i,10);
                executeEventFromParameter(param, JBparam);
              }
            }
            else executeEventFromParameter(param, JBparam);
            ////////////////////////////////
            // --- End Systematic run --- //
            ////////////////////////////////
          }
        }
      }
    }
  }
}

void NIsoMuon::executeEventFromParameter(AnalyzerParameter param, JBAnalyzerParameter JBparam){

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?
  if( MCSample.Index("Zp")!=kNPOS && JBparam.DileptonSign == "SS" ) return;
  if( (JBparam.BTagName.Index("VetoBJet")!=kNPOS || JBparam.BTagName.Index("LightJet")!=kNPOS) && JBparam.DileptonSign == "SS" ) return;

  Event ev = GetEvent();

  //GenLevelAnalysis(ev, param, JBparam);
  if( !(ev.PassTrigger(TriggerName) ) ) return;

  if( JBparam.METCutName != "" ) {
    if( !PassMETFilter() ) return;
    double MET = ev.GetMETVector().Pt();
    if( !(MET >= JBparam.METCut) ) return;
  }

  //////////////////
  // --- Syst --- //
  //////////////////
  if(RunSyst) {
    if(param.syst_ == AnalyzerParameter::Central) {}
    else if(param.syst_ == AnalyzerParameter::JetResUp) AllJets = SmearJets( AllJets, +1 );
    else if(param.syst_ == AnalyzerParameter::JetResDown) AllJets = SmearJets( AllJets, -1 );
    else if(param.syst_ == AnalyzerParameter::JetEnUp) AllJets = ScaleJets( AllJets, +1 );
    else if(param.syst_ == AnalyzerParameter::JetEnDown) AllJets = ScaleJets( AllJets, -1 );
    else if(param.syst_ == AnalyzerParameter::MuonRecoSFUp) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::MuonRecoSFDown) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::MuonEnUp) AllMuons = ScaleMuons( AllMuons, +1 );
    else if(param.syst_ == AnalyzerParameter::MuonEnDown) AllMuons = ScaleMuons( AllMuons, -1 );
    else if(param.syst_ == AnalyzerParameter::MuonIDSFUp) {}
    else if(param.syst_ == AnalyzerParameter::MuonIDSFDown) {}
    else if(param.syst_ == AnalyzerParameter::MuonISOSFUp) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::MuonISOSFDown) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::MuonTriggerSFUp) {}
    else if(param.syst_ == AnalyzerParameter::MuonTriggerSFDown) {}
    else if(param.syst_ == AnalyzerParameter::PUUp) {}
    else if(param.syst_ == AnalyzerParameter::PUDown) {}
    else if(param.syst_ == AnalyzerParameter::BTagUp) {}
    else if(param.syst_ == AnalyzerParameter::BTagDown) {}
    else if(param.syst_ == AnalyzerParameter::ElectronResUp) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::ElectronResDown) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::ElectronEnUp) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else if(param.syst_ == AnalyzerParameter::ElectronEnDown) {
      cout<<"[NIsoMuon::executeEventFromParameter] Syst: Do not use "<<param.GetSystType()<<endl;
      return;
    }
    else {
      cout << "[NIsoMuon::executeEventFromParameter] Wrong syst" << endl;
      return;
      //exit(EXIT_FAILURE);
    }
  }

  // --- Jet selection --- //
  vector<Jet> alljets = SelectJets(AllJets, param.Jet_ID, 30., 2.4); // FIXME
  if( alljets.size() == 0 ) return; // FIXME
  std::sort(alljets.begin(),alljets.end(),PtComparing);
  vector<Jet> jets;

  if(JBparam.BTagName.Index("BJet") == 0) {
    jets = SelectBJets(alljets, JBparam.BTagParameter);
    //if( jets.size() < 1 ) return;
  }
  else if(JBparam.BTagName.Index("VetoBJet")!=kNPOS||JBparam.BTagName.Index("LightJet")!=kNPOS) {
    jets = SelectLightJets(alljets, JBparam.BTagParameter);
    vector<Jet> bjets = SelectBJets(alljets, JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::comb));
    if( bjets.size() != 0 ) return;
  }

  if( jets.size() == 0 ) return;
  //if( JBparam.BTagName=="TwoVetoBJets" && jets.size()!=2 ) return;

  // --- Muon ID efficiency --- //
  if(analysisMode == AnalysisMode::MuonIDEfficiency) {
    if(param.Muon_Tight_ID != "POGMedium") return;
    if(JBparam.BTagName != "BJet") return;
    if(JBparam.DileptonSign != "OS") return;
    if(JBparam.SystName != "") return;

    vector<Muon> denominatorMuons =
      SelectMuons(AllMuons, "isTrackerMuon", 10., 2.4);
    if(denominatorMuons.size() < 2) return;

    std::sort(
      denominatorMuons.begin(),
      denominatorMuons.end(),
      PtComparing
    );

    MuonIDEfficiency(
      ev,
      param,
      JBparam,
      denominatorMuons,
      jets,
      alljets
    );
    return;
  }

  // --- Muon selection --- //
  vector<Muon> muons = SelectMuons(AllMuons, param.Muon_Tight_ID, Subleading_Muon_Pt, 2.4);
  if( muons.size() < 2 ) return; // FIXME
  std::sort(muons.begin(),muons.end(),PtComparing);

  // --- Trigger efficiency --- //
  if(analysisMode == AnalysisMode::TriggerEfficiency) {
    if(JBparam.BTagName != "BJet") return;
    if(JBparam.DileptonSign != "OS") return;
    if(JBparam.SystName != "") return;

    TriggerEfficiency(
      ev,
      param,
      JBparam,
      muons,
      jets,
      alljets
    );
    return;
  }

  //else if( param.Muon_Tight_ID=="POGTight" ) {
  //  IsoEfficiency(ev, param, JBparam, muons, jets, alljets);
  //  //SingleMuonIsoEfficiency(ev, param, JBparam, muons, jets, alljets);
  //}
  NIsoDimuon(ev, param, JBparam, muons, jets, alljets);
}


// ====================================================================================== //
// weight                                                                                 //
// ====================================================================================== //
double NIsoMuon::MC_Weight(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam) {
  double out = 1.;
  out *= MCweight();
  out *= ev.GetTriggerLumi("Full"); // Is IsoMu24 unprescaled?
  out *= weight_Prefire;
  //cout<<"weight_norm_1invpb = "<<weight_norm_1invpb<<", trigger lumi = "<<ev.GetTriggerLumi("Full")<<", MCweight = "<<ev.MCweight()<<", weight_Prefire = "<<weight_Prefire<<endl;
  if     (param.syst_ == AnalyzerParameter::PUUp)   out *= GetPileUpWeight(nPileUp,+1);
  else if(param.syst_ == AnalyzerParameter::PUDown) out *= GetPileUpWeight(nPileUp,-1);
  else                                              out *= GetPileUpWeight(nPileUp,0);

  ////////////////////////////////////////////////////////////
  // --- Xsec weight (PDF errer, PDF alphaS, PDF scale) --- //
  ////////////////////////////////////////////////////////////

  if(RunXSecSyst) {
    //cout<<"weight_Scale->size() = "<<weight_Scale->size()<<endl;
    for(unsigned i=0; i<weight_Scale->size(); i++) {
      //cout<<"weight_PDF[0] = "<<weight_PDF->at(0)<<", weight_PDF["<<i<<"] = "<<weight_PDF->at(i)<<endl;
      if(weight_Scale->size()!=0 && JBparam.SystName=="_Syst_PDFScale"+TString::Itoa(i,10)) {
        out *= weight_Scale->at(i);
      }
    }
    //cout<<"weight_PDF->size() = "<<weight_PDF->size()<<endl;
    for(unsigned i=0; i<weight_PDF->size(); i++) {
      if(weight_PDF->size()!=0 && JBparam.SystName=="_Syst_PDFError"+TString::Itoa(i,10)) {
        out *= weight_PDF->at(i);
      }
    }
    //cout<<"weight_AlphaS->size() = "<<weight_AlphaS->size()<<endl;
    for(unsigned i=0; i<weight_AlphaS->size(); i++) {
      if(weight_AlphaS->size()!=0 && JBparam.SystName=="_Syst_PDFAlphaS"+TString::Itoa(i,10)) {
        out *= weight_AlphaS->at(i);
      }
    }
  } 

  return out;
}

double NIsoMuon::Muon_Weight(AnalyzerParameter param, vector<Lepton*> leptons) {
  double out = 1.;

  vector<Muon*> muons;
  for(auto lep:leptons) {
    muons.push_back((Muon*)lep);
  }

  double this_trigsf = 1.;
  /* // HERE //
  if     (param.syst_ == AnalyzerParameter::MuonTriggerSFUp)   this_trigsf *= mcCorr->MuonTrigger_SF(param.Muon_Trigger_SF_Key, TriggerNameForSF_Muon, muons, +1);
  else if(param.syst_ == AnalyzerParameter::MuonTriggerSFDown) this_trigsf *= mcCorr->MuonTrigger_SF(param.Muon_Trigger_SF_Key, TriggerNameForSF_Muon, muons, -1);
  else                                                         this_trigsf *= mcCorr->MuonTrigger_SF(param.Muon_Trigger_SF_Key, TriggerNameForSF_Muon, muons);
  out *= this_trigsf;
  */
  /*
  cout<<fixed; cout.precision(10);
  cout<<endl<<"High pt muon trig SF"<<endl;
  cout<<"[NIsoMuon::Muon_Weight] trig sf(central) = "<<mcCorr->MuonTrigger_SF(param.Muon_Trigger_SF_Key, TriggerNameForSF_Muon, muons)<<endl;
  cout<<"[NIsoMuon::Muon_Weight] trig sf(up) = "<<mcCorr->MuonTrigger_SF(param.Muon_Trigger_SF_Key, TriggerNameForSF_Muon, muons, +1)<<endl;
  cout<<"[NIsoMuon::Muon_Weight] trig sf(down) = "<<mcCorr->MuonTrigger_SF(param.Muon_Trigger_SF_Key, TriggerNameForSF_Muon, muons, -1)<<endl;
  cout<<endl<<"iso muon trig SF"<<endl;
  cout<<"[NIsoMuon::Muon_Weight] trig sf(central) = "<<mcCorr->MuonTrigger_SF("POGTight", "IsoMu24", muons)<<endl;
  cout<<"[NIsoMuon::Muon_Weight] trig sf(up) = "<<mcCorr->MuonTrigger_SF("POGTight", "IsoMu24", muons, +1)<<endl;
  cout<<"[NIsoMuon::Muon_Weight] trig sf(down) = "<<mcCorr->MuonTrigger_SF("POGTight", "IsoMu24", muons, -1)<<endl;
  */

  for(unsigned int i=0; i<muons.size(); i++) {
    Muon *mu = muons[i];

    double this_idsf = 1.;
    if(param.syst_==AnalyzerParameter::MuonIDSFUp)        this_idsf=mcCorr->MuonID_SF(param.Muon_ID_SF_Key, mu->Eta(), mu->MiniAODPt(), +1);
    else if(param.syst_==AnalyzerParameter::MuonIDSFDown) this_idsf=mcCorr->MuonID_SF(param.Muon_ID_SF_Key, mu->Eta(), mu->MiniAODPt(), -1);
    else                                                  this_idsf=mcCorr->MuonID_SF(param.Muon_ID_SF_Key, mu->Eta(), mu->MiniAODPt());
    out *= this_idsf;
    //double this_isosf = 1.;
    //if(param.syst_==AnalyzerParameter::MuonISOSFUp)        this_isosf = 0;
    //else if(param.syst_==AnalyzerParameter::MuonISOSFDown) this_isosf = 0;
    //out *= this_isosf;
  }

  return out;
}


// ====================================================================================== //
// Start analysis                                                                         //
// ====================================================================================== //
void NIsoMuon::NIsoDimuon(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets) {
  JBparam.AnalysisName = "NIsoDimuon";
  TString this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;

  vector<Lepton*> LeadMuons;
  vector<Lepton*> SubMuons;
  vector<Jet> this_jet;
  vector<FatJet> this_fatjet;

  //////////////////////////////
  // --- Event selectioon --- //
  //////////////////////////////
  double tag_cut = 0.4;
//  if(JBparam.BTagName.Index("VetoBJet")!=kNPOS||JBparam.BTagName.Index("LightJet")!=kNPOS) {
//    if( alljets.size() != 2 ) return;
//    if( jets.size() != 2 ) return;
//    tag_cut = 1.;
//  }
  for(auto jet:alljets) {
    if( !(jet.Pt() > 30.) ) continue;
    for(unsigned iMu=0; iMu<muons.size(); iMu++) {
      if( !(muons[iMu].Pt() > Leading_Muon_Pt) ) return;
      if( !(jet.DeltaR(muons[iMu]) < dRmax) ) continue;
      for(unsigned jMu=iMu+1; jMu<muons.size(); jMu++) {
        if( !DimuonCharge(JBparam, muons[iMu], muons[jMu]) ) continue;
        if( !(jet.DeltaR(muons[jMu]) < dRmax) ) continue;
        if( !((muons[iMu]+muons[jMu]).M() > 1.8) ) continue;

        if(JBparam.BTagName.Index("BJet")==0) {
          for(auto tagjet:jets) {
            if( !(tagjet.Pt() > 30.) ) continue;          
            //bool dijet_angular_cut = (JBparam.BTagName.Index("VetoBJet") != kNPOS || JBparam.BTagName.Index("LightJet") != kNPOS) ? (fabs(jet.DeltaPhi(tagjet)) > tag_cut) : (fabs(jet.DeltaR(tagjet)) > tag_cut);
            //if(dijet_angular_cut) {
            if(fabs(jet.DeltaR(tagjet)) > tag_cut) {
              LeadMuons.push_back( &muons[iMu] );
              SubMuons.push_back( &muons[jMu] );
              this_jet.push_back( jet );
              this_jet.push_back( tagjet );
              break;
            }
          }
        }
        else if(JBparam.BTagName.Index("VetoBJet")!=kNPOS||JBparam.BTagName.Index("LightJet")!=kNPOS) {
          LeadMuons.push_back( &muons[iMu] );
          SubMuons.push_back( &muons[jMu] );
          this_jet.push_back( jet );
        }
        break;
      }
      if( this_jet.size()!=0 ) break;
    }
    if( this_jet.size()!=0 ) break;
  }
  if( this_jet.size()==0 ) return;

  for(unsigned i=0; i<1/*LeadMuons.size()*/; i++) { // Currently, only the leading pair is used //
    vector<Lepton*> dimuon = {LeadMuons[i],SubMuons[i]};

    //////////////////////////
    // --- Event weight --- //
    //////////////////////////
    double weight = 1.;
    if(!IsDATA) {
      weight = MC_Weight( ev , param , JBparam );
      weight *= Muon_Weight( param , dimuon );
  
      if(JBparam.BTagName!="") {
        if     (param.syst_ == AnalyzerParameter::BTagUp)   weight *= mcCorr->GetBTaggingReweight_1a({this_jet[0]}, JBparam.BTagParameter, "up");
        else if(param.syst_ == AnalyzerParameter::BTagDown) weight *= mcCorr->GetBTaggingReweight_1a({this_jet[0]}, JBparam.BTagParameter, "down");
        else                                                weight *= mcCorr->GetBTaggingReweight_1a({this_jet[0]}, JBparam.BTagParameter);
      }
  
      /*
      if( MCSample.Index("TT")!=kNPOS && JBparam.DileptonSign == "SS" ) {
        vector<Gen> gens = GetGens();
        weight *= mcCorr->GetTopPtReweight(gens);
      }
      */
    }
    // signal weight //
    // if( MCSample.Index("Zp")!=kNPOS ) weight *= 0.001;

    ///////////////////////////
    // --- Main Plotters --- //
    ///////////////////////////
    double DimuonMass = (*dimuon[0] + *dimuon[1]).M();
    if( DimuonMass < 2. ) return;
    double DimuonDR = dimuon[0]->DeltaR(*dimuon[1]);
    double DimuonPt = (*dimuon[0]+*dimuon[1]).Pt();
    double DimuonHT = dimuon[0]->Pt()+dimuon[1]->Pt();

    bool RejectUpsilon = (DimuonMass < 9. || 10.4 < DimuonMass);
    if( JBparam.DileptonSign == "SS" || JBparam.BTagName.Index("LightJet")!=kNPOS || JBparam.BTagName.Index("VetoBJet")!=kNPOS || RejectUpsilon ) {
      FillHist(this_region+"/Dilepton_Mass___"+this_region, DimuonMass, weight, 7500, 0., 150.);
      LeptonPlotter( dimuon, this_region, weight );
      JetPlotter( this_jet, {}, this_region, weight );

      //FillHist(this_region+"/ptratio___"+this_region, DimuonPt/this_jet[0].Pt(), weight, 100, 0., 1.);
      /*
      if( DimuonMass > 5. && RejectUpsilon && JBparam.DileptonSign == "OS" && !RunSyst && !RunXSecSyst ) {
        //FillHist(this_region+"/Dilepton_Mass_ProbeJet_pT___"+this_region, DimuonMass, this_jet[0].Pt(), weight, 140, 10., 150., 80,0.,2000.);
        FillHist(this_region+"/Dilepton_Mass_Dilepton_pT___"+this_region, DimuonMass, DimuonPt, weight, 7250, 5., 150., 80,0.,2000.);
        //FillHist(this_region+"/Dilepton_Mass_Dilepton_HT___"+this_region, DimuonMass, DimuonHT, weight, 140, 10., 150., 80,0.,2000.);
        //FillHist(this_region+"/Dilepton_Mass_TagJet_pT___"+this_region, DimuonMass, this_jet[1].Pt(), weight, 140, 10., 150., 80,0.,2000.);
        //FillHist(this_region+"/Dilepton_Mass_ProbeOverTagJet_pTratio___"+this_region, DimuonMass, this_jet[0].Pt()/this_jet[1].Pt(), weight, 140, 10., 150., 50,0.,5.);
      }
      */
    }

    /*
    /////////////////////////////
    // --- Lepton Plotters --- //
    /////////////////////////////
    if(1.2 < DimuonMass && DimuonMass < 2.9) {
      LeptonPlotter( dimuon, this_region+"_CR1", weight );
    }
    else if(3. < DimuonMass && DimuonMass < 3.2) {
      LeptonPlotter( dimuon, this_region+"_CR-jpsi", weight );
    }
    else if(3.3 < DimuonMass && DimuonMass < 3.6) {
      LeptonPlotter( dimuon, this_region+"_CR2", weight );
    }
    else if(3.66 < DimuonMass && DimuonMass < 3.72) {
      LeptonPlotter( dimuon, this_region+"_CR-psip", weight );
    }
    else if(3.8 < DimuonMass && DimuonMass < 5.) {
      LeptonPlotter( dimuon, this_region+"_CR3", weight );
    }
    else if(5. < DimuonMass && DimuonMass < 9.) {
      LeptonPlotter( dimuon, this_region+"_LMCR", weight );
    }
    else if( 11. < DimuonMass ) {
      LeptonPlotter( dimuon, this_region+"_SR", weight );
    }
    */

    ///////////////////////
    // --- MET study --- //
    ///////////////////////
    /*
    if( PassMETFilter() ) {
      Particle MET = ev.GetMETVector();
      
      FillHist(this_region+"/MET_Pt___"+this_region, MET.Pt(), weight, 100,0.,200.);
      FillHist(this_region+"/JetMET_DeltaPhi___"+this_region, fabs(MET.Phi()-this_jet[0].Phi()), weight, 32, 0, 3.2);
      if( 10.4 < DimuonMass && DimuonMass < 12. ) {
        FillHist(this_region+"_M11/MET_Pt___"+this_region+"_M11", MET.Pt(), weight, 1000,0.,1000.);
        FillHist(this_region+"_M11/JetMET_DeltaPhi___"+this_region+"_M11", fabs(MET.Phi()-this_jet[0].Phi()), weight, 320, 0, 3.2);
      }
      else if( 15. < DimuonMass && DimuonMass < 25.) {
        FillHist(this_region+"_M20/MET_Pt___"+this_region+"_M20", MET.Pt(), weight, 1000,0.,1000.);
        FillHist(this_region+"_M20/JetMET_DeltaPhi___"+this_region+"_M20", fabs(MET.Phi()-this_jet[0].Phi()), weight, 320, 0, 3.2);
      }
      else if( 35. < DimuonMass && DimuonMass < 45.) {
        FillHist(this_region+"_M40/MET_Pt___"+this_region+"_M40", MET.Pt(), weight, 1000,0.,1000.);
        FillHist(this_region+"_M40/JetMET_DeltaPhi___"+this_region+"_M40", fabs(MET.Phi()-this_jet[0].Phi()), weight, 320, 0, 3.2);
      }
      else if( 50. < DimuonMass && DimuonMass < 70.) {
        FillHist(this_region+"_M60/MET_Pt___"+this_region+"_M60", MET.Pt(), weight, 1000,0.,1000.);
        FillHist(this_region+"_M60/JetMET_DeltaPhi___"+this_region+"_M60", fabs(MET.Phi()-this_jet[0].Phi()), weight, 320, 0, 3.2);
      }
      //if( JBparam.METCutName =! "" ) {
      //  if( fabs( MET.Phi() - this_jet[0].Phi() < 0.3) ) {
      //    JBparam.AnalysisName = "NIsoDimuonMET";
      //    this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;
        
      //    // LeptonPlotter( dimuon, this_region, weight );
      //    JetPlotter(this_jet, this_fatjet, this_region, weight);
      //    DileptonPlotter( dimuon , this_region, weight );
      //    // FillHist(+"/MET_Pt___"+this_region, MET.Pt(), weight, 1000,0.,1000.);
      //    FillHist(this_region+"/DileptonMET_Mass___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
      //  }
  
      //  FillHist(this_region+"/MET_Pt_Mu_Pt___"+this_region, MET.Pt(), dimuon[0]->Pt(), weight, 50,0.,1000., 50,0.,1000.);
      //}
    }
    */

    ///////////////////////
    // --- Jet study --- //
    ///////////////////////
    //FillHist(this_region+"/Jet_mult___"+this_region, alljets.size(), weight, 10,0,10);
    //FillHist(this_region+"/LightJet_mult___"+this_region, jets.size(), weight, 10,0,10);

    /*
    if( 10.4 < DimuonMass && DimuonMass < 12. ) {
      FillHist(this_region+"_M11/Jet_mult___"+this_region+"_M11", alljets.size(), weight, 10,0,10);
      FillHist(this_region+"_M11/BJet_mult___"+this_region+"_M11", jets.size(), weight, 10,0,10);
      double dR=9999;
      for(auto jet:jets) {
        FillHist(this_region+"_M11/BJetJet_dR___"+this_region+"_M11", jet.DeltaR(this_jet[0]), weight, 100,0,5.);
        if(dR>jet.DeltaR(this_jet[0])) dR=jet.DeltaR(this_jet[0]);
      }
      FillHist(this_region+"_M11/BJetJet_dRclose___"+this_region+"_M11", dR, weight, 100,0,5.);
    }
    else if( 15. < DimuonMass && DimuonMass < 25.) {
      FillHist(this_region+"_M20/Jet_mult___"+this_region+"_M20", alljets.size(), weight, 10,0,10);
      FillHist(this_region+"_M20/BJet_mult___"+this_region+"_M20", jets.size(), weight, 10,0,10);
      double dR=9999;
      for(auto jet:jets) {
        FillHist(this_region+"_M11/BJetJet_dR___"+this_region+"_M20", jet.DeltaR(this_jet[0]), weight, 100,0,5.);
        if(dR>jet.DeltaR(this_jet[0])) dR=jet.DeltaR(this_jet[0]);
      }
      FillHist(this_region+"_M11/BJetJet_dRclose___"+this_region+"_M20", dR, weight, 100,0,5.);
    }
    else if( 35. < DimuonMass && DimuonMass < 45.) {
      FillHist(this_region+"_M40/Jet_mult___"+this_region+"_M40", alljets.size(), weight, 10,0,10);
      FillHist(this_region+"_M40/BJet_mult___"+this_region+"_M40", jets.size(), weight, 10,0,10);
      double dR=9999;
      for(auto jet:jets) {
        FillHist(this_region+"_M11/BJetJet_dR___"+this_region+"_M40", jet.DeltaR(this_jet[0]), weight, 100,0,5.);
        if(dR>jet.DeltaR(this_jet[0])) dR=jet.DeltaR(this_jet[0]);
      }
      FillHist(this_region+"_M11/BJetJet_dRclose___"+this_region+"_M40", dR, weight, 100,0,5.);
    }
    else if( 50. < DimuonMass && DimuonMass < 70.) {
      FillHist(this_region+"_M60/Jet_mult___"+this_region+"_M60", alljets.size(), weight, 10,0,10);
      FillHist(this_region+"_M60/BJet_mult___"+this_region+"_M60", jets.size(), weight, 10,0,10);
      double dR=9999;
      for(auto jet:jets) {
        FillHist(this_region+"_M11/BJetJet_dR___"+this_region+"_M60", jet.DeltaR(this_jet[0]), weight, 100,0,5.);
        if(dR>jet.DeltaR(this_jet[0])) dR=jet.DeltaR(this_jet[0]);
      }
      FillHist(this_region+"_M11/BJetJet_dRclose___"+this_region+"_M60", dR, weight, 100,0,5.);
    }
    */
  }

  return;

  //////////////////////
  // --- DY study --- //
  //////////////////////
  /*
  if( MCAnalysis && MCSample.Index("DY")!=kNPOS ) {
    cout<<endl<<endl<<"====================================================================="<<endl<<endl;
    cout<<"Event run: "<<run<<", lumi: "<<lumi<<", event: "<<event<<endl;
    cout<<"b category: "<<JBparam.BTagName<<endl;

    cout<<endl<<"# LHE #"<<endl;
    vector<LHE> lhes = GetLHEs();
    vector<LHE> LHEMuons;
    for(unsigned ilhe=0; ilhe<lhes.size(); ilhe++) {
      LHE lhe = lhes[ilhe];
      lhe.Print();
      if(abs(lhe.ID())==13) LHEMuons.push_back(lhe);
    }
    if(LHEMuons.size()!=2) cout<<" *** LHEMuons.size() == "<<LHEMuons.size()<<endl;
    else {
      cout<<" LHE Dimuon M = "<<(LHEMuons[0]+LHEMuons[1]).M()<<",  Pt = "<<(LHEMuons[0]+LHEMuons[1]).Pt()<<endl;
      FillHist(this_region+"/LHEDimuon_Pt___"+this_region, (LHEMuons[0]+LHEMuons[1]).Pt(), weight, 100, 0., 1000.);
    }

    vector<Gen> gens = GetGens();
    vector<bool> muon_gen_match={false,false};
    vector<Gen> GenMuons;
    Gen* Z = NULL;
    vector<Gen> GenZMuons;
    int Leading_Gen = 9999;
  
    for(unsigned igen=0; igen<gens.size(); igen++) {
      Gen gen = gens[igen];
      if( gen.isHardProcess() && gen.PID()==23 ) Z = &gen;
      if( gen.fromHardProcessFinalState() && abs(gen.PID())==13 ) GenZMuons.push_back(gen);
      if( !(gen.isLastCopy()&&abs(gen.PID())==13) ) continue;
      if( !(gen.Pt()>10) ) continue;
      for(unsigned iMu=0; iMu<dimuon.size(); iMu++) {
        if( !(gen.Charge()==dimuon[iMu]->Charge()) ) continue;
        if( !( (dimuon[iMu]->DeltaR(gen)<0.02&&fabs(dimuon[iMu]->Pt()/gen.Pt()-1)<0.3) || 
               (dimuon[iMu]->DeltaR(gen)<0.3&&fabs(dimuon[iMu]->Pt()/gen.Pt()-1)<0.1) )
          ) continue;
        if( Leading_Gen == gen.Index() ) continue;
        Leading_Gen = gen.Index();
        muon_gen_match[iMu]=true;
        GenMuons.push_back(gen);
      }
      if(muon_gen_match[0]&&muon_gen_match[1]) break;
    }
    cout<<endl<<" ### ------------------------------------------------------------- ###"<<endl;
    if(GenMuons.size()!=2) {
      cout<<"[ERROR] No 2 gen muons"<<endl;
      return;
    }

    if( Z != NULL ) FillHist(this_region+"/Z_Pt___"+this_region, Z->Pt(), weight, 100, 0., 1000.);
    if( GenZMuons.size()==2 ) FillHist(this_region+"/GenZDimuon_Pt___"+this_region, (GenZMuons[0]+GenZMuons[1]).Pt(), weight, 100, 0., 1000.);
    FillHist(this_region+"/GenDimuon_Pt___"+this_region, (GenMuons[0]+GenMuons[1]).Pt(), weight, 100, 0., 1000.);

    cout<<endl<<"# RECO #"<<endl;
    cout<<"Dimuon: M = "<<DimuonMass<<", Pt = "<<(*dimuon[0]+*dimuon[1]).Pt()<<", "; if(dimuon[0]->Charge()==dimuon[1]->Charge()) cout<<"SS"<<endl; else cout<<"OS"<<endl;
    cout<<"mu1: "<<dimuon[0]->Charge()<<" "; PrintPtEtaPhi(*dimuon[0]); cout<<" iso="<<dimuon[0]->RelIso()<<endl;
    cout<<"mu2: "<<dimuon[1]->Charge()<<" "; PrintPtEtaPhi(*dimuon[1]); cout<<" iso="<<dimuon[1]->RelIso()<<endl<<endl;
    cout<<"jet: "; PrintPtEtaPhi(this_jet[0]); cout<<" chf="<<this_jet[0].chargedHadronFraction()<<" nhf="<<this_jet[0].neutralHadronFraction()<<" nEMf="<<this_jet[0].neutralEmFraction()<<" cEMf="<<this_jet[0].chargedEmFraction()<<" muf="<<this_jet[0].muonFraction()<<" cMul="<<this_jet[0].chargedMultiplicity()<<" nMul="<<this_jet[0].neutralMultiplicity(); cout<<endl;


    cout<<endl<<"# GEN #"<<endl;
    if( Z!= NULL ) { cout<<"Z: "; PrintPtEtaPhi(*Z); cout<<", M = "<<Z->M()<<endl; }
    else cout<<"Z: NULL"<<endl;
    cout<<"Dimuon(hard): M= "<<(GenMuons[0]+GenMuons[1]).M()<<", Pt = "<<(GenMuons[0]+GenMuons[1]).Pt()<<endl;
    if( GenZMuons.size()==2 ) cout<<"Dimuon(HPFS): M= "<<(GenZMuons[0]+GenZMuons[1]).M()<<", Pt = "<<(GenZMuons[0]+GenZMuons[1]).Pt()<<endl;
    cout<<endl;
    for(unsigned i=0; i<GenMuons.size(); i++) {
      Gen gen = GenMuons[i];
      PrintGen(gen, gens);
      cout<<"mu";
      vector<Gen> history = TrackMotherParticles(gen,gens);
      for(unsigned j=0; j<history.size(); j++) {
        Gen mother = history[j];
        cout<<" - "; GenName(mother); cout<<" ("<<mother.Index()<<")";
      }
      cout<<endl;
    }
    cout<<endl;
    PrintAllGens(gens);
    cout<<endl<<endl;
  }

  // --------------------------- //
  // --- DY Pt binned sample --- //
  // --------------------------- //
  //if( MCSample.Index("DYJets_Pt")!=kNPOS ) {
  //}
  */

  // ---------------------------------------------------------------//
  // --- Gen-level heavy flavor matching --- //
  // ---------------------------------------------------------------//
  /*
  if( MCAnalysis && MCSample.Index("QCD")!=kNPOS ) {
    JBparam.AnalysisName = "NIsoDimuonHF";
    this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;
  
    vector<Gen> gens = GetGens();
    vector<bool> muon_gen_match={false,false};
    //vector<bool> muon_from_b={false,false};
    //vector<bool> muon_from_c={false,false};
    vector<Gen> GenMuons;
    int Leading_Gen = 9999;
  
    for(unsigned igen=0; igen<gens.size(); igen++) {
      Gen gen = gens[igen];
      if( !(gen.isLastCopy()&&abs(gen.PID())==13) ) continue;
      if( !(gen.Pt()>10) ) continue;
      for(unsigned iMu=0; iMu<dimuon.size(); iMu++) {
        if( !(gen.Charge()==dimuon[iMu]->Charge()) ) continue;
        if( !( (dimuon[iMu]->DeltaR(gen)<0.02&&fabs(dimuon[iMu]->Pt()/gen.Pt()-1)<0.3) || 
               (dimuon[iMu]->DeltaR(gen)<0.3&&fabs(dimuon[iMu]->Pt()/gen.Pt()-1)<0.1) )
          ) continue;
        //muon_gen_match[(dimuon[iMu]->Charge()+1)/2]=true;
        //if( BottomMesonDecay(gen,gens) || bbbarDecay(gen,gens) || bquarkDecay(gen,gens) ) { muon_from_b[(dimuon[iMu]->Charge()+1)/2]=true; }
        //else if( CharmMesonDecay(gen,gens) || ccbarDecay(gen,gens) || cquarkDecay(gen,gens) ) { muon_from_c[(dimuon[iMu]->Charge()+1)/2]=true; }
        //if( BottomMesonDecay(gen,gens) || bbbarDecay(gen,gens) || bquarkDecay(gen,gens) ) { muon_from_b[iMu]=true; }
        //else if( CharmMesonDecay(gen,gens) || ccbarDecay(gen,gens) || cquarkDecay(gen,gens) ) { muon_from_c[iMu]=true; }
        if( Leading_Gen == gen.Index() ) continue;
        Leading_Gen = gen.Index();
        muon_gen_match[iMu]=true;
        GenMuons.push_back(gen);
      }
      if(muon_gen_match[0]&&muon_gen_match[1]) break;
    }
    cout<<endl<<" ### ------------------------------------------------------------- ###"<<endl;
    if(GenMuons.size()!=2) {
      cout<<"[ERROR] No 2 gen muons"<<endl;
      return;
    }
    cout<<endl;
    cout<<"M(mu,mu) = "<<DimuonMass<<", "; if(dimuon[0]->Charge()==dimuon[1]->Charge()) cout<<"SS"<<endl; else cout<<"OS"<<endl;
    cout<<"mu1: "<<dimuon[0]->Charge()<<" "; PrintPtEtaPhi(*dimuon[0]); cout<<endl;
    cout<<"mu2: "<<dimuon[1]->Charge()<<" "; PrintPtEtaPhi(*dimuon[1]); cout<<endl<<endl;
    //cout<<"B match: "<<muon_from_b[0]<<", "<<muon_from_b[1]<<endl;
    //cout<<"D match: "<<muon_from_c[0]<<", "<<muon_from_c[1]<<endl;
    cout<<endl;
    for(unsigned i=0; i<GenMuons.size(); i++) {
      Gen gen = GenMuons[i];
      PrintGen(gen, gens);
      cout<<"mu";
      vector<Gen> history = TrackMotherParticles(gen,gens);
      for(unsigned j=0; j<history.size(); j++) {
        Gen mother = history[j];
        cout<<" - "; GenName(mother); cout<<" ("<<mother.Index()<<")";
      }
      cout<<endl;
    }
    cout<<endl; 
    if(fromSingleB(GenMuons,gens)) {
      DileptonPlotter( dimuon , this_region+"_SingleB", weight );
      FillHist(this_region+"_SingleB"+"/LeadingLeptonJet_DeltaR___"+this_region+"_SingleB", dimuon[0]->DeltaR( this_jet[0] ), weight, 400, 0., .4);
      FillHist(this_region+"_SingleB"+"/SubLeptonJet_DeltaR___"+this_region+"_SingleB", dimuon[1]->DeltaR( this_jet[0] ), weight, 400, 0., .4);
      cout<<" ### Single B ###"<<endl<<endl;
    }
    else if(fromSingleD(GenMuons,gens)) {
      DileptonPlotter( dimuon , this_region+"_SingleD", weight );
      FillHist(this_region+"_SingleD"+"/LeadingLeptonJet_DeltaR___"+this_region+"_SingleD", dimuon[0]->DeltaR( this_jet[0] ), weight, 400, 0., .4);
      FillHist(this_region+"_SingleD"+"/SubLeptonJet_DeltaR___"+this_region+"_SingleD", dimuon[1]->DeltaR( this_jet[0] ), weight, 400, 0., .4);
      cout<<" ### Single D ###"<<endl<<endl;
    }
    else if(fromBBbar(GenMuons,gens)) {
      DileptonPlotter( dimuon , this_region+"_BBbar", weight );
      cout<<" ### BBbar ###"<<endl<<endl;
    }
    else if(fromDDbar(GenMuons,gens)) {
      DileptonPlotter( dimuon , this_region+"_DDbar", weight );
      cout<<" ### DDbar ###"<<endl<<endl;
    }
    else {
      DileptonPlotter( dimuon , this_region+"_others", weight );
      FillHist(this_region+"_others"+"/LeadingLeptonJet_DeltaR___"+this_region+"_others", dimuon[0]->DeltaR( this_jet[0] ), weight, 400, 0., .4);
      FillHist(this_region+"_others"+"/SubLeptonJet_DeltaR___"+this_region+"_others", dimuon[1]->DeltaR( this_jet[0] ), weight, 400, 0., .4);
      cout<<" ### Others ###"<<endl<<endl;
    }
    PrintAllGens(gens);
    cout<<endl<<endl;
  }
  */
  /*
  for(unsigned i=0; i<2; i++) {
    Muon* muon = (Muon*)dimuon[i];
    if( 10 < muon->Pt() && muon->Pt() < 15 ) {
      FillHist(this_region+"/Lepton10_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton10_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton10_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton10_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 20 < muon->Pt() && muon->Pt() < 25 ) {
      FillHist(this_region+"/Lepton20_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton20_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton20_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton20_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 30 < muon->Pt() && muon->Pt() < 40 ) {
      FillHist(this_region+"/Lepton30_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton30_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton30_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton30_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 50 < muon->Pt() && muon->Pt() < 70 ) {
      FillHist(this_region+"/Lepton50_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton50_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton50_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton50_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 100 < muon->Pt() && muon->Pt() < 150 ) {
      FillHist(this_region+"/Lepton100_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton100_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton100_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton100_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
  }
  */


  ///////////////////////
  // --- QCD study --- //
  ///////////////////////
  /*
  if( MCSample.Index("QCD")==kNPOS ) return;
  if( !( ( *dimuon[0] + *dimuon[1] ).M() > 12 ) ) return;

  cout<<fixed;
  cout<<setprecision(2);
  cout<<endl<<endl;
  cout<<"+++++++++++++++++++++++++++++++ "<<JBparam.DileptonSign<<" M="<<(*dimuon[0]+*dimuon[1]).M()<<endl;
  for(unsigned iMu=0; iMu<dimuon.size(); iMu++) {
    Muon* muon = (Muon*)dimuon[iMu];
    cout<<"Mu  ";
    PrintPtEtaPhi( *muon );
    cout<<" iso: "<<setw(6)<<muon->TrkIso()/muon->Pt()<<endl;
  }
  cout<<"Jet ";
  PrintPtEtaPhi( this_jet[0] );
  cout<<" mu frac: "<<this_jet[0].muonFraction()<<endl;
  cout<<"MET ";
  PrintPtEtaPhi( MET );
  cout<<endl;

  cout<<" - - - - - - - - - - - - - - - "<<endl;
  cout<<"Event run: "<<run<<", event: "<<event<<endl;
  vector<Gen> gens = GetGens();
  vector<int> gennums;
  if( gens.size() == 0 ) return;
  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];

    if( !(gen.Status()==1&&gen.MotherIndex()!=0&&gen.MotherIndex()!=1) ) continue;
    if( !(this_jet[0].DeltaR( gen ) < 0.5 && gen.Pt() > 5) ) continue;

    gennums.push_back( iGen );
    vector<Gen> History = TrackMotherParticles(gen, gens);

    for(unsigned iHistory=0; iHistory<History.size(); iHistory++) {
      Gen TrackingGen = History[iHistory];
      GenName( TrackingGen );
      cout<<"<"<<setw(3)<<TrackingGen.Index()<<"> ";
      PrintPtEtaPhi( TrackingGen );
      cout<<" ~ ";
    }
    cout<<endl;
  }

  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];
    cout<<setw(3)<<gen.Index()<<"<"; GenName( gen ); cout<<"> status: "<<gen.Status()<<" PFS: "<<gen.isPromptFinalState()<<" ";
    PrintPtEtaPhi( gen );
    cout<<" <- "<<setw(3)<<gen.MotherIndex();
    for(unsigned igennum=0; igennum<gennums.size(); igennum++) {
      if( iGen == gennums[igennum] ) {
        cout<<"  *";
        break;
      }
    }
    cout<<endl;
  }
  */

}

void NIsoMuon::MuonIDEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> denominatorMuons, vector<Jet> jets, vector<Jet> alljets) {
  // This measures: NUM_TightID_DEN_TrackerMuons, in the analysis phase space.  
  JBparam.AnalysisName = "MuonIDEfficiency";
  const TString base_region = param.Name + JBparam.SystName + "_" + JBparam.AnalysisName;

  // Use the same two-dimensional input variables as the Muon POG correction:
  // probe |eta| and probe pT.  These defaults start from pT = 10 GeV as requested.
  // If you want exact one-to-one binning with a local JSON, run:
  //   correction summary POG/MUO/<era>_UL/muon_JPsi.json.gz
  // and replace the two vectors below with the JSON bin edges.

  const vector<double> ptEdges  = {10., 15., 20., 25., 30., 40., 50., 60., 120., 200., 2000.};
  const vector<double> etaEdges = {0., 0.9, 1.2, 2.1, 2.4};

  auto edgeLabel = [](double x) -> TString {
    TString out = Form("%.1f", x);
    out.ReplaceAll(".", "p");
    out.ReplaceAll("-", "m");
    return out;
  };

  auto binTag = [&](Muon probe) -> TString {
    const double pt = probe.Pt();
    const double abseta = fabs(probe.Eta());
    for(unsigned int ieta = 0; ieta + 1 < etaEdges.size(); ++ieta) {
      if( !(etaEdges[ieta] <= abseta && abseta < etaEdges[ieta+1]) ) continue;
      for(unsigned int ipt = 0; ipt + 1 < ptEdges.size(); ++ipt) {
        if( !(ptEdges[ipt] <= pt && pt < ptEdges[ipt+1]) ) continue;
        return "Pt" + edgeLabel(ptEdges[ipt]) + "to" + edgeLabel(ptEdges[ipt+1]) + "_AbsEta" + edgeLabel(etaEdges[ieta]) + "to" + edgeLabel(etaEdges[ieta+1]);
      }
    }
    return "";
  };

  auto fillProbe = [&](const TString &tag, const bool pass, const double mass, Muon tagMuon, Muon probe, Jet jet, const double weight) {
    const TString pf = pass ? "Pass" : "Fail";
    const TString region = base_region + "_" + tag + "_" + pf;

    FillHist(region + "/DileptonJPsi_Mass___" + region, mass, weight, 300, 2., 5.);
    FillHist(region + "/DileptonZ_Mass___"    + region, mass, weight, 200, 0., 200.);
    FillHist(region + "/Probe_Pt___"          + region, probe.Pt(), weight, 200, 0., 200.);
    FillHist(region + "/Probe_absEta___"      + region, fabs(probe.Eta()), weight, 48, 0., 2.4);
    FillHist(region + "/Tag_Pt___"            + region, tagMuon.Pt(), weight, 200, 0., 500.);
    FillHist(region + "/Tag_absEta___"        + region, fabs(tagMuon.Eta()), weight, 48, 0., 2.4);
    FillHist(region + "/BJet_Pt___"           + region, jet.Pt(), weight, 200, 0., 1000.);
    FillHist(region + "/ProbeBJet_DeltaR___"  + region, jet.DeltaR(probe), weight, 100, 0., 0.5);
    FillHist(region + "/TagBJet_DeltaR___"    + region, jet.DeltaR(tagMuon), weight, 100, 0., 0.5);
    FillHist(region + "/Dilepton_DeltaR___"    + region, tagMuon.DeltaR(probe), weight, 60, 0., 0.6);
  };

  double weight = 0.;
  bool EventSelection = false;

  // nominal selection //
  for(auto jet:alljets) {
    if( !(jet.Pt() > 30.) ) continue;
    for(auto tagMuon:denominatorMuons) {
      if( !(tagMuon.Pt() > Leading_Muon_Pt) ) return;
      if( !(fabs(tagMuon.Eta()) < 2.4) ) continue;
      if( !(tagMuon.PassID("POGMedium")) ) continue;

      bool PassHighPtTag = false;
      if(DataYear == 2016) {
        PassHighPtTag =
          tagMuon.PassPath("HLT_Mu50_v") ||
          tagMuon.PassPath("HLT_TkMu50_v");
      }
      else if(DataYear == 2017 || DataYear == 2018) {
        PassHighPtTag =
          tagMuon.PassPath("HLT_Mu50_v") ||
          tagMuon.PassPath("HLT_OldMu100_v") ||
          tagMuon.PassPath("HLT_TkMu100_v");
      }
      if(!PassHighPtTag) continue;

      if( !(jet.DeltaR(tagMuon) < dRmax) ) continue; // non-iso selection
      for(auto probe:denominatorMuons) {
        if( !(probe.Pt() > 10.) ) continue;
        if( !(fabs(probe.Eta()) < 2.4) ) continue;
        if( !(probe.isTrackerMuon()) ) continue;
        if( !(jet.DeltaR(probe) < dRmax) ) continue;
        if( !(tagMuon.DeltaR(probe) > 0.05) ) continue; // dimuon dR cut
        if( !DimuonCharge(JBparam, tagMuon, probe) ) continue;
        const double mass = (tagMuon + probe).M();
        if( !(2. < mass && mass < 5.) ) continue;

        EventSelection = true;
        weight = 1.;
        if(!IsDATA) weight = MC_Weight(ev, param, JBparam);

        const bool pass = probe.PassID("POGMedium");
        const TString thisBin = binTag(probe);
        if(thisBin != "") fillProbe(thisBin, pass, mass, tagMuon, probe, jet, weight);
        break;
      }
      if( EventSelection ) break;
    }
    if( EventSelection ) break;
  }
}

void NIsoMuon::TriggerEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets) {
  JBparam.AnalysisName = "TriggerEfficiency";
  TString this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;

  vector<Lepton*> ProbeMuons;
  Muon probe;
  vector<Jet> this_jet;

  // --- tag muon (iso) --- //
  bool PassTag = false; unsigned iTag=999;
  for(unsigned i=0; i<muons.size(); i++) {
    Muon muon = muons[i];
    if( !(muon.Pt() > Leading_Muon_Pt) ) return;
    if( !(muon.PassID("POGTightWithTightIso")) ) continue;

    bool PassIsolatedTag = false;
    if(DataYear == 2016) {
      PassIsolatedTag =
        muon.PassPath("HLT_IsoMu24_v") ||
        muon.PassPath("HLT_IsoTkMu24_v");
    }
    else if(DataYear == 2017) {
      PassIsolatedTag = muon.PassPath("HLT_IsoMu27_v");
    }
    else if(DataYear == 2018) {
      PassIsolatedTag = muon.PassPath("HLT_IsoMu24_v");
    }
    if(!PassIsolatedTag) continue;

    bool JetClean = true;
    for(auto jet:alljets) {
      if( jet.DeltaR(muon) < 0.5 ) {
        JetClean = false;
        break;
      }
    }
    if( JetClean ) {
      PassTag = true;
      iTag = i;
      break;
    }
  }
  if( !PassTag ) return;

  //////////////////////////////
  // --- Event selectioon --- //
  //////////////////////////////
  bool EventSelection = false;
  for(auto jet:alljets) {
    if( !(jet.Pt() > 30.) ) continue;
    for(unsigned i=0; i<muons.size(); i++) {
      if( i == iTag ) continue;
      Muon& muon = muons[i];
      if( !(muon.Pt() > 10.) ) return;
      if( !(muon.PassID("POGMedium")) ) continue;
      if( !(jet.DeltaR(muon) < dRmax) ) continue;
      EventSelection = true;
      ProbeMuons.push_back( &muon );
      this_jet.push_back( jet );
      probe=muons[i];
      break;
    }
    if( EventSelection ) break;
  }
  if( !EventSelection ) return;
  if( !(ProbeMuons.size()==1&&this_jet.size()==1) ) return;

  //////////////////////////
  // --- Event weight --- //
  //////////////////////////
  double weight = 1.;
  if(!IsDATA) {
    weight = MC_Weight( ev , param , JBparam );
    weight *= Muon_Weight( param , ProbeMuons );
    //if     (param.syst_ == AnalyzerParameter::BTagUp)   weight *= mcCorr->GetBTaggingReweight_1a({this_jet[0]}, JBparam.BTagParameter, "up");
    //else if(param.syst_ == AnalyzerParameter::BTagDown) weight *= mcCorr->GetBTaggingReweight_1a({this_jet[0]}, JBparam.BTagParameter, "down");
    //else                                                weight *= mcCorr->GetBTaggingReweight_1a({this_jet[0]}, JBparam.BTagParameter);
  }

  FillHist(this_region+"/Muon_Pt___"+this_region, ProbeMuons[0]->Pt(), weight, 500, 0., 500.);
  FillHist(this_region+"/Muon_Eta___"+this_region, fabs(ProbeMuons[0]->Eta()), weight, 50, 0., 1.);
  FillHist(this_region+"/Muon_dR___"+this_region, ProbeMuons[0]->DeltaR(this_jet[0]), weight, 60, 0., 3.);

  ////////////////////////////////
  // --- Trigger efficiency --- //
  ////////////////////////////////
  if(fabs(probe.Eta())<.9) FillHist(this_region+"/LeptonBarrel_Pt___"+this_region, probe.Pt(), weight, 500, 0., 500.);
  else if(fabs(probe.Eta())<1.2) FillHist(this_region+"/LeptonOverlap_Pt___"+this_region, fabs(probe.Pt()), weight, 500, 0., 500.);
  else if(fabs(probe.Eta())<2.1) FillHist(this_region+"/LeptonEndcap_Pt___"+this_region, fabs(probe.Pt()), weight, 500, 0., 500.);
  else FillHist(this_region+"/LeptonForward_Pt___"+this_region, fabs(probe.Pt()), weight, 500, 0., 500.);

  // Pass target HighPtMuon trigger.
  // Keep this OR consistent with TriggerSetup() and the SKNano implementation.
  bool PassTargetTrigger = false;
  if(DataYear == 2016) {
    PassTargetTrigger =
      probe.PassPath("HLT_Mu50_v") ||
      probe.PassPath("HLT_TkMu50_v");
  }
  else if(DataYear == 2017 || DataYear == 2018) {
    PassTargetTrigger =
      probe.PassPath("HLT_Mu50_v") ||
      probe.PassPath("HLT_OldMu100_v") ||
      probe.PassPath("HLT_TkMu100_v");
  }

  if(!PassTargetTrigger) return;

  if(fabs(probe.Eta())<.9) FillHist(this_region+"/LeptonBarrel_Pt___hlt___"+this_region, probe.Pt(), weight, 500, 0., 500.);
  else if(fabs(probe.Eta())<1.2) FillHist(this_region+"/LeptonOverlap_Pt___hlt___"+this_region, fabs(probe.Pt()), weight, 500, 0., 500.);
  else if(fabs(probe.Eta())<2.1) FillHist(this_region+"/LeptonEndcap_Pt___hlt___"+this_region, fabs(probe.Pt()), weight, 500, 0., 500.);
  else FillHist(this_region+"/LeptonForward_Pt___hlt___"+this_region, fabs(probe.Pt()), weight, 500, 0., 500.);
  return;
}

/*
void NIsoMuon::IsoEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets) {
  JBparam.AnalysisName = "IsoEfficiency";
  TString this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;

  //vector<Muon> niso_muons = SelectMuons(muons, "NonIsolatedMuon", Leading_Muon_Pt, 2.4);

  //Lepton *tag; Lepton *prb;
  //vector<Jet> this_jet;
  //vector<FatJet> this_fatjet;
  bool passing_probe = 0;

  ///////////////////////////
  // --- TnP selection --- // 
  ///////////////////////////
  //for(auto tag_:niso_muons) {
  for(int i=0; i<muons.size(); i++) {
    Muon prb_ = muons[i];
    if( !(prb_.Pt()>Leading_Muon_Pt) ) return;
    for(auto jet:jets) {
      if( !(jet.DeltaR(prb_) < 0.3) ) continue; 
      for(int j=i+1; j<muons.size(); j++) {
        Muon tag_ = muons[j];

        double DimuonMass = (tag_+prb_).M();
			  //if( !(fabs(DimuonMass-3.1) < 0.2) ) continue;
			  if( !( DimuonMass > 5. ) ) continue;
        if( !(jet.DeltaR(tag_) < 0.3) ) continue;
        if( !DimuonCharge(JBparam, tag_, prb_) ) continue;
        //if( tag_.Pt()==prb_.Pt() ) continue;

        //////////////////////////
        // --- Event weight --- //
        //////////////////////////
        double weight = 1.;
        if(!IsDATA) {
          weight = MC_Weight( ev , param , JBparam );
          //weight *= Muon_Weight( param , {tag, prb} );
        }

        FillHist(this_region+"/Njets___"+this_region, alljets.size(), weight, 10, 0, 10);
        FillHist(this_region+"/Nbjets___"+this_region, jets.size(), weight, 10, 0, 10);

        //if( prb_.PassID("NonIsolatedMuon") ) passing_probe = 1;
        if( prb_.PassID("NonIsolatedMuon") && tag_.PassID("NonIsolatedMuon") ) passing_probe = 1;
        double DimuonDeltaR = tag_.DeltaR( prb_ );
        double DR_lj = jet.DeltaR(prb_);

        FillHist(this_region+"/DileptonJPsi_Mass___"+this_region, DimuonMass, weight, 40, 2.9, 3.3);
        FillHist(this_region+"/Dilepton_Mass___"+this_region, DimuonMass, weight, 500, 0., 100.);
        FillHist(this_region+"/Dilepton_DeltaR___"+this_region, DimuonDeltaR, weight, 80, 0., 0.8);
        FillHist(this_region+"/LeptonJet_DeltaR___"+this_region, DR_lj, weight, 80, 0., 0.4);

        if(prb_.Pt()>tag_.Pt()) {
          FillHist(this_region+"/LeadingLeptonJet_DeltaR___"+this_region, DR_lj, weight, 80, 0., 0.4);
          FillHist(this_region+"/SubLeptonJet_DeltaR___"+this_region, jet.DeltaR(tag_), weight, 80, 0., 0.4);
        }
        else {
          FillHist(this_region+"/LeadingLeptonJet_DeltaR___"+this_region, jet.DeltaR(tag_), weight, 80, 0., 0.4);
          FillHist(this_region+"/SubLeptonJet_DeltaR___"+this_region, DR_lj, weight, 80, 0., 0.4);
        }

        if(fabs(DimuonMass-3.1)<0.2) {
          FillHist(this_region+"/DileptonJPsi_DeltaR___"+this_region, DimuonDeltaR, weight, 80, 0., 0.8); 
          FillHist(this_region+"/LeptonJPsi_Iso___"+this_region, prb_.RelIso(), weight, 100, 0., 5.);
        }
        else if(DimuonMass > 5.) {
          FillHist(this_region+"/DileptonAbove5_DeltaR___"+this_region, DimuonDeltaR, weight, 80, 0., 0.8);
          FillHist(this_region+"/Lepton_Iso___"+this_region, prb_.RelIso(), weight, 100, 0., 5.);
          FillHist(this_region+"/Lepton_TrkIso___"+this_region, prb_.TrkIso()/prb_.Pt(), weight, 100, 0., 5.);

          double JetPt_NoMuon = jet.Pt() - prb_.Pt() - tag_.Pt();
          FillHist(this_region+"/Jet_Pt___"+this_region, jet.Pt(), weight, 100, 0., 1000.);
          FillHist(this_region+"/JetNoMuon_Pt___"+this_region, JetPt_NoMuon, weight, 100, 0., 1000.);

          if( tag_.Pt() < 20 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_Mu2Pt-0to20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_Mu2Pt-0to20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_Mu2Pt-0to20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_Mu2Pt-0to20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_Mu2Pt-0to20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_Mu2Pt-0to20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if( 20 < tag_.Pt() && tag_.Pt() < 30 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_Mu2Pt-20to30_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_Mu2Pt-20to30_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_Mu2Pt-20to30_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_Mu2Pt-20to30_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_Mu2Pt-20to30_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_Mu2Pt-20to30_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if( 30 < tag_.Pt() && tag_.Pt() < 50 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_Mu2Pt-30to50_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_Mu2Pt-30to50_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_Mu2Pt-30to50_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_Mu2Pt-30to50_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_Mu2Pt-30to50_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_Mu2Pt-30to50_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if( 50 < tag_.Pt() && tag_.Pt() < 100 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_Mu2Pt-50to100_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_Mu2Pt-50to100_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_Mu2Pt-50to100_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_Mu2Pt-50to100_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_Mu2Pt-50to100_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_Mu2Pt-50to100_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_Mu2Pt-100toInf_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_Mu2Pt-100toInf_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_Mu2Pt-100toInf_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_Mu2Pt-100toInf_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_Mu2Pt-100toInf_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_Mu2Pt-100toInf_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }

          double DR_l2j = jet.DeltaR(tag_);
          if( DR_l2j < 0.05 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR2-0top05_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR2-0top05_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR2-0top05_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR2-0top05_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR2-0top05_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR2-0top05_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.05 < DR_l2j && DR_l2j < 0.1 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR2-p05top10_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR2-p05top10_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR2-p05top10_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR2-p05top10_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR2-p05top10_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR2-p05top10_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.1 < DR_l2j && DR_l2j < 0.15 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR2-p10top15_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR2-p10top15_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR2-p10top15_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR2-p10top15_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR2-p10top15_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR2-p10top15_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.15 < DR_l2j && DR_l2j < 0.20 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR2-p15top20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR2-p15top20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR2-p15top20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR2-p15top20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR2-p15top20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR2-p15top20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.2 < DR_l2j && DR_l2j < 0.25 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR2-p20top25_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR2-p20top25_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR2-p20top25_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR2-p20top25_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR2-p20top25_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR2-p20top25_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.25 < DR_l2j && DR_l2j < 0.3 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR2-p25top30_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR2-p25top30_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR2-p25top30_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR2-p25top30_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR2-p25top30_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR2-p25top30_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }

          double DR_ll = prb_.DeltaR(tag_);
          if( DR_ll < 0.1 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dRll-0top10_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dRll-0top10_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dRll-0top10_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dRll-0top10_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dRll-0top10_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dRll-0top10_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.1 < DR_ll && DR_ll < 0.2 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dRll-p10top20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dRll-p10top20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dRll-p10top20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dRll-p10top20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dRll-p10top20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dRll-p10top20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.2 < DR_ll && DR_ll < 0.3 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dRll-p20top30_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dRll-p20top30_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dRll-p20top30_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dRll-p20top30_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dRll-p20top30_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dRll-p20top30_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.3 < DR_ll && DR_ll < 0.4 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dRll-p30top40_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dRll-p30top40_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dRll-p30top40_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dRll-p30top40_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dRll-p30top40_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dRll-p30top40_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.4 < DR_ll && DR_ll < 0.6 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dRll-p40top60_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dRll-p40top60_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dRll-p40top60_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dRll-p40top60_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dRll-p40top60_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dRll-p40top60_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }


          if( DR_lj < 0.05 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-0top05_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-0top05_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-0top05_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-0top05_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-0top05_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-0top05_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.05 < DR_lj && DR_lj < 0.1 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p05top10_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.1 < DR_lj && DR_lj < 0.15 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p10top15_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.15 < DR_lj && DR_lj < 0.20 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p15top20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.2 < DR_lj && DR_lj < 0.25 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p15top20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(0.25 < DR_lj && DR_lj < 0.3 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p25top30_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }


          if(JetPt_NoMuon < 80 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(80 < JetPt_NoMuon && JetPt_NoMuon < 120 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(120 < JetPt_NoMuon && JetPt_NoMuon < 160 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(160 < JetPt_NoMuon && JetPt_NoMuon < 200 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(200 < JetPt_NoMuon && JetPt_NoMuon < 300 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(300 < JetPt_NoMuon && JetPt_NoMuon < 500 ) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }
          else if(500 < JetPt_NoMuon) {
            if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
            else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
          }



        }
        FillHist(this_region+"/Lepton_Pt___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
        FillHist(this_region+"/Lepton_Eta___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
        if(prb_.Pt()>tag_.Pt()) {
          FillHist(this_region+"/LeadingLepton_Pt___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
          FillHist(this_region+"/LeadingLepton_Eta___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
        }
        else {
          FillHist(this_region+"/SubLepton_Pt___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
          FillHist(this_region+"/SubLepton_Eta___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
        }

        if(passing_probe) {
          FillHist(this_region+"/DileptonJPsi_Mass___pass___"+this_region, DimuonMass, weight, 40, 2.9, 3.3);
          FillHist(this_region+"/Dilepton_Mass___pass___"+this_region, DimuonMass, weight, 500, 0., 100.);
          FillHist(this_region+"/Dilepton_DeltaR___pass___"+this_region, DimuonDeltaR, weight, 80, 0., 0.8);
          FillHist(this_region+"/LeptonJet_DeltaR___pass___"+this_region, DR_lj, weight, 80, 0., 0.4);

          if(prb_.Pt()>tag_.Pt()) {
            FillHist(this_region+"/LeadingLeptonJet_DeltaR___pass___"+this_region, DR_lj, weight, 80, 0., 0.4);
            FillHist(this_region+"/SubLeptonJet_DeltaR___pass___"+this_region, jet.DeltaR(tag_), weight, 80, 0., 0.4);
          }
          else {
            FillHist(this_region+"/LeadingLeptonJet_DeltaR___pass___"+this_region, jet.DeltaR(tag_), weight, 80, 0., 0.4);
            FillHist(this_region+"/SubLeptonJet_DeltaR___pass___"+this_region, DR_lj, weight, 80, 0., 0.4);
          }

          if(fabs(DimuonMass-3.1)<0.2) {
            FillHist(this_region+"/DileptonJPsi_DeltaR___pass___"+this_region, DimuonDeltaR, weight, 80, 0., 0.8);
            FillHist(this_region+"/LeptonJPsi_Iso___pass___"+this_region, prb_.RelIso(), weight, 100, 0., 5.);
          }
          else if(DimuonMass > 5.) {
            FillHist(this_region+"/DileptonAbove5_DeltaR___pass___"+this_region, DimuonDeltaR, weight, 80, 0., 0.8);
            FillHist(this_region+"/Lepton_Iso___pass___"+this_region, prb_.RelIso(), weight, 100, 0., 5.);
          }
          FillHist(this_region+"/Lepton_Pt___pass___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
          FillHist(this_region+"/Lepton_Eta___pass___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
          if(prb_.Pt()>tag_.Pt()) {
            FillHist(this_region+"/LeadingLepton_Pt___pass___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
            FillHist(this_region+"/LeadingLepton_Eta___pass___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
          }
          else {
            FillHist(this_region+"/SubLepton_Pt___pass___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
            FillHist(this_region+"/SubLepton_Eta___pass___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
          }
        }

        // Loop over bins
//        bool fillhist=0;
//        for(int i_dR = 0; i_dR < NdRBin; ++i_dR) {
//          for(int i_pt = 0; i_pt < NPtBin; ++i_pt) {
//            if( edge_dR[i_dR] < DimuonDeltaR && DimuonDeltaR < edge_dR[i_dR+1] &&
//                edge_pt[i_pt] < prb->Pt() && prb->Pt() < edge_pt[i_pt+1]) {
//      
//              // Create a unique name for the histogram, using format_bin for clean formatting
//      				TString hist_name = "/Dilepton_Mass___dR" + format_bin100(edge_dR[i_dR]) +
//                          "to" + format_bin100(edge_dR[i_dR+1]) +
//                          "_pt" + format_bin(edge_pt[i_pt]) +
//                          "to" + format_bin(edge_pt[i_pt+1]);
//      
//              // Fill the histogram for the current dR and pt bin
//              FillHist(this_region+hist_name+"___"+this_region, DimuonMass, weight, 40, 2.9, 3.3);
//              if(passing_probe) FillHist(this_region+hist_name+"___pass___"+this_region, DimuonMass, weight, 40, 2.9, 3.3);
//              fillhist=1;
//              break;
//            }
//          }
//          if(fillhist) break;
//        }
      }
      break;
    }
  }
}

void NIsoMuon::SingleMuonIsoEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets) {
  JBparam.AnalysisName = "SingleMuonIsoEfficiency";
  TString this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;

  if(jets.size() < 2) return;
  if(muons.size() != 2) return;

  //vector<Muon> niso_muons = SelectMuons(muons, "NonIsolatedMuon", Subleading_Muon_Pt, 2.4);

  //Lepton *tag; Lepton *prb;
  //vector<Jet> this_jet;
  //vector<FatJet> this_fatjet;
  bool passing_probe = 0;

  ///////////////////////////
  // --- TnP selection --- // 
  ///////////////////////////
  //for(auto tag_:niso_muons) {
  for(auto tag_:muons) {
    if( !(tag_.Pt() > Leading_Muon_Pt) ) return;
    if( !(tag_.RelIso() < 0.25) ) continue; // PFIsoLoose

    bool JetCleaning = true;
    for(auto jet:alljets) {
      if( jet.DeltaR(tag_) < 0.5 ) {
        JetCleaning=false;
        break;
      }
    }
    if( !JetCleaning ) break;

    for(auto prb_:muons) {
      if( tag_.Pt()==prb_.Pt() ) continue;

      double OpeningAngle = 999.;
      Jet jet_;
      for(auto jet:jets) {
        if( jet.DeltaR(prb_) < 0.3 ) {
          OpeningAngle = jet.DeltaR(prb_);
          jet_ = jet;
          break;
        }
      }
      if( !(OpeningAngle!=999.) ) break;

      //////////////////////////
      // --- Event weight --- //
      //////////////////////////
      double weight = 1.;
      if(!IsDATA) {
        weight = MC_Weight( ev , param , JBparam );
        //weight *= Muon_Weight( param , {tag, prb} );
      }

      FillHist(this_region+"/Njets___"+this_region, alljets.size(), weight, 10, 0, 10);
      FillHist(this_region+"/Nbjets___"+this_region, jets.size(), weight, 10, 0, 10);

      double DimuonMass = (tag_+prb_).M();

      FillHist(this_region+"/Lepton_Pt___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
      FillHist(this_region+"/Lepton_Eta___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
      FillHist(this_region+"/Dilepton_Mass___"+this_region, DimuonMass, weight, 500, 0., 100.);
      FillHist(this_region+"/LeptonJet_DeltaR___"+this_region, OpeningAngle, weight, 80, 0., 0.4);
      FillHist(this_region+"/Lepton_Iso___"+this_region, prb_.RelIso(), weight, 100, 0., 5.);
      FillHist(this_region+"/Lepton_TrkIso___"+this_region, prb_.TrkIso()/prb_.Pt(), weight, 100, 0., 5.);

      double JetPt_NoMuon = jet_.Pt() - prb_.Pt();
      FillHist(this_region+"/Jet_Pt___"+this_region, jet_.Pt(), weight, 100, 0., 1000.);
      FillHist(this_region+"/JetNoMuon_Pt___"+this_region, JetPt_NoMuon, weight, 100, 0., 1000.);

      if( OpeningAngle < 0.05 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-0top05_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-0top05_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-0top05_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-0top05_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-0top05_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-0top05_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(0.05 < OpeningAngle && OpeningAngle < 0.1 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p05top10_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p05top10_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(0.1 < OpeningAngle && OpeningAngle < 0.15 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p10top15_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p10top15_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(0.15 < OpeningAngle && OpeningAngle < 0.20 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p15top20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(0.2 < OpeningAngle && OpeningAngle < 0.25 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p15top20_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p15top20_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(0.25 < OpeningAngle && OpeningAngle < 0.3 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_dR-p25top30_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_dR-p25top30_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }


      if(JetPt_NoMuon < 80 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-0to80_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(80 < JetPt_NoMuon && JetPt_NoMuon < 120 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-80to120_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(120 < JetPt_NoMuon && JetPt_NoMuon < 160 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-120to160_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(160 < JetPt_NoMuon && JetPt_NoMuon < 200 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-160to200_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(200 < JetPt_NoMuon && JetPt_NoMuon < 300 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-200to300_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(300 < JetPt_NoMuon && JetPt_NoMuon < 500 ) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-300to500_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }
      else if(500 < JetPt_NoMuon) {
        if(30. < prb_.Pt() && prb_.Pt() < 40.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-30to40_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(40. < prb_.Pt() && prb_.Pt() < 50.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-40to50_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(50. < prb_.Pt() && prb_.Pt() < 70.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-50to70_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(70. < prb_.Pt() && prb_.Pt() < 100.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-70to100_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(100. < prb_.Pt() && prb_.Pt() < 200.) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-100to200_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
        else if(200. < prb_.Pt()) FillHist(this_region+"/Lepton_JetPt-500toInf_Pt-200toInf_Iso___"+this_region, prb_.RelIso(), weight, 20, 0., 5.);
      }


      if( !prb_.PassID("NonIsolatedMuon") ) continue;
      FillHist(this_region+"/Lepton_Pt___pass___"+this_region, prb_.Pt(), weight, 190, 10., 200.);
      FillHist(this_region+"/Lepton_Eta___pass___"+this_region, prb_.Eta(), weight, 480, -2.4, 2.4);
      FillHist(this_region+"/Dilepton_Mass___pass___"+this_region, DimuonMass, weight, 500, 0., 100.);
      FillHist(this_region+"/LeptonJet_DeltaR___pass___"+this_region, OpeningAngle, weight, 80, 0., 0.4);
      FillHist(this_region+"/Lepton_Iso___pass___"+this_region, prb_.RelIso(), weight, 100, 0., 5.);

    }
  }
}
*/

void NIsoMuon::GenLevelAnalysis(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam) {
  JBparam.AnalysisName = "GenLevel";
  TString this_region = param.Name+JBparam.SystName+"_"+JBparam.AnalysisName;

  bool PrintAllGenParticles=0;
  bool SignalStudy=1;
  bool PassGenJetDR=0;
  bool PassTrigger=1;
  bool FillHists=1;
  bool PassAcceptance=0;
  bool RecoMatching=0;

  //cout<<endl<<endl;
  //cout<<"+++++++++++++++++++++++++++++++"<<endl;
  vector<Gen> gens = GetGens();
  /*
  vector<GenJet> genjets = GetAllGenJets();

  // ----- Print all gen particles ----- //
  if(PrintAllGenParticles) {
    for(unsigned iGen=0; iGen<gens.size(); iGen++) {
      Gen gen = gens[iGen];
      if(gen.MotherIndex()>-1) {
        cout<<setw(4)<<gen.Index()<<", ID:"<<setw(4)<<gen.PID()<<", "; PrintPtEtaPhi(gen); cout<<", status:"<<setw(3)<<gen.Status()<<", m:"<<setw(4)<<gen.MotherIndex()<<", mID:"<<gens.at(gen.MotherIndex()).PID()<<", hard"<<gen.isHardProcess()<<" last"<<gen.isLastCopy()<<" HPFS"<<gen.fromHardProcessFinalState();
        if(gen.isLastCopy()&&fabs(gen.PID())==13) {
          Gen mother = MotherParticle(gen,gens);
          cout<<" mI:"<<mother.Index()<<" mID:"<<mother.PID();
          if(mother.PID()==23) cout<<" << "<<mother.Index();
        }
        cout<<endl;
  
      }
      else {
        cout<<setw(4)<<gen.Index()<<", ID:"<<setw(4)<<gen.PID()<<", "; PrintPtEtaPhi(gen); cout<<", status:"<<setw(3)<<gen.Status()<<", m:"<<setw(4)<<gen.MotherIndex()<<", mID: -1, hard"<<gen.isHardProcess()<<" last"<<gen.isLastCopy()<<" HPFS"<<gen.fromHardProcessFinalState()<<endl;
      }
  
      if( !(gen.Status()==1&&gen.MotherIndex()!=0&&gen.MotherIndex()!=1) ) continue;
      vector<Gen> History = TrackMotherParticles(gen, gens);
      for(unsigned iHistory=0; iHistory<History.size(); iHistory++) {
        Gen TrackingGen = History[iHistory];
        GenName( TrackingGen );
        cout<<"("<<setw(2)<<TrackingGen.Index()<<") - ";
      }
      cout<<endl;
    }
  }
  // ---------------------------------------------------------//


  // ----- Signal study ----- //
  if(SignalStudy) {
    vector<Gen> gen_muons;
    Gen Zcandidate;
    for(unsigned iGen=0; iGen<gens.size(); iGen++) {
      Gen gen = gens[iGen];
      if(abs(gen.PID())==13&&gen.isLastCopy()) {
        Gen mother=MotherParticle(gen,gens);
        if(mother.PID()==23) {
          gen_muons.push_back(gen);
          Zcandidate=mother;
        }
      }
    }
    if(gen_muons.size()<2) { cout<<"[ERROR] muon size < 2"<<endl; return; }
    if(gen_muons.size()>2) { cout<<"[ERROR] muon size > 2"<<endl; return; }
    cout<<"[TEST] muon size = 2"<<endl; 

    GenJet* ptr_genjet=NULL;
    double dR=9999;
cout<<"[TEST] genjets.size() == "<<genjets.size()<<endl;
    for(unsigned i=0; i<genjets.size(); i++) {
      GenJet* genjet = &genjets[i];
      if( genjet->DeltaR(Zcandidate) < dR ) {
        dR = genjet->DeltaR(Zcandidate);
        ptr_genjet = genjet;
      }
    }
    if(ptr_genjet==NULL) { cout<<"[ERROR] No genjet"<<endl; return; }
    GenJet this_genjet = *ptr_genjet;
    cout<<"[TEST] Select genjet candidate"<<endl;

    if( PassGenJetDR ) {
      for(unsigned i=0; i<gen_muons.size(); i++) {
        if(!(this_genjet.DeltaR(gen_muons[i])<0.4)) return;
      }
    }

    double weight = 1.;
    if(!IsDATA) {
      weight = MC_Weight( ev , param , JBparam );
      //weight *= Muon_Weight( param , dimuon );
    }


    // ----- Histograms ----- //
    if( FillHists ) {
      cout<<"[TEST] FillHists"<<endl;
      FillHist( this_region+"/GenDimuon_Mass___"+this_region, Zcandidate.M(), weight, 1000, 0., 100);
  
      FillHist( this_region+"/GenJet_Pt___"+this_region, this_genjet.Pt(), weight, 1000, 0., 1000);
      FillHist( this_region+"/GenJet_Eta___"+this_region, this_genjet.Eta(), weight, 100, -5, 5);
      for(unsigned i=0; i<gen_muons.size(); i++) {
        FillHist( this_region+"/GenLepton_Pt___"+this_region, gen_muons[i].Pt(), weight, 1000, 0., 1000);
        FillHist( this_region+"/GenLepton_Eta___"+this_region, gen_muons[i].Eta(), weight, 100, -5, 5);
        if( this_genjet.Pt() > 0 ) FillHist( this_region+"/GenLeptonGenJet_DeltaR___"+this_region, this_genjet.DeltaR( gen_muons[i] ), weight, 100,0.,1.);
      }
      FillHist( this_region+"/GenZ_Pt___"+this_region, Zcandidate.Pt(), weight, 1000, 0., 1000);
      FillHist( this_region+"/GenZ_Eta___"+this_region, Zcandidate.Eta(), weight, 100, -5, 5);
      FillHist( this_region+"/GenDilepton_Pt___"+this_region, (gen_muons[0]+gen_muons[1]).Pt(), weight, 1000, 0., 1000);
      FillHist( this_region+"/GenDilepton_Eta___"+this_region, (gen_muons[0]+gen_muons[1]).Eta(), weight, 100, -5, 5);
      if( this_genjet.Pt() > 0 ) FillHist( this_region+"/GenDileptonGenJet_DeltaR___"+this_region, this_genjet.DeltaR( gen_muons[0]+gen_muons[1] ), weight, 100,0.,1.);
      FillHist( this_region+"/GenDilepton_DeltaR___"+this_region, gen_muons[0].DeltaR( gen_muons[1] ), weight, 100,0.,1.);
      if( this_genjet.Pt() > 0 ) FillHist( this_region+"/GenZGenJet_DeltaR___"+this_region, this_genjet.DeltaR( Zcandidate ), weight, 100,0.,1.);
    
      FillHist( this_region+"/Efficiency___"+this_region, 0, weight, 8,0,8.);
    }


    // ----- Muon Acceptance ----- //
    if( PassAcceptance ) {
      bool MuonPtCut  = (gen_muons[0].Pt()>Leading_Muon_Pt && gen_muons[1].Pt()>Subleading_Muon_Pt) || (gen_muons[0].Pt()>Subleading_Muon_Pt && gen_muons[1].Pt()>Leading_Muon_Pt);
      bool MuonEtaCut = fabs(gen_muons[0].Eta())<2.4 && fabs(gen_muons[1].Eta())<2.4;
      bool JetPtCut   = this_genjet.Pt()>30;
      bool JetEtaCut  = fabs(this_genjet.Eta())<2.4;
      bool AcceptanceCut = MuonPtCut&&MuonEtaCut&&JetPtCut&&JetEtaCut;
      if(AcceptanceCut)
        FillHist( this_region+"/Efficiency___"+this_region, 1, weight, 8,0,8.);
    
    
      if(AcceptanceCut&&!ev.PassTrigger(TriggerName)) {
        cout<<endl;
        cout<<"[Event "<<event<<"] Acceptance: "<<AcceptanceCut<<", Tirg: "<<ev.PassTrigger(TriggerName)<<endl;
        cout<<"- gen -"<<endl;
        cout<<" mu1: ("<<gen_muons[0].Pt()<<", "<<gen_muons[0].Eta()<<")"<<endl;
        cout<<" mu2: ("<<gen_muons[1].Pt()<<", "<<gen_muons[1].Eta()<<"), pt: "<<MuonPtCut<<", eta: "<<MuonEtaCut<<endl;
        cout<<" jet: ("<<this_genjet.Pt()<<", "<<this_genjet.Eta()<<"), pt: "<<JetPtCut<<", eta: "<<JetEtaCut<<endl;
        cout<<"- reco -"<<endl;
        for(unsigned i=0; i<AllMuons.size(); i++) {
          if(AllMuons[i].Pt()>5&&fabs(AllMuons[i].Eta())<2.8) cout<<" mu: ("<<AllMuons[i].Pt()<<", "<<AllMuons[i].Eta()<<", "<<AllMuons[i].Phi()<<"), glb: "<<AllMuons[i].isGlobalMuon()<<", trk: "<<AllMuons[i].isTrackerMuon()<<", PF: "<<AllMuons[i].isPFMuon()<<", loose: "<<AllMuons[i].PassID("POGLoose")<<", tight: "<<AllMuons[i].PassID("POGTight")<<endl;
        }
        cout<<" dR = "<<AllMuons[0].DeltaR(AllMuons[1])<<endl;
        for(unsigned i=0; i<AllJets.size(); i++) {
          if(AllJets[i].Pt()>15&&fabs(AllJets[i].Eta())<2.8) cout<<" jet: ("<<AllJets[i].Pt()<<", "<<AllJets[i].Eta()<<")"<<endl;
        }
      }

      // ----- trigger ----- //
      if( PassTrigger ) {
        if( !(ev.PassTrigger(TriggerName) ) ) return;
        if(AcceptanceCut) FillHist( this_region+"/Efficiency___"+this_region, 2, weight, 8,0,8.);
      }  
  
      if( RecoMatching ) {
        // ----- reco matching ----- //
        vector<Muon> muons = SelectMuons(AllMuons, "", Subleading_Muon_Pt, 2.4);
        vector<Jet> jets = SelectJets(AllJets, "", 30.,2.4);
        Jet* this_jet = FindRecoParticle(this_genjet, jets);
        vector<Muon*> this_muons;
        for(unsigned i=0; i<gen_muons.size(); i++) {
          this_muons.push_back((Muon*)FindRecoParticle(gen_muons[i], muons));
        }
        if(this_jet==NULL||this_muons[0]==NULL||this_muons[1]==NULL) return;
        if(!(this_muons[0]->Pt()>Leading_Muon_Pt||this_muons[1]->Pt()>Leading_Muon_Pt)) return;
        FillHist( this_region+"/Efficiency___"+this_region, 3, weight, 8,0,8.);
      
        this_jet=NULL;
        jets = SelectJets(jets, param.Jet_ID, 30.,2.4);
        this_jet = FindRecoParticle(this_genjet, jets);
        if(this_jet==NULL) return;
        FillHist( this_region+"/Efficiency___"+this_region, 4, weight, 8,0,8.);
      
        this_muons.clear();
        muons = SelectMuons(muons, param.Muon_Tight_ID, Subleading_Muon_Pt, 2.4);
        for(unsigned i=0; i<gen_muons.size(); i++) {
          this_muons.push_back((Muon*)FindRecoParticle(gen_muons[i], muons));
        }
        if(this_muons[0]==NULL||this_muons[1]==NULL) return;
        if(!(this_muons[0]->Pt()>Leading_Muon_Pt||this_muons[1]->Pt()>Leading_Muon_Pt)) return;
        FillHist( this_region+"/Efficiency___"+this_region, 5, weight, 8,0,8.);
      
        ////////////////////////////////////////////////////////
      
        for(unsigned i=0; i<2; i++) {
          FillHist( this_region+"/Lepton_RelIso___"+this_region, this_muons[i]->RelIso(), weight, 500, 0., 5.);
          FillHist( this_region+"/Lepton_Iso___"+this_region, this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 100, 0., 100.);
          FillHist( this_region+"/Lepton_RelTrkIso___"+this_region, this_muons[i]->TrkIso()/this_muons[i]->Pt(), weight, 500, 0., 5.);
          FillHist( this_region+"/Lepton_TrkIso___"+this_region, this_muons[i]->TrkIso(), weight, 100, 0., 100.);
      
          double trkiso=this_muons[i]->TrkIso();
          unsigned j=1; if(i==1) j=0;
          if(this_muons[i]->DeltaR(*this_muons[j])<0.3&&this_muons[i]->TrkIso()>this_muons[j]->Pt()) trkiso=trkiso-this_muons[j]->Pt();
          FillHist( this_region+"/Lepton_RelTrkIsoCorr___"+this_region, trkiso/this_muons[i]->Pt(), weight, 500, 0., 5.);
      
          FillHist( this_region+"/Lepton_Pt_Lepton_Iso___"+this_region, this_muons[i]->Pt(), this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 20,0,100.,20,0,100.);
          FillHist( this_region+"/Lepton_Pt_Lepton_TrkIso___"+this_region, this_muons[i]->Pt(), this_muons[i]->TrkIso(), weight, 20,0,100.,100,0,100.);
          FillHist( this_region+"/Lepton_Pt_Lepton_TrkIsoCorr___"+this_region, this_muons[i]->Pt(), trkiso, weight, 20,0,100.,100,0,100.);
          FillHist( this_region+"/LeptonJet_DeltaR_Lepton_Iso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 100,0,1.,100,0,100.);
          FillHist( this_region+"/LeptonJet_DeltaR_Lepton_TrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->TrkIso(), weight, 100,0,1.,100,0,100.);
          FillHist( this_region+"/LeptonJet_DeltaR_Lepton_TrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[i]), trkiso, weight, 100,0,1.,100,0,100.);
          FillHist( this_region+"/LeptonJet_DeltaR_Lepton_RelIso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->RelIso(), weight, 100,0,1.,300,0,3.);
          FillHist( this_region+"/LeptonJet_DeltaR_Lepton_RelTrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->TrkIso()/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
          FillHist( this_region+"/LeptonJet_DeltaR_Lepton_RelTrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[i]), trkiso/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
          FillHist( this_region+"/DileptonJet_DeltaR_Lepton_Iso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 100,0,1.,100,0,100.);
          FillHist( this_region+"/DileptonJet_DeltaR_Lepton_TrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->TrkIso(), weight, 100,0,1.,100,0,100.);
          FillHist( this_region+"/DileptonJet_DeltaR_Lepton_TrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), trkiso, weight, 100,0,1.,100,0,100.);
          FillHist( this_region+"/DileptonJet_DeltaR_Lepton_RelIso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->RelIso(), weight, 100,0,1.,300,0,3.);
          FillHist( this_region+"/DileptonJet_DeltaR_Lepton_RelTrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->TrkIso()/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
          FillHist( this_region+"/DileptonJet_DeltaR_Lepton_RelTrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), trkiso/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
      
          FillHist( this_region+"/LeptonJet_DeltaR___"+this_region, this_muons[i]->DeltaR( this_jet[0] ), weight, 100, 0., 1.);
        }
      
        FillHist( this_region+"/Dilepton_DeltaR___"+this_region, this_muons[0]->DeltaR( *this_muons[1] ), weight, 100, 0., 1.);
        FillHist( this_region+"/DileptonJet_DeltaR___"+this_region, this_jet[0].DeltaR( *this_muons[0] + *this_muons[1] ), weight, 100, 0., 1.);
      
      ////////////////////////////////////////////////////////
      
        if(!(this_muons[0]->RelIso()>0.3&&this_muons[1]->RelIso()>0.3)) return;
        FillHist( this_region+"/Efficiency___"+this_region, 6, weight, 8,0,8.);
      
        if(!(this_jet->DeltaR(*this_muons[0])<0.3&&this_jet->DeltaR(*this_muons[1])<0.3)) return;
        FillHist( this_region+"/Efficiency___"+this_region, 7, weight, 8,0,8.);
      }
    }
  }
  */
}

 
NIsoMuon::NIsoMuon(){

}

NIsoMuon::~NIsoMuon(){

}

