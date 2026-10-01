#ifndef NIsoMuon_h
#define NIsoMuon_h

#include "JBAnalyzerCore.h"

class NIsoMuon : public JBAnalyzerCore {

public:

  enum class AnalysisMode {
    NIsoDimuon,
    MuonIDEfficiency,
    TriggerEfficiency
  };

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param, JBAnalyzerParameter option);

  double MC_Weight(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam);
  double Muon_Weight(AnalyzerParameter param, vector<Lepton*> muons);

  void NIsoDimuon(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets);
  void MuonIDEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> denominatorMuons, vector<Jet> jets, vector<Jet> alljets);
  void TriggerEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets);
  void IsoEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets);
  void SingleMuonIsoEfficiency(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets, vector<Jet> alljets);
  void GenLevelAnalysis(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam);

  void executeEvent();

  ///////////////////////////////
  // --- InitialAnalyzer() --- //
  ///////////////////////////////
  AnalysisMode analysisMode;
  bool RunSyst, RunXSecSyst;
  bool RunConvenerStudy, RunDYValidationDRStudy;
  bool MCAnalysis;

  vector<TString> MuonID1s, MuonID2s;
  TString  MuonIDSFKey, MuonISOSFKey;

  vector<TString> DileptonSigns;

  vector<TString> JetIDs;

  //JetTagging::Tagger BTagger;
  //vector<JetTagging::WP> BTaggingWPs;
  vector<JetTagging::Parameters> jtps;
  vector<TString> BTagNames;

  vector<double> METCuts;
  vector<TString> METCutNames;

  /////////////////////
  // --- Trigger --- //
  /////////////////////
  vector<TString> TriggerName;
  TString TriggerNameForSF_Muon;
  double Leading_Muon_Pt, Subleading_Muon_Pt;

  // /////////////////////////
  // --- executeEvent() --- //
  ////////////////////////////
  vector<Muon> AllMuons;
  vector<Electron> AllElectrons;
  vector<Tau> AllTaus;
  vector<Jet> AllJets;
  vector<FatJet> AllFatJets;

  double weight_Prefire;


  NIsoMuon();
  ~NIsoMuon();

};



#endif

