#ifndef Dielectron_h
#define Dielectron_h

#include "AnalyzerCore.h"

class Dielectron : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  std::vector<Lepton*> DielectronSelection(vector<Electron>& electrons);
  double MCweight(Event ev);
  void executeEvent();

  vector<TString> ElectronIDs;

  vector<TString> IsoEleTriggerName;
  double TriggerSafePtCut;

  vector<Electron> AllElectrons;

  double weight_Prefire;


  Dielectron();
  ~Dielectron();

};



#endif

