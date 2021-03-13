#ifndef NIsoMuon_h
#define NIsoMuon_h

#include "JBAnalyzerCore.h"

class NIsoMuon : public JBAnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param, JBAnalyzerParameter option);

  double MC_Weight(Event ev, AnalyzerParameter param);
  double Muon_Weight(AnalyzerParameter param, vector<Lepton*> muons);

  void NIsoDimuon(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets);
  void GenLevelAnalysis(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam);

  void executeEvent();

  ///////////////////////////////
  // --- InitialAnalyzer() --- //
  ///////////////////////////////
  vector<TString> MuonID1s, MuonID2s;
  TString  MuonIDSFKey, MuonISOSFKey;

  vector<TString> DileptonSigns;

  vector<TString> JetIDs;

  Jet::Tagger BTagger;
  vector<Jet::WP> BTaggingWPs;
  vector<TString> BTagNames;

  vector<double> METCuts;
  vector<TString> METCutNames;

  /////////////////////
  // --- Trigger --- //
  /////////////////////
  vector<TString> TriggerName;
  double Leading_Muon_Pt, Subleading_Muon_Pt;

  // /////////////////////////
  // --- executeEvent() --- //
  ////////////////////////////
  vector<Muon> AllMuons;
  vector<Jet> AllJets;
  vector<FatJet> AllFatJets;

  double weight_Prefire;


  NIsoMuon();
  ~NIsoMuon();

};



#endif

