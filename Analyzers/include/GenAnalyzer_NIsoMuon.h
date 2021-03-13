#ifndef GenAnalyzer_NIsoMuon_h
#define GenAnalyzer_NIsoMuon_h

#include "JBAnalyzerCore.h"

class GenAnalyzer_NIsoMuon : public JBAnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param, JBAnalyzerParameter JBparam);
  void executeEvent();

  vector<TString> IsoMuTriggerName;
  double Leading_Muon_Pt, Subleading_Muon_Pt;

  double weight_Prefire;

  Jet JetMatching(Gen gen, vector<Jet> jets, double cone_size);

  GenAnalyzer_NIsoMuon();
  ~GenAnalyzer_NIsoMuon();

};



#endif

