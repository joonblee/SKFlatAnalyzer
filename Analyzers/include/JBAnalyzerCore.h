#ifndef JBAnalyzerCore_h
#define JBAnalyzerCore_h

#include "AnalyzerCore.h"
#include "GenJet.h"
#include "GenFatJet.h"

class JBAnalyzerParameter {

public:

  TString     DileptonSign;

  bool        SetBTag;
  Jet::Tagger BTagger;
  Jet::WP     BTaggingWP;
  TString     BTagName;

  double      METCut;
  TString     METCutName;

  TString     AnalysisName;

  void Clear();

  JBAnalyzerParameter();

};

class JBAnalyzerCore: public AnalyzerCore {

public:

  //bool JBIsBTagged(Jet jet, Jet::Tagger tagger, Jet::WP WP);

  void TriggerSetup(std::vector<TString>& TriggerName, double& Leading_Muon_Pt, double& Subleading_Muon_Pt);

  std::vector<Gen> TrackMotherParticles(Gen gen, std::vector<Gen> gens);
  Gen MotherParticle(Gen gen, std::vector<Gen> gens);
  Muon* FindRecoParticle(Gen gen, std::vector<Muon>& muons);
  Jet* FindRecoParticle(GenJet genjet, std::vector<Jet>& jets);

  void GenName(Gen gen);

  bool isBottomMeson(Gen gen);
  bool isCharmMeson(Gen gen);
  bool isStrangeMeson(Gen gen);
  bool isbbbar(Gen gen);
  bool isccbar(Gen gen);
  bool isLightMeson(Gen gen);
  bool isbquark(Gen gen);
  bool iscquark(Gen gen);
  bool issquark(Gen gen);
  bool isudquark(Gen gen);

  bool fromBottomMeson(Gen gen, vector<Gen> gens);
  bool fromCharmMeson(Gen gen, vector<Gen> gens);
  bool fromStrangeMeson(Gen gen, vector<Gen> gens);
  bool frombbbar(Gen gen, vector<Gen> gens);
  bool fromccbar(Gen gen, vector<Gen> gens);
  bool fromLightMeson(Gen gen, vector<Gen> gens);
  bool frombquark(Gen gen, vector<Gen> gens);
  bool fromcquark(Gen gen, vector<Gen> gens);
  bool fromsquark(Gen gen, vector<Gen> gens);
  bool fromudquark(Gen gen, vector<Gen> gens);
  bool fromb(Gen gen, vector<Gen> gens);
  bool fromc(Gen gen, vector<Gen> gens);
  

  void PrintPtEtaPhi(Gen gen);
  void PrintPtEtaPhi(Lepton lep);
  void PrintPtEtaPhi(Muon mu);
  void PrintPtEtaPhi(Electron el);
  void PrintPtEtaPhi(Jet jet);
  void PrintPtEtaPhi(Particle MET);

  bool DimuonCharge(JBAnalyzerParameter JBparam, Muon muon_1, Muon muon_2);
  bool DileptonCharge(JBAnalyzerParameter JBparam, Muon muon, Electron elec);


  vector<Jet> SelectBJets(vector<Jet> jets, Jet::Tagger tagger, Jet::WP WP, bool applySF, int systematic);
  vector<Jet> SelectLightJets(vector<Jet> jets, Jet::Tagger tagger, Jet::WP WP, bool applySF, int systematic);

  std::vector<Muon> JBGetAllMuons();
  std::vector<GenJet> GetAllGenJets();
  std::vector<GenFatJet> GetAllGenFatJets();

  //==== Quick Plotters
  void LeptonPlotter(std::vector<Lepton *> leps, TString this_region, double weight);
  void DileptonPlotter(std::vector<Lepton*> leps, TString this_region, double weight);
  void JetPlotter(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight);
  void DijetPlotter(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight);

};

#endif

