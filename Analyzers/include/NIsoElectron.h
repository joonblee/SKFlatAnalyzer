#ifndef NIsoElectron_h
#define NIsoElectron_h

#include "AnalyzerCore.h"

class NIsoElectron : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  std::vector<Lepton*> NIsoElectronSelection(vector<Electron>& electrons, vector<Electron>& Velectrons, int sgn);
  double MCweight(Event ev);
  void executeEvent();

  vector<TString> ElectronIDs, NIsoElectronIDs;
  TString  ElectronIDSFKey, ElectronISOSFKey;

  TString JetID;

  vector<TString> IsoEleTriggerName;
  double TriggerSafePtCut, TriggerSecondPtCut;

  vector<Electron> AllElectrons;
  vector<Jet> AllJets;

  double weight_Prefire;


  NIsoElectron();
  ~NIsoElectron();

};



#endif

