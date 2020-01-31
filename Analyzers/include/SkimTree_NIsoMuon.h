#ifndef SkimTree_NIsoMuon_h
#define SkimTree_NIsoMuon_h

#include "AnalyzerCore.h"

class SkimTree_NIsoMuon : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  SkimTree_NIsoMuon();
  ~SkimTree_NIsoMuon();

  TTree *newtree;

  vector<TString> triggers;

  vector<Muon> AllMuons;

  void WriteHist();

};



#endif

