#include "GenAnalyzer_NIsoMuon.h"

void GenAnalyzer_NIsoMuon::initializeAnalyzer(){
  /////////////////////
  // --- Trigger --- //
  /////////////////////
  IsoMuTriggerName = {};
  Leading_Muon_Pt = 999.;
  Subleading_Muon_Pt = 10.; // *** CHANGE SUBLEADING MUON PT *** //

  if( DataYear == 2016 ) {
    if( TriggerInput == "SingleMuon" ) {
      IsoMuTriggerName = {
        "HLT_IsoMu24_v", "HLT_IsoTkMu24_v"
      };
      Leading_Muon_Pt = 26.;
    }
    else if( TriggerInput == "HighPtMuon" ) {
      IsoMuTriggerName = {
        "HLT_Mu50_v", "HLT_TkMu50_v"
      };
      Leading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon" ) {
      IsoMuTriggerName = {
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
      IsoMuTriggerName = {
        "HLT_Mu17_Mu8_SameSign_DZ_v"
      };
      Leading_Muon_Pt = 20.;
      Subleading_Muon_Pt = 10.;
    }
    else if( TriggerInput == "NonIsolatedDoubleMuon" ) {
      IsoMuTriggerName = { "HLT_Mu30_TkMu11_v" };
      Leading_Muon_Pt = 32.;
      Subleading_Muon_Pt = 13.;
      //Subleading_Muon_Pt = 32.;
    }
  }

}

void GenAnalyzer_NIsoMuon::executeEvent(){
  //AllMuons = JBGetAllMuons();
  //AllJets = GetAllJets();
  //AllFatJets = GetAllFatJets();

  weight_Prefire = GetPrefireWeight(0);

  // cout<<run<<endl;

  AnalyzerParameter param;
  JBAnalyzerParameter JBparam;


  executeEventFromParameter(param, JBparam);

}

void GenAnalyzer_NIsoMuon::executeEventFromParameter(AnalyzerParameter param, JBAnalyzerParameter JBparam){

  //if(!PassMETFilter()) return;

  Event ev = GetEvent();

  if( !(ev.PassTrigger(IsoMuTriggerName) ) ) return;


  double weight=1;
  if(!IsDATA) {
    weight = weight_norm_1invpb * ev.GetTriggerLumi("Full"); // Is IsoMu24 unprescaled?
    weight *= ev.MCweight();
    weight *= weight_Prefire;;
  }

  vector<Gen> gens = GetGens();
  vector<Muon> AllMuons = GetAllMuons();
  vector<Jet> AllJets = GetAllJets();
  vector<Gen> MuFromBottomMeson, MuFromCharmMeson, MuFromStrangeMeson, MuFrombbbar, MuFromccbar, MuFromLightMeson, MuFromOthers;

  vector<Jet> jets = SelectJets(AllJets, "tight", 30.,2.4);

  /*
  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];

    if( !(gen.Status()==1) ) continue;
    if( !(gen.MotherIndex()!=0 && gen.MotherIndex()!=1) ) continue;
    if( !(gen.Pt() > Subleading_Muon_Pt) ) continue;
    if( !(fabs(gen.Eta()) < 2.4) ) continue;

    if( fabs(gen.PID())==11 ) {
      if( fromb(gen,gens) ) {
        FillHist("AtLeastOneElectron/fromBottom", gen.Pt(), weight, 1000,0.,1000.);
      }
      else if( fromc(gen,gens) ) {
        FillHist("AtLeastOneElectron/fromCharm", gen.Pt(), weight, 1000,0.,1000.);
      }
      else if( fromStrangeMeson(gen,gens) || fromsquark(gen,gens) ) {
        FillHist("AtLeastOneElectron/fromStrange", gen.Pt(), weight, 1000,0.,1000.);
      }
      else if( fromLightMeson(gen,gens) || fromudquark(gen,gens) ) {
        FillHist("AtLeastOneElectron/fromUpDown", gen.Pt(), weight, 1000,0.,1000.);
      }
      else {
        FillHist("AtLeastOneElectron/fromOthers", gen.Pt(), weight, 1000,0.,1000.);
      }
    }
    else if( fabs(gen.PID())==13 ) {
      if( fromb(gen,gens) ) {
        FillHist("AtLeastOneMuon/fromBottom", gen.Pt(), weight, 1000,0.,1000.);
      }
      else if( fromc(gen,gens) ) {
        FillHist("AtLeastOneMuon/fromCharm", gen.Pt(), weight, 1000,0.,1000.);
      }
      else if( fromStrangeMeson(gen,gens) || fromsquark(gen,gens) ) {
        FillHist("AtLeastOneMuon/fromStrange", gen.Pt(), weight, 1000,0.,1000.);
      }
      else if( fromLightMeson(gen,gens) || fromudquark(gen,gens) ) {
        FillHist("AtLeastOneMuon/fromUpDown", gen.Pt(), weight, 1000,0.,1000.);
      }
      else {
        FillHist("AtLeastOneMuon/fromOthers", gen.Pt(), weight, 1000,0.,1000.);
      }
    }
  }
  */

  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];

    if( !(gen.Status()==1) ) continue;
    if( !(gen.MotherIndex()!=0 && gen.MotherIndex()!=1) ) continue;
    if( !(fabs(gen.PID())==13) ) continue;
    if( !(gen.Pt() > Subleading_Muon_Pt) ) continue;
    if( !(fabs(gen.Eta()) < 2.4) ) continue;

    if( fromBottomMeson(gen,gens) || frombbbar(gen,gens) || frombquark(gen,gens) ) { MuFromBottomMeson.push_back( gen ); }
    //else if( frombbbar(gen,gens) ) { MuFrombbbar.push_back( gen ); }
    else if( fromCharmMeson(gen,gens) || fromccbar(gen,gens) || fromcquark(gen,gens) ) { MuFromCharmMeson.push_back( gen ); }
    //else if( fromccbar(gen,gens) ) { MuFromccbar.push_back( gen ); }
    else if( fromStrangeMeson(gen,gens) || fromsquark(gen,gens) ) { MuFromStrangeMeson.push_back( gen ); }
    else if( fromLightMeson(gen,gens) || fromudquark(gen,gens) ) { MuFromLightMeson.push_back( gen ); }
    else if( !gen.isPrompt() ) { MuFromOthers.push_back( gen ); }
    //else { cout<<"mu from others"<<endl; }
  }


  std::sort( MuFromBottomMeson.begin() , MuFromBottomMeson.end() , PtComparing );
  std::sort( MuFrombbbar.begin() , MuFrombbbar.end() , PtComparing );
  std::sort( MuFromCharmMeson.begin() , MuFromCharmMeson.end() , PtComparing );
  std::sort( MuFromccbar.begin() , MuFromccbar.end() , PtComparing );
  std::sort( MuFromStrangeMeson.begin() , MuFromStrangeMeson.end() , PtComparing );
  std::sort( MuFromLightMeson.begin() , MuFromLightMeson.end() , PtComparing );
  std::sort( MuFromOthers.begin() , MuFromOthers.end() , PtComparing );


  if( MuFromBottomMeson.size() > 1 && MuFromBottomMeson[0].Pt() > Leading_Muon_Pt ) {
    FillHist("FromBottomMeson/Dimuon_DeltaR", MuFromBottomMeson[0].DeltaR( MuFromBottomMeson[1] ), weight, 500,0,5.);
    FillHist("FromBottomMeson/Muon_Pt", MuFromBottomMeson[0].Pt(), weight, 200,0,200);
    FillHist("FromBottomMeson/Muon_Pt", MuFromBottomMeson[1].Pt(), weight, 200,0,200);
    FillHist("FromBottomMeson/Muon_Eta", MuFromBottomMeson[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromBottomMeson/Muon_Eta", MuFromBottomMeson[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromBottomMeson/Dimuon_Mass", (MuFromBottomMeson[0]+MuFromBottomMeson[1]).M(), weight, 200,0,200);
  }
  /*
  else if( MuFrombbbar.size() > 1 && MuFrombbbar[0].Pt() > Leading_Muon_Pt ) {
    FillHist("Frombbbar/Dimuon_DeltaR", MuFrombbbar[0].DeltaR( MuFrombbbar[1] ), weight, 500,0,5.);
    FillHist("Frombbbar/Muon_Pt", MuFrombbbar[0].Pt(), weight, 200,0,200);
    FillHist("Frombbbar/Muon_Pt", MuFrombbbar[1].Pt(), weight, 200,0,200);
    FillHist("Frombbbar/Muon_Eta", MuFrombbbar[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("Frombbbar/Muon_Eta", MuFrombbbar[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("Frombbbar/Dimuon_Mass", (MuFrombbbar[0]+MuFrombbbar[1]).M(), weight, 200,0,200);
  }
  */
  else if( MuFromCharmMeson.size() > 1 && MuFromCharmMeson[0].Pt() > Leading_Muon_Pt ) {
    FillHist("FromCharmMeson/Dimuon_DeltaR", MuFromCharmMeson[0].DeltaR( MuFromCharmMeson[1] ), weight, 500,0,5.);
    FillHist("FromCharmMeson/Muon_Pt", MuFromCharmMeson[0].Pt(), weight, 200,0,200);
    FillHist("FromCharmMeson/Muon_Pt", MuFromCharmMeson[1].Pt(), weight, 200,0,200);
    FillHist("FromCharmMeson/Muon_Eta", MuFromCharmMeson[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromCharmMeson/Muon_Eta", MuFromCharmMeson[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromCharmMeson/Dimuon_Mass", (MuFromCharmMeson[0]+MuFromCharmMeson[1]).M(), weight, 200,0,200);
  }
  /*
  else if( MuFromccbar.size() > 1 && MuFromccbar[0].Pt() > Leading_Muon_Pt ) {
    FillHist("Fromccbar/Dimuon_DeltaR", MuFromccbar[0].DeltaR( MuFromccbar[1] ), weight, 500,0,5.);
    FillHist("Fromccbar/Muon_Pt", MuFromccbar[0].Pt(), weight, 200,0,200);
    FillHist("Fromccbar/Muon_Pt", MuFromccbar[1].Pt(), weight, 200,0,200);
    FillHist("Fromccbar/Muon_Eta", MuFromccbar[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("Fromccbar/Muon_Eta", MuFromccbar[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("Fromccbar/Dimuon_Mass", (MuFromccbar[0]+MuFromccbar[1]).M(), weight, 200,0,200);
  }
  */
  else if( MuFromStrangeMeson.size() > 1 && MuFromStrangeMeson[0].Pt() > Leading_Muon_Pt ) {
    FillHist("FromStrangeMeson/Dimuon_DeltaR", MuFromStrangeMeson[0].DeltaR( MuFromStrangeMeson[1] ), weight, 500,0,5.);
    FillHist("FromStrangeMeson/Muon_Pt", MuFromStrangeMeson[0].Pt(), weight, 200,0,200);
    FillHist("FromStrangeMeson/Muon_Pt", MuFromStrangeMeson[1].Pt(), weight, 200,0,200);
    FillHist("FromStrangeMeson/Muon_Eta", MuFromStrangeMeson[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromStrangeMeson/Muon_Eta", MuFromStrangeMeson[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromStrangeMeson/Dimuon_Mass", (MuFromStrangeMeson[0]+MuFromStrangeMeson[1]).M(), weight, 200,0,200);
  }
  else if( MuFromLightMeson.size() > 1 && MuFromLightMeson[0].Pt() > Leading_Muon_Pt ) {
    FillHist("FromLightMeson/Dimuon_DeltaR", MuFromLightMeson[0].DeltaR( MuFromLightMeson[1] ), weight, 500,0,5.);
    FillHist("FromLightMeson/Muon_Pt", MuFromLightMeson[0].Pt(), weight, 200,0,200);
    FillHist("FromLightMeson/Muon_Pt", MuFromLightMeson[1].Pt(), weight, 200,0,200);
    FillHist("FromLightMeson/Muon_Eta", MuFromLightMeson[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromLightMeson/Muon_Eta", MuFromLightMeson[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromLightMeson/Dimuon_Mass", (MuFromLightMeson[0]+MuFromLightMeson[1]).M(), weight, 200,0,200);
  }
  else if( MuFromOthers.size() > 1 && MuFromOthers[0].Pt() > Leading_Muon_Pt ) {
    FillHist("FromOthers/Dimuon_DeltaR", MuFromOthers[0].DeltaR( MuFromOthers[1] ), weight, 500,0,5.);
    FillHist("FromOthers/Muon_Pt", MuFromOthers[0].Pt(), weight, 200,0,200);
    FillHist("FromOthers/Muon_Pt", MuFromOthers[1].Pt(), weight, 200,0,200);
    FillHist("FromOthers/Muon_Eta", MuFromOthers[0].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromOthers/Muon_Eta", MuFromOthers[1].Eta(), weight, 100,-5.0,5.0);
    FillHist("FromOthers/Dimuon_Mass", (MuFromOthers[0]+MuFromOthers[1]).M(), weight, 200,0,200);
  }

  if( MuFromBottomMeson.size() > 1 ) {
    bool breaker=false;
    for(unsigned i=0; i<MuFromBottomMeson.size(); i++) {
      if( !(MuFromBottomMeson[i].Pt() > Leading_Muon_Pt) ) continue;

      

      //vector<Gen> GenHistory_1 = TrackMotherParticles(MuFromBottomMeson[i], gens);
      //for(int iHistory=GenHistory.size()-1; iHistory>-1; iHistory--) {
      //  Gen TrackingGen = GenHistory_1[iHistory];
      //  if( is 


      for(unsigned j=i+1; j<MuFromBottomMeson.size(); j++) {
        if( !(MuFromBottomMeson[i].DeltaR( MuFromBottomMeson[j] ) > 0.4) ) continue;

        FillHist("FromBottomMeson_dR04/Dimuon_DeltaR", MuFromBottomMeson[i].DeltaR( MuFromBottomMeson[j] ), weight, 500,0,5.);
        FillHist("FromBottomMeson_dR04/Muon_Pt", MuFromBottomMeson[i].Pt(), weight, 200,0,200);
        FillHist("FromBottomMeson_dR04/Muon_Pt", MuFromBottomMeson[j].Pt(), weight, 200,0,200);
        FillHist("FromBottomMeson_dR04/Muon_Eta", MuFromBottomMeson[i].Eta(), weight, 100,-5.0,5.0);
        FillHist("FromBottomMeson_dR04/Muon_Eta", MuFromBottomMeson[j].Eta(), weight, 100,-5.0,5.0);
        FillHist("FromBottomMeson_dR04/Dimuon_Mass", (MuFromBottomMeson[i]+MuFromBottomMeson[j]).M(), weight, 200,0,200);

        breaker=true;
        break;
      }
      if( breaker ) break;
    }
  }
  else if( MuFromCharmMeson.size() > 1 ) {
    bool breaker = false;
    for(unsigned i=0; i<MuFromCharmMeson.size(); i++) {
      if( !(MuFromCharmMeson[i].Pt() > Leading_Muon_Pt) ) continue;
      for(unsigned j=i+1; j<MuFromCharmMeson.size(); j++) {
        if( !(MuFromCharmMeson[i].DeltaR( MuFromCharmMeson[j] ) > 0.4) ) continue;

        FillHist("FromCharmMeson_dR04/Dimuon_DeltaR", MuFromCharmMeson[i].DeltaR( MuFromCharmMeson[j] ), weight, 500,0,5.);
        FillHist("FromCharmMeson_dR04/Muon_Pt", MuFromCharmMeson[i].Pt(), weight, 200,0,200);
        FillHist("FromCharmMeson_dR04/Muon_Pt", MuFromCharmMeson[j].Pt(), weight, 200,0,200);
        FillHist("FromCharmMeson_dR04/Muon_Eta", MuFromCharmMeson[i].Eta(), weight, 100,-5.0,5.0);
        FillHist("FromCharmMeson_dR04/Muon_Eta", MuFromCharmMeson[j].Eta(), weight, 100,-5.0,5.0);
        FillHist("FromCharmMeson_dR04/Dimuon_Mass", (MuFromCharmMeson[i]+MuFromCharmMeson[j]).M(), weight, 200,0,200);

        breaker=true;
        break;
      }
      if( breaker ) break;
    }
  }


}

Jet GenAnalyzer_NIsoMuon::JetMatching(Gen gen, vector<Jet> jets, double cone_size) {
  Jet jet;
  for(unsigned i=0; i<jets.size(); i++) {
    jet = jets[i];
    if( jet.DeltaR( gen ) < 0.3 ) {
      return jet;
    }
  }
  return jet;
}


GenAnalyzer_NIsoMuon::GenAnalyzer_NIsoMuon(){

}

GenAnalyzer_NIsoMuon::~GenAnalyzer_NIsoMuon(){

}


