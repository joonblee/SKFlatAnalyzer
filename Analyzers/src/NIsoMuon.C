#include "NIsoMuon.h"

void NIsoMuon::initializeAnalyzer(){
  MuonIDs = {
    "POGTightWithTightTrkIso",
    "POGTight",
    // "POGHighPt",
    // "POGMedium",
    // "POGLoose",
    "POGTightWithVetoTightTrkIso",
    "POGTightWithVetoLooseTrkIso"
  };

  MuonIDSFKey = "NUM_TightID_DEN_genTracks";
  MuonISOSFKey = "NUM_TightRelIso_DEN_TightIDandIPCut";

  JetID = "tight";

  NIsoMuonIDs = {
    "POGTightWithTightTrkIso",
    "POGTight",
    "POGTightWithVetoTightTrkIso",
    "POGTightWithVetoLooseTrkIso"
  };

  IsoMuTriggerName = {};
  Leading_Muon_Pt = 999.;
  Subleading_Muon_Pt = 10.;

  //cout<<endl<<TriggerInput<<endl<<endl;

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
    else if( TriggerInput == "SingleMuon-HighPtCut" ) {
      IsoMuTriggerName = {
        "HLT_IsoMu24_v", "HLT_IsoTkMu24_v"
      };
      Leading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon-HighPtCut" ) {
      IsoMuTriggerName = {
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
        "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
        "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"//,
        //"HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
        //"HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"
      };
      Leading_Muon_Pt = 52.;
    }

  }
  else if( DataYear == 2017 ) {
    if( TriggerInput == "SingleMuon" ) {
      IsoMuTriggerName = {
        "HLT_IsoMu27_v"
      };
      Leading_Muon_Pt = 30.;
    }
    else if( TriggerInput == "HighPtMuon" ) {
      IsoMuTriggerName = {
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v"
      };
      Leading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon" ) {
      IsoMuTriggerName = {
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v"
      };
      Leading_Muon_Pt = 20.;
    }
  }
  else if( DataYear == 2018 ) {
    if( TriggerInput == "SingleMuon" ) {
      IsoMuTriggerName = {
        "HLT_IsoMu24_v"
      };
      Leading_Muon_Pt = 26.;
    }
    else if( TriggerInput == "HighPtMuon" ) {
      IsoMuTriggerName = {
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v"
      };
      Leading_Muon_Pt = 52.;
    }
    else if( TriggerInput == "DoubleMuon" ) {
      IsoMuTriggerName = {
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"
      };
      Leading_Muon_Pt = 20.;
    }
  }

  cout<<endl;
  cout<<" + + + Input Check Lists + + + "<<endl;
  cout<<" + Trigger Input : "<<TriggerInput<<endl;
  cout<<" + Data Year     : "<<DataYear<<endl;
  cout<<" + + + + + + + + + + + + + + + "<<endl;
  cout<<endl;
}

void NIsoMuon::executeEvent(){
  AllMuons = GetAllMuons();
  AllJets = GetAllJets();

  weight_Prefire = GetPrefireWeight(0);

  // cout<<run<<endl;

  AnalyzerParameter param;

  for(unsigned int it_MuonID=0; it_MuonID<MuonIDs.size(); it_MuonID++) {
    TString MuonID = MuonIDs.at(it_MuonID);

    param.Clear();

    param.Muon_Tight_ID = MuonID;

    param.Muon_ID_SF_Key = MuonIDSFKey;
    param.Muon_ISO_SF_Key = MuonISOSFKey;

    param.Jet_ID = JetID;

    for(unsigned int it_NIsoMuonID=0; it_NIsoMuonID<NIsoMuonIDs.size(); it_NIsoMuonID++) {
      TString NIsoMuonID = NIsoMuonIDs.at(it_NIsoMuonID);

      param.Muon_Veto_ID = NIsoMuonID;

      param.Name = MuonID+"_"+NIsoMuonID;

      executeEventFromParameter(param);
    }
  }
}

void NIsoMuon::executeEventFromParameter(AnalyzerParameter param){

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?

  Event ev = GetEvent();

  if( !(ev.PassTrigger(IsoMuTriggerName) ) ) return;


  vector<Muon> muons = SelectMuons(AllMuons, param.Muon_Tight_ID, Subleading_Muon_Pt, 2.4);
  vector<Muon> Vmuons = SelectMuons(AllMuons, param.Muon_Veto_ID, Subleading_Muon_Pt, 2.4);
  vector<Jet> jets = SelectJets(AllJets, param.Jet_ID, 30.,2.7);

  // Does ExampleRun.C code contain muon scaling/smearing & JEC?
  //std::sort(muons.begin(), muons.end(), PtComparing);
  //std::sort(jets.begin(), jets.end(), PtComparing);

  vector<Lepton*> dimuon;

  double weight = 1.;
  if(!IsDATA) weight = MCweight( ev );


  vector<int> sign = {-1, 1};
  for(int it_sign = 0; it_sign < 2; it_sign++) {
    TString str_sign;
    if( it_sign ) str_sign = "SS_"; else str_sign = "OS_";

    // ----- Event Selection ----- //
    dimuon = NIsoMuonSelection( muons, Vmuons, sign[it_sign] );
    if( dimuon.size() != 2 ) return;
  

    // - + - + - + - + - + - Plotting - + - + - + - + - //
    FillLeptonPlots( dimuon , str_sign+param.Name , weight );
    FillDileptonPlots( dimuon , str_sign+param.Name , weight );

    /*
    // --- deltaR cut --- //
    vector<double> dR_cut = {0.017,0.1,0.2,0.3,0.6,999.};
    if( TriggerInput == "DoubleMuon" )
      dR_cut[0] = 0.02;  

    double dR_MuMu = dimuon[0]->DeltaR( *dimuon[1] );
  
    for(unsigned i = 0; i < dR_cut.size(); i++) {
      if( dR_MuMu > dR_cut[i] ) continue;
      //TString str_dR_cut = to_string( dR_cut[i] );
      ostringstream str_dR_cut;
      //// str_dR_cut.str("");
      str_dR_cut<<setprecision(3);
      str_dR_cut<<dR_cut[i];
      FillLeptonPlots( dimuon , str_sign+param.Name+"_dR<"+str_dR_cut.str() , weight );
      FillDileptonPlots( dimuon , str_sign+param.Name+"_dR<"+str_dR_cut.str() , weight );
      break;
    }
    */
 
    // --- mass cut --- //
    double Mass = (*dimuon[0] + *dimuon[1]).M();
    if( 0.76 < Mass && Mass < 0.80 ) { // M(rho, omega) = 0.78 pm 0.02 (2%)
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Rho_Omega", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Rho_Omega", weight);
    }
    else if( 1.0 < Mass && Mass < 1.04 ) { // M(phi) = 1.02 pm 0.02 (2%)
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Phi", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Phi", weight);
    }
    else if( 3. < Mass && Mass < 3.2 ) { // M(J/psi) = 3.1 pm 0.1 (3%)
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Jpsi", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Jpsi", weight);
    }
    else if( 3.63 < Mass && Mass < 3.75 ) { // M(Psi') = 3.69 pm 0.06 (2%)
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_PsiPrime", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_PsiPrime", weight);
    }
    else if( 9.2 < Mass && Mass < 10.5 ) { // M(Upsilon) = 9.5, 10.0, 10.4
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Upsilon", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Upsilon", weight);
    }
    else if( Mass < 50 ) {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M<50", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M<50", weight);
    }
    else {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M>50", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M>50", weight);
    }

    // --- Veto 3rd muon --- //
    vector<double> PtBox;
    PtBox.clear();
    for(unsigned iMu=0; iMu<muons.size(); iMu++) {
      PtBox.push_back( muons[iMu].Pt() );
    }
    for(unsigned iMu=0; iMu<Vmuons.size(); iMu++) {
      if( find(PtBox.begin(), PtBox.end(), Vmuons[iMu].Pt()) == PtBox.end() ) {
        PtBox.push_back( Vmuons[iMu].Pt() );
      }
    }
    //cout<<endl<<"==============================="<<endl;
    if( PtBox.size() == 2 ) {
      FillDileptonPlots( dimuon , str_sign+param.Name+"_Veto3rdMuon" , weight );
      // cout<<"size == 2"<<endl;
    }
    else {
      FillDileptonPlots( dimuon , str_sign+param.Name+"_Impose3rdMuon" , weight );
      // cout<<"size == "<<PtBox.size()<<"   --- NOTE ---"<<endl;
    }
    //for(unsigned jjj=0; jjj<PtBox.size(); jjj++) {
    //   cout<<PtBox[jjj]<<endl;
    //}


    // --- jet analysis --- //
    // Count # of jets
    JSFillHist( str_sign+param.Name, "Njets___"+str_sign+param.Name, jets.size(), weight, 10, 0., 10.);

    // Plotting 1 ~ 4 jet evt
    for(unsigned Njet = 0; Njet < 5; Njet++) {
      if( jets.size() == Njet ) {
        TString str_Njet = to_string( Njet );
        FillLeptonPlots( dimuon , str_sign+param.Name+"_"+str_Njet+"jet" , weight );
        FillDileptonPlots( dimuon , str_sign+param.Name+"_"+str_Njet+"jet" , weight );
        break;
      }
    }

    if( jets.empty() ) continue;
  
    // dR(mu,jet) 2D histogram
    double dR_MuJet = 999.;
    int min_dR_jet = 999;
    int min_dR_mu  = 999;
    for(unsigned i = 0; i < 2; i++) {
      for(unsigned j = 0; j < jets.size(); j++) {
        if( dimuon[i]->DeltaR( jets[j] ) < dR_MuJet ) {
          dR_MuJet = dimuon[i]->DeltaR( jets[j] );
          min_dR_jet = j;
          min_dR_mu  = i;
        }
      }
    }
  
    JSFillHist( str_sign+param.Name, "dR_MuJet___"+str_sign+param.Name, jets[min_dR_jet].DeltaR( *dimuon[ dimuon.size() - 1 - min_dR_mu ] ), jets[min_dR_jet].DeltaR( *dimuon[min_dR_mu] ), weight, 400, 0., 4., 400, 0., 4.);

  }
}

// ----- Event Selection ----- //
std::vector<Lepton*> NIsoMuon::NIsoMuonSelection(vector<Muon>& muons, vector<Muon>& Vmuons, int sgn) {
  std::vector<Lepton*> out;
  if( muons.size() < 1 || Vmuons.size() < 1 ) return out;
  for(unsigned i=0; i<muons.size(); i++) {
    if( muons[i].Pt() < Leading_Muon_Pt ) break;
    for(unsigned j=0; j<Vmuons.size(); j++) {
      if( muons[i].Pt() != Vmuons[j].Pt() && muons[i].Charge() * Vmuons[j].Charge() == sgn ) {
        out.push_back( &muons[i] );
        out.push_back( &Vmuons[j] );
        break;
      }
    }
    if( out.size() == 2 ) break;
  }
  return out;
}

double NIsoMuon::MCweight(Event ev) {
  double out = 1.;
  out *= weight_norm_1invpb * ev.GetTriggerLumi("Full"); // Is IsoMu24 unprescaled?
  out *= ev.MCweight();
  // out *= weight_Prefire;
  
  /*
  for(unsigned int i=0; i<muons.size(); i++) {
    double this_idsf = mcCorr->MuonID_SF(param.Muon_ID_SF_Key, muons.at(i).Eta(), muons.at(i).MiniAODPt());
    double this_isosf = 1.;
    if( param.Muon_Tight_ID == "POGTightWithTightIso" )
      this_isosf = mcCorr->MuonID_SF(param.Muon_ISO_SF_Key, muons.at(i).Eta(), muons.at(i).MiniAODPt());
    out *= this_idsf*this_isosf;
  }
  */
  return out;
}

NIsoMuon::NIsoMuon(){

}

NIsoMuon::~NIsoMuon(){

}

