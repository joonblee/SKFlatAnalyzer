#include "JBAnalyzerCore.h"

void JBAnalyzerParameter::Clear() {
  SetBTag    = false;
  BTagName       = "";
}

JBAnalyzerParameter::JBAnalyzerParameter() {
  SetBTag    = true;
  BTagger    = Jet::DeepCSV;
  BTaggingWP = Jet::Tight;
  BTagName   = "Tight_DeepCSV";
  DileptonSign = "";
}

/////////////////////
// --- Trigger --- //
/////////////////////
void JBAnalyzerCore::TriggerSetup(std::vector<TString>& TriggerName, double& Leading_Muon_Pt, double& Subleading_Muon_Pt) {
  Subleading_Muon_Pt = 10.;

  if( DataYear == 2016 ) {
    if( TriggerInput == "SingleMuon" ) {
      TriggerName = {
        "HLT_IsoMu24_v", "HLT_IsoTkMu24_v"
      };
      Leading_Muon_Pt = 26.;
    }
    else if( TriggerInput == "HighPtMuon" ) {
      TriggerName = {
        "HLT_Mu50_v", "HLT_TkMu50_v"
      };
      Leading_Muon_Pt = 52.;
      //Subleading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon" ) {
      TriggerName = {
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
        "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
        "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
        "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
        "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"
      };
      Leading_Muon_Pt = 20.;
    }
    else if( TriggerInput == "SameSignDoubleMuon" ) {
      TriggerName = {
        "HLT_Mu17_Mu8_SameSign_DZ_v"
      };
      Leading_Muon_Pt = 20.;
      Subleading_Muon_Pt = 10.;
    }
    else if( TriggerInput == "DoubleHighPtMuon" || TriggerInput == "NonIsolatedDoubleMuon" ) {
      TriggerName = { "HLT_Mu30_TkMu11_v" };
      Leading_Muon_Pt = 32.;
      Subleading_Muon_Pt = 13.;
      //Subleading_Muon_Pt = 32.;
    }
  }
  else if( DataYear == 2017 ) {
    if( TriggerInput == "SingleMuon" ) {
      TriggerName = {
        "HLT_IsoMu27_v"
      };
      Leading_Muon_Pt = 30.;
    }
    else if( TriggerInput == "HighPtMuon" ) {
      TriggerName = {
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v"
      };
      Leading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon" ) {
      TriggerName = {
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v"
      };
      Leading_Muon_Pt = 20.;
    }
    else if( TriggerInput == "SameSignDoubleMuon" ) {
      TriggerName = {
        "HLT_Mu18_Mu9_SameSign_DZ_v",
        "HLT_Mu18_Mu9_SameSign_v"
      };
      Leading_Muon_Pt = 20.;
      Subleading_Muon_Pt = 10.;
    }
    else if( TriggerInput == "DoubleHighPtMuon" || TriggerInput == "NonIsolatedDoubleMuon" ) {
      TriggerName = {"HLT_Mu37_TkMu27_v"};
      Leading_Muon_Pt = 40.;
      Subleading_Muon_Pt = 30.;
    }
  }
  else if( DataYear == 2018 ) {
    if( TriggerInput == "SingleMuon" ) {
      TriggerName = {
        "HLT_IsoMu24_v"
      };
      Leading_Muon_Pt = 26.;
    }
    else if( TriggerInput == "HighPtMuon" ) {
      TriggerName = {
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v"
      };
      Leading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon" ) {
      TriggerName = {
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"
      };
      Leading_Muon_Pt = 20.;
    }
    else if( TriggerInput == "DoubleHighPtMuon" || TriggerInput == "NonIsolatedDoubleMuon" ) {
      TriggerName = {
        "HLT_Mu37_TkMu27_v"
      };
      Leading_Muon_Pt = 40.;
      Subleading_Muon_Pt = 30.;
    }
  }

  return;
}

////////////////////////////////
// --- Gen-level Analysis --- //
////////////////////////////////
std::vector<Gen> JBAnalyzerCore::TrackMotherParticles(Gen gen, std::vector<Gen> gens) {
  vector<Gen> out;
  out.push_back(gen);

  Gen dum = gen;
  while( dum.MotherIndex() > -1 ) {
    dum = gens[dum.MotherIndex()];
    out.push_back(dum);
  }

  return out;
}

Gen JBAnalyzerCore::MotherParticle(Gen gen, std::vector<Gen> gens) {
  Gen out=gen;
  int pdgid=gen.PID();

  while( out.MotherIndex() > -1 && gens.at(out.MotherIndex()).PID() == pdgid ) {
    out = gens[out.MotherIndex()];
  }
  out = gens[out.MotherIndex()];

  return out;
}

Muon* JBAnalyzerCore::FindRecoParticle(Gen gen, std::vector<Muon>& muons) {
  vector<Muon*> candidates;
  for(unsigned i=0; i<muons.size(); i++){
    double dR=gen.DeltaR(muons[i]);
    if(dR<0.1&&muons[i].Charge()*gen.Charge()>0){
      candidates.push_back((Muon*)&muons[i]);
    }
  }
  if(candidates.size()==1) return candidates[0];
  else if(candidates.size()==0) return NULL;
  Muon* out;
  double ptratio=9999;
  for(unsigned i=0; i<candidates.size(); i++) {
    if(fabs(candidates[i]->Pt()/gen.Pt()-1)<ptratio) {
      ptratio=fabs(candidates[i]->Pt()/gen.Pt()-1);
      out=candidates[i];
    }
  }
  return out;
}

Jet* JBAnalyzerCore::FindRecoParticle(GenJet genjet, std::vector<Jet>& jets) {
  vector<Jet*> candidates;
  for(unsigned i=0; i<jets.size(); i++){
    double dR=genjet.DeltaR(jets[i]);
    if(dR<0.4){
      candidates.push_back((Jet*)&jets[i]);
    }
  }
  if(candidates.size()==1) return candidates[0];
  else if(candidates.size()==0) return NULL;

  cout<<endl;
  cout<<"[genjet]  ("<<genjet.Pt()<<", "<<genjet.Eta()<<", "<<genjet.Phi()<<")"<<endl;
  for(unsigned i=0; i<candidates.size(); i++) {
    cout<<"[recojet] ("<<candidates[i]->Pt()<<", "<<candidates[i]->Eta()<<", "<<candidates[i]->Phi()<<")"<<endl;
  }

  Jet* out;
  double ptratio=9999;
  for(unsigned i=0; i<candidates.size(); i++) {
    if(fabs(candidates[i]->Pt()/genjet.Pt()-1)<ptratio) {
      ptratio=fabs(candidates[i]->Pt()/genjet.Pt()-1);
      out=candidates[i];
    }
  }
  return out;
}

bool JBAnalyzerCore::isBottomMeson(Gen gen) {
  vector<int> PIDs = {511,521,10511,10521,513,523,10513,10523,20513,20523,515,525,531,10531,533,10533,20533,535,541,10541,543,10543,20543,545};
  for(unsigned i=0; i<PIDs.size(); i++) {
    if( fabs(gen.PID()) == PIDs[i] ) return true;
  }
  return false;
}
bool JBAnalyzerCore::isCharmMeson(Gen gen) {
  vector<int> PIDs = {411,421,10411,10421,413,423,10413,10423,20413,20423,415,425,431,10431,433,10433,20433,435};
  for(unsigned i=0; i<PIDs.size(); i++) {
    if( fabs(gen.PID()) == PIDs[i] ) return true;
  }
  return false;
}
bool JBAnalyzerCore::isStrangeMeson(Gen gen) {
  vector<int> PIDs = {130,310,311,321,9000311,9000321,10311,10321,100311,100321,9010311,9010321,9020311,9020321,313,323,10313,10323,20313,20323,100313,100323,9000313,9000323,30313,30323,315,325,9000315,9000325,10315,10325,20315,20325,9010315,9010325,9020315,9020325,317,327,9010317,9010327,319,329,9000319,9000329};
  for(unsigned i=0; i<PIDs.size(); i++) {
    if( fabs(gen.PID()) == PIDs[i] ) return true;
  }
  return false;
}
bool JBAnalyzerCore::isLightMeson(Gen gen) {
  vector<int> PIDs = {111,211,9000111,9000211,100111,100211,10111,10211,9010111,9010211,113,213,10113,10213,20113,20213,9000113,9000213,100113,100213,9010113,9010213,9020113,9020213,30113,30213,9030113,9030213,9040113,9040213,115,215,10115,10215,9000115,9000215,9010115,9010215,117,217,9000117,9000217,9010117,9010217,119,219,221,331,9000221,9010221,100221,10221,9020221,100331,9030221,10331,9040221,9050221,9060221,9070221,9080221,223,333,10223,20223,10333,20333,100223,9000223,9010223,30223,100333,225,9000225,335,9010225,9020225,10225,9030225,10335,9040225,9050225,9060225,9070225,9080225,9090225,227,337,229,9000229,9010229};
  for(unsigned i=0; i<PIDs.size(); i++) {
    if( fabs(gen.PID()) == PIDs[i] ) return true;
  }
  return false;
}
bool JBAnalyzerCore::isbbbar(Gen gen) {
  vector<int> PIDs = {551,10551,100551,110551,200551,210551,553,10553,20553,30553,100553,110553,120553,130553,200553,210553,220553,300553,9000553,9010553,555,10555,20555,100555,110555,120555,200555,557,100557};
  for(unsigned i=0; i<PIDs.size(); i++) {
    if( fabs(gen.PID()) == PIDs[i] ) return true;
  }
  return false;
}
bool JBAnalyzerCore::isccbar(Gen gen) {
  vector<int> PIDs = {441,10441,100441,443,10443,20443,100443,30443,9000443,9010443,9020443,445,100445};
  for(unsigned i=0; i<PIDs.size(); i++) {
    if( fabs(gen.PID()) == PIDs[i] ) return true;
  }
  return false;
}
bool JBAnalyzerCore::isbquark(Gen gen) {
  if( fabs(gen.PID()) == 5 ) return true;
  else return false;
}
bool JBAnalyzerCore::iscquark(Gen gen) {
  if( fabs(gen.PID()) == 4 ) return true;
  else return false;
}
bool JBAnalyzerCore::issquark(Gen gen) {
  if( fabs(gen.PID()) == 3 ) return true;
  else return false;
}
bool JBAnalyzerCore::isudquark(Gen gen) {
  if( fabs(gen.PID()) == 2 || fabs(gen.PID()) == 1 ) return true;
  else return false;
}
bool JBAnalyzerCore::fromBottomMeson(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isBottomMeson(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromCharmMeson(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isCharmMeson(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromStrangeMeson(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isStrangeMeson(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::frombbbar(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isbbbar(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromccbar(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isccbar(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromLightMeson(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isLightMeson(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::frombquark(Gen gen, vector<Gen> gens) {
   vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isbquark(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromcquark(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( iscquark(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromsquark(Gen gen, vector<Gen> gens) {
   vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( issquark(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromudquark(Gen gen, vector<Gen> gens) {
   vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isudquark(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromb(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isBottomMeson(TrackingGen) || isbbbar(TrackingGen) || isbquark(TrackingGen) ) return true;
  }
  return false;
}
bool JBAnalyzerCore::fromc(Gen gen, vector<Gen> gens) {
  vector<Gen> GenHistory = TrackMotherParticles(gen, gens);
  for(int i=GenHistory.size()-1; i>-1; i--) {
    Gen TrackingGen = GenHistory[i];
    if( isCharmMeson(TrackingGen) || isccbar(TrackingGen) || iscquark(TrackingGen) ) return true;
  }
  return false;
}

///////////////////
// --- Print --- //
///////////////////
void JBAnalyzerCore::GenName(Gen gen) {
  // quarks //
  if( gen.PID() == 1 ) cout<<"    d+";
  else if( gen.PID() == 2 ) cout<<"    u+";
  else if( gen.PID() == 3 ) cout<<"    s+";
  else if( gen.PID() == 4 ) cout<<"    c+";
  else if( gen.PID() == 5 ) cout<<"    b+";
  else if( gen.PID() == 6 ) cout<<"    t+";
  else if( gen.PID() == 9 ) cout<<"    g ";
  // diquarks //
  else if( gen.PID() == 2101 ) cout<<"   ud0";
  else if( gen.PID() == 2103 ) cout<<"   ud1";
  // leptons //
  else if( gen.PID() == 11 ) cout<<"   el-";
  else if( gen.PID() == 12 ) cout<<" nu_e ";
  else if( gen.PID() == 13 ) cout<<"   mu-";
  else if( gen.PID() == 14 ) cout<<" nu_m ";
  else if( gen.PID() == 15 ) cout<<"  tau-";
  else if( gen.PID() == 16 ) cout<<" nu_t ";
  // gauge and higgs bosons //
  else if( gen.PID() == 21 ) cout<<"    g ";
  else if( gen.PID() == 22 ) cout<<"gamma ";
  else if( gen.PID() == 23 ) cout<<"    Z ";
  else if( gen.PID() == 24 ) cout<<"    W+";
  else if( gen.PID() == 25 ) cout<<"    h ";
  // light I=1 mesons //
  else if( gen.PID() == 111 ) cout<<"   pi0";
  else if( gen.PID() == 211 ) cout<<"   pi+";
  else if( gen.PID() == 113 ) cout<<"  rho0";
  else if( gen.PID() == 213 ) cout<<"  rho+";
  else if( gen.PID() == 221 ) cout<<"  eta ";
  else if( gen.PID() == 331 ) cout<<" eta' ";
  else if( gen.PID() == 223 ) cout<<"omega ";
  else if( gen.PID() == 333 ) cout<<"  phi ";
  else if( gen.PID() == 20113 ) cout<<"a(1)0 ";
  else if( gen.PID() == 20213 ) cout<<"a(1)+ ";
  // strange mesons //
  else if( gen.PID() == 130 ) cout<<"  K_L0";
  else if( gen.PID() == 310 ) cout<<"  K_S0";
  else if( gen.PID() == 311 ) cout<<"   K0 ";
  else if( gen.PID() == 321 ) cout<<"   K+ ";
  else if( gen.PID() == 313 ) cout<<"  K*0 ";
  else if( gen.PID() == 323 ) cout<<"  K*+ ";
  else if( gen.PID() == 20313 ) cout<<"K(1)0 ";
  else if( gen.PID() == 20323 ) cout<<"K(1)+ ";
  // charmed mesons //
  else if( gen.PID() == 411 ) cout<<"    D+";
  else if( gen.PID() == 421 ) cout<<"    D0";
  else if( gen.PID() == 413 ) cout<<"   D*+";
  else if( gen.PID() == 423 ) cout<<"   D*0";
  else if( gen.PID() == 431 ) cout<<"   Ds+";
  else if( gen.PID() == 433 ) cout<<"  Ds*+";
  // bottom mesons //
  else if( gen.PID() == 511 ) cout<<"    B0";
  else if( gen.PID() == 521 ) cout<<"    B+";
  else if( gen.PID() == 513 ) cout<<"   B*0";
  else if( gen.PID() == 523 ) cout<<"   B*+";
  else if( gen.PID() == 531 ) cout<<"   Bs0";
  else if( gen.PID() == 533 ) cout<<"  Bs*+";
  // ccbar mesons //
  else if( gen.PID() == 441 ) cout<<"eta_c ";
  else if( gen.PID() == 443 ) cout<<"  J/P ";
  else if( gen.PID() == 100443 ) cout<<" J/P' ";
  // bbbar mesons //
  else if( gen.PID() == 551 ) cout<<"eta_b ";
  else if( gen.PID() == 553 ) cout<<"    Y ";
  // light baryons //
  else if( gen.PID() == 2212 ) cout<<"    p+";
  else if( gen.PID() == 2112 ) cout<<"    n ";
  // strange baryons //
  else if( gen.PID() == 3222 ) cout<<"Lambda";
  // charmed baryons //
  else if( gen.PID() == 4122 ) cout<<"Lam_c+";

  // +++ anti-particles +++ //
  // quarks //
  else if( gen.PID() == -1 ) cout<<"    d+";
  else if( gen.PID() == -2 ) cout<<"    u+";
  else if( gen.PID() == -3 ) cout<<"    s+";
  else if( gen.PID() == -4 ) cout<<"    c+";
  else if( gen.PID() == -5 ) cout<<"    b+";
  else if( gen.PID() == -6 ) cout<<"    t+";
  // leptons //
  else if( gen.PID() == -11 ) cout<<"   el+";
  else if( gen.PID() == -12 ) cout<<" nu_e~";
  else if( gen.PID() == -13 ) cout<<"   mu+";
  else if( gen.PID() == -14 ) cout<<" nu_m~";
  else if( gen.PID() == -15 ) cout<<"  tau+";
  else if( gen.PID() == -16 ) cout<<" nu_t~";
  // gauge and higgs bosons //
  else if( gen.PID() == -24 ) cout<<"    W-";
  // light I=1 mesons //
  else if( gen.PID() == -211 ) cout<<"   pi-";
  else if( gen.PID() == -213 ) cout<<"  rho-";
  else if( gen.PID() == -20113 ) cout<<"a(1)0~";
  else if( gen.PID() == -20213 ) cout<<"a(1)- ";
  // strange mesons //
  else if( gen.PID() == -311 ) cout<<"   K0~";
  else if( gen.PID() == -321 ) cout<<"   K- ";
  else if( gen.PID() == -313 ) cout<<"  K*0~";
  else if( gen.PID() == -323 ) cout<<"  K*- ";
  else if( gen.PID() == -20313 ) cout<<"K(1)0~";
  else if( gen.PID() == -20323 ) cout<<"K(1)- ";
  // charmed mesons //
  else if( gen.PID() == -411 ) cout<<"   D- ";
  else if( gen.PID() == -421 ) cout<<"   D0~";
  else if( gen.PID() == -413 ) cout<<"  D*- ";
  else if( gen.PID() == -423 ) cout<<"  D*0~";
  else if( gen.PID() == -431 ) cout<<"   Ds-";
  else if( gen.PID() == -433 ) cout<<"  Ds*-";
  // bottom mesons //
  else if( gen.PID() == -511 ) cout<<"   B0~";
  else if( gen.PID() == -521 ) cout<<"   B- ";
  else if( gen.PID() == -513 ) cout<<"  B*0~";
  else if( gen.PID() == -523 ) cout<<"  B*- ";
  else if( gen.PID() == -531 ) cout<<"  Bs0~";
  else if( gen.PID() == -533 ) cout<<"  Bs*-";
  // light baryons //
  else if( gen.PID() == -2212 ) cout<<"    p-";
  // charmed baryons //
  else if( gen.PID() == -4122 ) cout<<"Lam_c-";

  // others //
  else cout<<setw(6)<<gen.PID();

  return;
}

void JBAnalyzerCore::PrintPtEtaPhi(Gen gen) {
  cout<<fixed<<setprecision(2);
  cout<<"("<<setw(6)<<gen.Pt()<<","<<setw(6)<<gen.Eta()<<","<<setw(6)<<gen.Phi()<<")";
}
void JBAnalyzerCore::PrintPtEtaPhi(Lepton lep) {
  cout<<fixed<<setprecision(2);
  cout<<"("<<setw(6)<<lep.Pt()<<","<<setw(6)<<lep.Eta()<<","<<setw(6)<<lep.Phi()<<")";
}
void JBAnalyzerCore::PrintPtEtaPhi(Muon mu) {
  cout<<fixed<<setprecision(2);
  cout<<"("<<setw(6)<<mu.Pt()<<","<<setw(6)<<mu.Eta()<<","<<setw(6)<<mu.Phi()<<")";
}
void JBAnalyzerCore::PrintPtEtaPhi(Electron el) {
  cout<<fixed<<setprecision(2);
  cout<<"("<<setw(6)<<el.Pt()<<","<<setw(6)<<el.Eta()<<","<<setw(6)<<el.Phi()<<")";
}
void JBAnalyzerCore::PrintPtEtaPhi(Jet jet) {
  cout<<fixed<<setprecision(2);
  cout<<"("<<setw(6)<<jet.Pt()<<","<<setw(6)<<jet.Eta()<<","<<setw(6)<<jet.Phi()<<")";
}
void JBAnalyzerCore::PrintPtEtaPhi(Particle MET) {
  cout<<fixed<<setprecision(2);
  cout<<"("<<setw(6)<<MET.Pt()<<",    0.,"<<setw(6)<<MET.Phi()<<")";
}

//////////////////////
// --- Analysis --- //
//////////////////////
bool JBAnalyzerCore::DimuonCharge(JBAnalyzerParameter JBparam, Muon muon_1, Muon muon_2) {
  if( JBparam.DileptonSign == "OS" ) {
    if( muon_1.Charge() * muon_2.Charge() < 0 ) return true;
  }
  else if( JBparam.DileptonSign == "SS" ) {
    if( muon_1.Charge() * muon_2.Charge() > 0 ) return true;
  }
  else if( JBparam.DileptonSign == "PP" ) {
    if( muon_1.Charge() == 1 && muon_2.Charge() == 1 ) return true;
  }
  else if( JBparam.DileptonSign == "MM" ) {
    if( muon_1.Charge() == -1 && muon_2.Charge() == -1 ) return true;
  }
  return false;
}
bool JBAnalyzerCore::DileptonCharge(JBAnalyzerParameter JBparam, Muon muon, Electron elec) {
  if( JBparam.DileptonSign == "OS" ) {
    if( muon.Charge() * elec.Charge() < 0 ) return true;
  }
  else if( JBparam.DileptonSign == "SS" ) {
    if( muon.Charge() * elec.Charge() > 0 ) return true;
  }
  else if( JBparam.DileptonSign == "PP" ) {
    if( muon.Charge() == 1 && elec.Charge() == 1 ) return true;
  }
  else if( JBparam.DileptonSign == "MM" ) {
    if( muon.Charge() == -1 && elec.Charge() == -1 ) return true;
  }
  return false;
}


vector<Jet> JBAnalyzerCore::SelectBJets(vector<Jet> jets, Jet::Tagger tagger, Jet::WP WP, bool applySF, int systematic) {
  vector<Jet> out;
  for(unsigned i=0; i<jets.size(); i++) {
    if(IsBTagged(jets[i], tagger, WP, applySF, systematic))
      out.push_back(jets[i]);
  }
  return out;
}
vector<Jet> JBAnalyzerCore::SelectLightJets(vector<Jet> jets, Jet::Tagger tagger, Jet::WP WP, bool applySF, int systematic) {
  vector<Jet> out;
  for(unsigned i=0; i<jets.size(); i++) {
    if(!IsBTagged(jets[i], tagger, WP, applySF, systematic))
      out.push_back(jets[i]);
  }
  return out;
}

//////////////////////////
// --- Call objects --- //
//////////////////////////
std::vector<Muon> JBAnalyzerCore::JBGetAllMuons(){

  std::vector<Muon> out;
  for(unsigned int i=0; i<muon_pt->size(); i++){

    Muon mu;

    mu.SetCharge(muon_charge->at(i));
    mu.SetMiniAODPt(muon_pt->at(i));
    mu.SetMiniAODTunePPt(muon_TuneP_pt->at(i));

    double rc = muon_roch_sf->at(i);
    double rc_err = muon_roch_sf_up->at(i)-rc;
    //==== For the Rochester corection, up and down err are the same
    mu.SetMomentumScaleUpDown( muon_pt->at(i) * (rc+rc_err), muon_pt->at(i) * (rc-rc_err)  );
    mu.SetPtEtaPhiM(muon_pt->at(i)*rc, muon_eta->at(i), muon_phi->at(i), muon_mass->at(i));

    //==== TuneP
    //==== Apply scailing later with AnalyzerCore::UseTunePMuon()
    mu.SetTuneP4(muon_TuneP_pt->at(i), muon_TuneP_ptError->at(i), muon_TuneP_eta->at(i), muon_TuneP_phi->at(i), muon_TuneP_charge->at(i));

    // --- My Variables --- //
    mu.SetInner(muon_Inner_pt->at(i), muon_Inner_eta->at(i), muon_Inner_phi->at(i));
    mu.SetOuter(muon_Outer_pt->at(i), muon_Outer_eta->at(i), muon_Outer_phi->at(i));
    mu.SetBest(muon_Best_pt->at(i), muon_Best_eta->at(i), muon_Best_phi->at(i));
    mu.SetGLB(muon_GLB_pt->at(i), muon_GLB_eta->at(i), muon_GLB_phi->at(i));
    // -------------------- //
    mu.SetMVA(muon_MVA->at(i));

    mu.SetdXY(muon_dxyVTX->at(i), muon_dxyerrVTX->at(i));
    mu.SetdZ(muon_dzVTX->at(i), muon_dzerrVTX->at(i));
    mu.SetIP3D(muon_3DIPVTX->at(i), muon_3DIPerrVTX->at(i));
    mu.SetTypeBit(muon_TypeBit->at(i));
    mu.SetIDBit(muon_IDBit->at(i));
    mu.SetisPOGHighPt(muon_ishighpt->at(i));
    mu.SetIso(muon_PfChargedHadronIsoR04->at(i),muon_PfNeutralHadronIsoR04->at(i),muon_PfGammaIsoR04->at(i),muon_PFSumPUIsoR04->at(i),muon_trkiso->at(i));
    mu.SetNormalizedChi2(muon_normchi->at(i));
    mu.SetValidMuonHits(muon_validmuonhits->at(i));
    mu.SetMatchedStations(muon_matchedstations->at(i));
    mu.SetValidPixelHits(muon_pixelHits->at(i));
    mu.SetTrackerLayers(muon_trackerLayers->at(i));

    //==== Should be set after Eta is set
    mu.SetMiniIso(
      muon_PfChargedHadronMiniIso->at(i),
      muon_PfNeutralHadronMiniIso->at(i),
      muon_PfGammaMiniIso->at(i),
      muon_PFSumPUMiniIso->at(i),
      Rho,
      mu.EA()
    );

    out.push_back(mu);

  }
  return out;

}


std::vector<GenJet> JBAnalyzerCore::GetAllGenJets(){
  std::vector<GenJet> out;
/*
  for(unsigned int i=0; i<genjet_pt->size(); i++){
    GenJet genjet;
    genjet.SetPtEtaPhiM(genjet_pt->at(i), genjet_eta->at(i), genjet_phi->at(i), genjet_m->at(i));
    //genjet.SetCharge(genjet_charge->at(i));
    //genjet.SetArea(genjet_area->at(i));

    out.push_back(genjet);
  }
*/

  return out;
}

std::vector<GenFatJet> JBAnalyzerCore::GetAllGenFatJets(){
  std::vector<GenFatJet> out;
/*
  for(unsigned int i=0; i<genfatjet_pt->size(); i++){
    GenFatJet genfatjet;
    genfatjet.SetPtEtaPhiM(genfatjet_pt->at(i), genfatjet_eta->at(i), genfatjet_phi->at(i), genfatjet_m->at(i));
    //genfatjet.SetCharge(genfatjet_charge->at(i));
    //genfatjet.SetArea(genfatjet_area->at(i));

    out.push_back(genfatjet);
  }
*/

  return out;
}

/////////////////////
// --- Plotter --- //
/////////////////////
void JBAnalyzerCore::LeptonPlotter(std::vector<Lepton *> leps, TString this_region, double weight){

  for(unsigned int i=0; i<leps.size(); i++){

    TString this_itoa = TString::Itoa(i,10);

    Lepton *lep = leps[i];

    FillHist(this_region+"/Lepton_"+this_itoa+"_Pt___"+this_region, lep->Pt(), weight, 20000, 0., 2000.);
    FillHist(this_region+"/Lepton_"+this_itoa+"_Eta___"+this_region, lep->Eta(), weight, 600, -3., 3.);
    FillHist(this_region+"/Lepton_"+this_itoa+"_Phi___"+this_region, lep->Phi(), weight, 640, -3.2, 3.2);
    FillHist(this_region+"/Lepton_"+this_itoa+"_RelIso___"+this_region, lep->RelIso(), weight, 500, 0., 5.);
    /*
    FillHist(this_region+"/Lepton_"+this_itoa+"_MiniRelIso___"+this_region, lep->MiniRelIso(), weight, 100, 0., 1.);

    FillHist(this_region+"/Lepton_"+this_itoa+"_dXY___"+this_region, fabs(lep->dXY()), weight, 500, 0., 0.05);
    FillHist(this_region+"/Lepton_"+this_itoa+"_dXYSig___"+this_region, fabs(lep->dXY()/lep->dXYerr()), weight, 100, 0., 10);
    FillHist(this_region+"/Lepton_"+this_itoa+"_dZ___"+this_region, fabs(lep->dZ()), weight, 500, 0., 0.5);
    FillHist(this_region+"/Lepton_"+this_itoa+"_dZSig___"+this_region, fabs(lep->dZ()/lep->dZerr()), weight, 100, 0., 10);
    FillHist(this_region+"/Lepton_"+this_itoa+"_IP3D___"+this_region, fabs(lep->IP3D()), weight, 500, 0., 0.5);
    FillHist(this_region+"/Lepton_"+this_itoa+"_IP3DSig___"+this_region, fabs(lep->IP3D()/lep->IP3Derr()), weight, 100, 0., 10);
    */
    FillHist(this_region+"/Lepton_Pt___"+this_region, lep->Pt(), weight, 20000, 0., 2000.);
    FillHist(this_region+"/Lepton_Eta___"+this_region, lep->Eta(), weight, 600, -3., 3.);
    FillHist(this_region+"/Lepton_Phi___"+this_region, lep->Phi(), weight, 640, -3.2, 3.2);
    FillHist(this_region+"/Lepton_RelIso___"+this_region, lep->RelIso(), weight, 500, 0., 5.);


    if(lep->LeptonFlavour()==Lepton::ELECTRON){
      Electron *el = (Electron *)lep;
      // FillHist(this_region+"/Lepton_"+this_itoa+"_MVANoIso___"+this_region, el->MVANoIso(), weight, 200, -1., 1.);
      FillHist(this_region+"/Lepton_"+this_itoa+"_RelIso___"+this_region, el->RelIso(), weight, 500, 0., 5.);
    }
    else if(lep->LeptonFlavour()==Lepton::MUON){
      Muon *mu = (Muon *)lep;
      // FillHist(this_region+"/Lepton_"+this_itoa+"_Chi2___"+this_region, mu->Chi2(), weight, 500, 0., 50.);
      FillHist(this_region+"/Lepton_"+this_itoa+"_TrkRelIso___"+this_region, mu->TrkIso()/mu->TuneP4().Pt(), weight, 500, 0., 5.);
      //FillHist(this_region+"/Lepton_TrkRelIso___"+this_region, mu->TrkIso()/mu->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton_TrkRelIso___"+this_region, mu->TrkIso()/mu->TuneP4().Pt(), weight, 50, 0., 5.);
      FillHist(this_region+"/Lepton_isGlobalMuon___"+this_region, mu->isGlobalMuon(), weight, 2, 0, 2);
      FillHist(this_region+"/Lepton_isPFMuon___"+this_region, mu->isPFMuon(), weight, 2, 0, 2);
      FillHist(this_region+"/Lepton_NormalizedChi2___"+this_region, mu->NormalizedChi2(), weight, 10, 0, 10);
      FillHist(this_region+"/Lepton_ValidMuonHits___"+this_region, mu->ValidMuonHits(), weight, 20, 0, 20);
      FillHist(this_region+"/Lepton_MatchedStations___"+this_region, mu->MatchedStations(), weight, 5, 0, 5);
      FillHist(this_region+"/Lepton_dXY___"+this_region, mu->dXY(), weight, 40, -.1, .1);
      FillHist(this_region+"/Lepton_dXYerr___"+this_region, mu->dXYerr(), weight, 40, 0., .02);
      FillHist(this_region+"/Lepton_dZ___"+this_region, mu->dZ(), weight, 80, -.2, .2);
      FillHist(this_region+"/Lepton_dZerr___"+this_region, mu->dZerr(), weight, 40, 0., .02);
      FillHist(this_region+"/Lepton_ValidPixelHits___"+this_region, mu->ValidPixelHits(), weight, 10, 0, 10);
      FillHist(this_region+"/Lepton_TrackerLayers___"+this_region, mu->TrackerLayers(), weight, 20, 0, 20);
      FillHist(this_region+"/Lepton_IP3D___"+this_region, mu->IP3D(), weight, 40, -.1, .1);
      FillHist(this_region+"/Lepton_IP3Derr___"+this_region, mu->IP3Derr(), weight, 40, 0., .2);
    }
    else{
      cout << "[JBAnalyzerCore::LeptonPlotter] lepton flavour wrong.." << endl;
      exit(EXIT_FAILURE);
    }


  }

}

void JBAnalyzerCore::DileptonPlotter(std::vector<Lepton*> leps, TString this_region, double weight) {
  Lepton Leading_Lepton = *leps[0];
  Lepton Sub_Lepton = *leps[1];

  FillHist(this_region+"/Dilepton_Mass___"+this_region, (Leading_Lepton + Sub_Lepton).M(), weight, 20000,0.,2000.);
  FillHist(this_region+"/Dilepton_EWKMass___"+this_region, (Leading_Lepton + Sub_Lepton).M(), weight, 1500,0.,150.);
  FillHist(this_region+"/Dilepton_LowMass___"+this_region, (Leading_Lepton + Sub_Lepton).M(), weight, 1200,0.,12.);
  FillHist(this_region+"/Dilepton_DeltaR___"+this_region, Leading_Lepton.DeltaR( Sub_Lepton ), weight, 5000,0.,5.0);

  FillHist(this_region+"/Dilepton_Pt___"+this_region, (Leading_Lepton + Sub_Lepton).Pt(), weight, 20000,0.,2000.);
  FillHist(this_region+"/Dilepton_HT___"+this_region, Leading_Lepton.Pt() + Sub_Lepton.Pt(), weight, 20000,0.,2000.);

  FillHist(this_region+"/Dilepton_PtDiff___"+this_region, abs( Leading_Lepton.Pt()-Sub_Lepton.Pt() ), weight, 10000,0.,1000.);
  FillHist(this_region+"/Dilepton_RelPtDiff___"+this_region, abs( Leading_Lepton.Pt()-Sub_Lepton.Pt() )/Leading_Lepton.Pt(), weight, 1000,0.,1.);

  /*
  if( leps[0]->LeptonFlavour()==Lepton::MUON) {
    Muon* Leading_Muon = (Muon*)leps[0];
    Muon* Sub_Muon = (Muon*)leps[1];
    // FillHist(this_region+"/Dilepton_TuneP4_DeltaR___"+this_region, Leading_Muon->TuneP4().DeltaR( Sub_Muon->TuneP4() ), weight, 5000,0.,0.5);
    // FillHist(this_region+"/Dilepton_Inner_DeltaR___"+this_region, Leading_Muon->Inner().DeltaR( Sub_Muon->Inner() ), weight, 5000,0.,0.5);
    FillHist(this_region+"/Dilepton_Outer_DeltaR___"+this_region, Leading_Muon->Outer().DeltaR( Sub_Muon->Outer() ), weight, 5000,0.,0.5);
    // FillHist(this_region+"/Dilepton_Best_DeltaR___"+this_region, Leading_Muon->Best().DeltaR( Sub_Muon->Best() ), weight, 5000,0.,0.5);
    // FillHist(this_region+"/Dilepton_GLB_DeltaR___"+this_region, Leading_Muon->GLB().DeltaR( Sub_Muon->GLB() ), weight, 5000,0.,0.5);
  }
  */
}

void JBAnalyzerCore::JetPlotter(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight){

  for(unsigned int i=0; i<jets.size(); i++){

    TString this_itoa = TString::Itoa(i,10);
    FillHist(this_region+"/Jet_"+this_itoa+"_Pt___"+this_region, jets.at(i).Pt(), weight, 5000, 0., 500.);
    FillHist(this_region+"/Jet_"+this_itoa+"_Eta___"+this_region, jets.at(i).Eta(), weight, 600, -3., 3.);
    FillHist(this_region+"/Jet_"+this_itoa+"_Phi___"+this_region, jets.at(i).Phi(), weight, 600, -3., 3.);
    FillHist(this_region+"/Jet_"+this_itoa+"_Mass___"+this_region, jets.at(i).M(), weight, 10000, 0., 1000.);
  }

  for(unsigned int i=0; i<fatjets.size(); i++){

    TString this_itoa = TString::Itoa(i,10);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Pt___"+this_region, fatjets.at(i).Pt(), weight, 10000, 0., 1000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Eta___"+this_region, fatjets.at(i).Eta(), weight, 600, -3., 3.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Mass___"+this_region, fatjets.at(i).M(), weight, 3000, 0., 3000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_SDMass___"+this_region, fatjets.at(i).SDMass(), weight, 3000, 0., 3000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_LSF___"+this_region, fatjets.at(i).LSF(), weight, 100, 0., 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau21___"+this_region, fatjets.at(i).PuppiTau2()/fatjets.at(i).PuppiTau1(), weight, 100, 0., 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau31___"+this_region, fatjets.at(i).PuppiTau3()/fatjets.at(i).PuppiTau1(), weight, 100, 0., 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau32___"+this_region, fatjets.at(i).PuppiTau3()/fatjets.at(i).PuppiTau2(), weight, 100, 0., 1.);
  }

}

void JBAnalyzerCore::DijetPlotter(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight) {
  FillHist(this_region+"/Dijet_Mass___"+this_region, (jets[0] + jets[1]).M(), weight, 10000,0.,10000.);
  FillHist(this_region+"/Dijet_LowMass___"+this_region, (jets[0] + jets[1]).M(), weight, 2000,0.,20.);
  FillHist(this_region+"/Dijet_DeltaR___"+this_region, jets[0].DeltaR( jets[1] ), weight, 5000,0.,5.0);

  FillHist(this_region+"/Dijet_PtDiff___"+this_region, abs( jets[0].Pt()-jets[1].Pt() ), weight, 10000,0.,1000.);
  FillHist(this_region+"/Dijet_RelPtDiff___"+this_region, abs( jets[0].Pt()-jets[1].Pt() )/jets[0].Pt(), weight, 1000,0.,1.);
}


