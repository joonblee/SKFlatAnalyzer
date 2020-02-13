#ifndef NIsoMuon_h
#define NIsoMuon_h

#include "AnalyzerCore.h"

class NIsoMuon : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  //std::vector<Lepton*> NIsoMuonSelection(vector<Muon>& muons, vector<Muon>& Vmuons, int sgn);
  std::vector<Lepton*> NIsoMuonSelection(vector<Muon>& muons, int sgn);
  double MC_Weight(Event ev, AnalyzerParameter param, vector<Lepton*> dimuon);
  void executeEvent();

  vector<TString> MuonID1s, MuonID2s;
  TString  MuonIDSFKey, MuonISOSFKey;

  TString JetID;

  vector<TString> IsoMuTriggerName;
  double Leading_Muon_Pt, Subleading_Muon_Pt;

  vector<Muon> AllMuons;
  vector<Jet> AllJets;

  double weight_Prefire;


  NIsoMuon();
  ~NIsoMuon();

};



#endif

