#ifndef JBAnalyzerCore_h
#define JBAnalyzerCore_h

#include "AnalyzerCore.h"

class JBAnalyzerParameter {

public:

  TString     DileptonSign;

  bool        SetBTag;
  JetTagging::Tagger BTagger;
  JetTagging::WP     BTaggingWP;
  JetTagging::Parameters BTagParameter;
  TString     BTagName;

  double      METCut;
  TString     METCutName;

  TString     AnalysisName;
  TString     SystName;

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
  bool BottomMesonDecay(Gen gen, vector<Gen> gens);
  bool CharmMesonDecay(Gen gen, vector<Gen> gens);
  bool bbbarDecay(Gen gen, vector<Gen> gens);
  bool ccbarDecay(Gen gen, vector<Gen> gens);
  bool bquarkDecay(Gen gen, vector<Gen> gens);
  bool cquarkDecay(Gen gen, vector<Gen> gens);

  bool fromBBbar(vector<Gen> dilepton, vector<Gen> gens);
  bool fromDDbar(vector<Gen> dilepton, vector<Gen> gens);
  bool fromSingleB(vector<Gen> dilepton, vector<Gen> gens);
  bool fromSingleD(vector<Gen> dilepton, vector<Gen> gens);

  void PrintPtEtaPhi(Gen gen);
  void PrintPtEtaPhi(Lepton lep);
  void PrintPtEtaPhi(Muon mu);
  void PrintPtEtaPhi(Electron el);
  void PrintPtEtaPhi(Jet jet);
  void PrintPtEtaPhi(Particle MET);

  void PrintGen(Gen gen, vector<Gen> gens);
  void PrintAllGens(vector<Gen> gens);

  bool DimuonCharge(JBAnalyzerParameter JBparam, Muon muon_1, Muon muon_2);
  bool DileptonCharge(JBAnalyzerParameter JBparam, Muon muon, Electron elec);

  /*
  vector<Jet> SelectBJets(vector<Jet> jets, JetTagging::Tagger tagger, JetTagging::WP WP, bool applySF, int systematic);
  vector<Jet> SelectLightJets(vector<Jet> jets, JetTagging::Tagger tagger, JetTagging::WP WP, bool applySF, int systematic);
  */
  vector<Jet> SelectBJets(vector<Jet> jets, JetTagging::Parameters jtp);
  vector<Jet> SelectLightJets(vector<Jet> jets, JetTagging::Parameters jtp);

  std::vector<Muon> JBGetAllMuons();

  //==== Quick Plotters
  void LeptonPlotter(std::vector<Lepton *> leps, TString this_region, double weight);
  void DileptonPlotter(std::vector<Lepton*> leps, TString this_region, double weight);
  void JetPlotter(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight);
  void DijetPlotter(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight);

  string format_bin100(Double_t value);
  string format_bin(Double_t value);
};

#endif

