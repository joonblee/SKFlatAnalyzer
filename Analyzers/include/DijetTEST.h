#ifndef DijetTEST_h
#define DijetTEST_h

#include "AnalyzerCore.h"

class DijetTEST : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  double MCweight(Event ev);
  void executeEvent();

  TString JetID;

  vector<Jet> AllJets;


  DijetTEST();
  ~DijetTEST();

};



#endif

