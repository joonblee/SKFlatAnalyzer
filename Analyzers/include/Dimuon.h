#ifndef Dimuon_h
#define Dimuon_h

#include "AnalyzerCore.h"

class Dimuon : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  std::vector<Lepton*> DimuonSelection(vector<Muon>& muons, int sgn);
  double MCweight(Event ev);
  void executeEvent();

  vector<TString> MuonIDs;
  TString  MuonIDSFKey, MuonISOSFKey;

  TString JetID;

  vector<TString> IsoMuTriggerName;
  double TriggerSafePtCut;

  vector<Muon> AllMuons;
  vector<Jet> AllJets;

  double weight_Prefire;



  Dimuon();
  ~Dimuon();

};



#endif

