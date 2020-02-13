#include "NIsoMuon.h"

void NIsoMuon::initializeAnalyzer(){
  MuonID1s = {
    //"POGTightWithTightTrkIso",
    //"POGTight",
    // "POGHighPt",
    // "POGMedium",
    // "POGLoose",
    //"POGTightWithVetoTightTrkIso",
    //"POGTightWithVetoLooseTrkIso"
    "NonIsolatedMuon"
  };

  MuonIDSFKey = "NUM_TightID_DEN_genTracks";
  MuonISOSFKey = "NUM_TightRelIso_DEN_TightIDandIPCut";

  JetID = "tight";

  MuonID2s = {
    //"POGTightWithTightTrkIso",
    //"POGTight",
    //"POGTightWithVetoTightTrkIso",
    //"POGTightWithVetoLooseTrkIso"
    "NonIsolatedMuon"
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

}

void NIsoMuon::executeEvent(){
  AllMuons = GetAllMuons();
  AllJets = GetAllJets();

  weight_Prefire = GetPrefireWeight(0);

  // cout<<run<<endl;

  AnalyzerParameter param;

  for(unsigned int it_MuonID1=0; it_MuonID1<MuonID1s.size(); it_MuonID1++) {
    TString MuonID1 = MuonID1s.at(it_MuonID1);

    param.Clear();

    param.Muon_Tight_ID = MuonID1;

    param.Muon_ID_SF_Key = MuonIDSFKey;
    param.Muon_ISO_SF_Key = MuonISOSFKey;

    param.Jet_ID = JetID;

    param.Name = MuonID1;

    executeEventFromParameter(param);

  }
}

void NIsoMuon::executeEventFromParameter(AnalyzerParameter param){

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?

  Event ev = GetEvent();

  if( !(ev.PassTrigger(IsoMuTriggerName) ) ) return;


  vector<Muon> muons = SelectMuons(AllMuons, param.Muon_Tight_ID, Subleading_Muon_Pt, 2.4);
  vector<Jet> jets = SelectJets(AllJets, param.Jet_ID, 30.,2.7);

  // Does ExampleRun.C code contain muon scaling/smearing & JEC?
  //std::sort(muons.begin(), muons.end(), PtComparing);
  //std::sort(jets.begin(), jets.end(), PtComparing);

  vector<Lepton*> dimuon;

  double weight = 1.;


  vector<int> sign = {-1, 1};
  for(int it_sign = 0; it_sign < 2; it_sign++) {
    TString str_sign;
    if( it_sign ) str_sign = "SS_"; else str_sign = "OS_";

    // ----- Event Selection ----- //
    dimuon.clear();
    dimuon = NIsoMuonSelection( muons, sign[it_sign] );
    if( dimuon.size() != 2 ) return;
  

    // - + - + - + - + - + - Plotting - + - + - + - + - //
    if(!IsDATA) weight = MC_Weight( ev , param , dimuon );

    FillLeptonPlots( dimuon , str_sign+param.Name , weight );
    FillDileptonPlots( dimuon , str_sign+param.Name , weight );

 
    // --- mass cut --- //
    double Mass = (*dimuon[0] + *dimuon[1]).M();
    /*
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
    //else if( 80 < Mass && Mass < 100 ) { // M(Z) = 80, 100
    //  FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Z", weight);
    //  FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Z", weight);
    //}
    else if( Mass < 50 ) {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M<50", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M<50", weight);
    }
    else {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M>50", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M>50", weight);
    }
    */

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
  
    JSFillHist( str_sign+param.Name, "DileptonJet_DeltaR___"+str_sign+param.Name, jets[min_dR_jet].DeltaR( *dimuon[ dimuon.size() - 1 - min_dR_mu ] ), jets[min_dR_jet].DeltaR( *dimuon[min_dR_mu] ), weight, 400, 0., 4., 400, 0., 4.);


    // dR(mu,j) < 0.3
    bool isPass=false;
    dimuon.clear();
    vector<Jet> this_jet; this_jet.clear();
    vector<FatJet> this_fatjet;

    for(unsigned iJet=0; iJet<jets.size(); iJet++) {
      for(unsigned iMu=0; iMu<muons.size(); iMu++) {
        if( muons[iMu].Pt() < Leading_Muon_Pt ) {
          isPass=true;
          break;
        }
        if( jets[iJet].DeltaR(muons[iMu]) > 0.4 ) continue;
        for(unsigned jMu=iMu+1; jMu<muons.size(); jMu++) {
          if( jets[iJet].DeltaR(muons[jMu]) > 0.4 ) continue;
          if( muons[iMu].Charge() * muons[jMu].Charge() == sign[it_sign] ) {
            isPass=true;

            dimuon.push_back( &muons[iMu] );
            dimuon.push_back( &muons[jMu] );
            this_jet.push_back( jets[iJet] );

            if(!IsDATA) weight = MC_Weight( ev , param , dimuon );

            FillLeptonPlots( dimuon, str_sign+param.Name+"_dR(j,m)04", weight );
            FillJetPlots(this_jet, this_fatjet, str_sign+param.Name+"_dR(j,m)04", weight);
            FillDileptonPlots( dimuon , str_sign+param.Name+"_dR(j,m)04" , weight );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "Dilepton_Mass___"+str_sign+param.Name+"_dR(j,m)04", ( muons[iMu] + muons[jMu] ).M(), weight, 10000,0.,10000. );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "Dilepton_LowMass___"+str_sign+param.Name+"_dR(j,m)04", ( muons[iMu] + muons[jMu] ).M(), weight, 6000,0.,12. );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "Jet_Mass___"+str_sign+param.Name+"_dR(j,m)04", jets[iJet].M(), weight, 10000,0.,10000. );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "Jet_LowMass___"+str_sign+param.Name+"_dR(j,m)04", jets[iJet].M(), weight, 6000,0.,12. );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "DileptonJet_Mass___"+str_sign+param.Name+"_dR(j,m)04", ( muons[iMu] + muons[jMu] + jets[iJet] ).M(), weight, 10000,0.,10000. );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "DileptonJet_LowMass___"+str_sign+param.Name+"_dR(j,m)04", ( muons[iMu] + muons[jMu] + jets[iJet] ).M(), weight, 6000,0.,12. );
            JSFillHist( str_sign+param.Name+"_dR(j,m)04", "Jet_0_PtminusSumMuPt___"+str_sign+param.Name+"_dR(j,m)04", jets[iJet].Pt()-muons[iMu].Pt()-muons[jMu].Pt(), weight, 2100, -100., 2000.);

            Mass = (muons[iMu]+muons[jMu]).M();
            if( 80 < Mass && Mass < 100 ) {
              FillLeptonPlots( dimuon, str_sign+param.Name+"_dR(j,m)04_Zcut", weight );
              FillDileptonPlots( dimuon , str_sign+param.Name+"_dR(j,m)04_Zcut" , weight );
              FillJetPlots(this_jet, this_fatjet, str_sign+param.Name+"_dR(j,m)04_Zcut", weight);
              JSFillHist( str_sign+param.Name+"_dR(j,m)04_Zcut", "Jet_0_PtminusSumMuPt___"+str_sign+param.Name+"_dR(j,m)04_Zcut", jets[iJet].Pt()-muons[iMu].Pt()-muons[jMu].Pt(), weight, 2100, -100., 2000.);
              JSFillHist( str_sign+param.Name+"_dR(j,m)04_Zcut", "DileptonJet_DeltaR___"+str_sign+param.Name+"_dR(j,m)04_Zcut", jets[min_dR_jet].DeltaR( *dimuon[ dimuon.size() - 1 - min_dR_mu ] ), jets[min_dR_jet].DeltaR( *dimuon[min_dR_mu] ), weight, 800, 0., .8, 800, 0., .8);

               cout<<endl;
               cout<<" + + + - - - - - Z candidate in a jet - - - - - + + + "<<endl;
               cout<<" run # = "<<run<<"  |  lumi # = "<<lumi<<"  |  event # = "<<event<<endl;
               cout<<" + ------------------------------------------------ + "<<endl;
               cout<<endl;
            }
            break;
          }
        }
         if( isPass ) break;
      }
      if( isPass ) break;
    }

  }
}

// ----- Event Selection ----- //
std::vector<Lepton*> NIsoMuon::NIsoMuonSelection(vector<Muon>& muons, int sgn) {
  std::vector<Lepton*> out;
  if( muons.size() < 2 ) return out;
  for(unsigned i=0; i<muons.size(); i++) {
    if( muons[i].Pt() < Leading_Muon_Pt ) break;;
    for(unsigned j=i+1; j<muons.size(); j++) {
      if( muons[i].Charge() * muons[j].Charge() == sgn ) {
        out.push_back( &muons[i] );
        out.push_back( &muons[j] );
        break;
      }
    }
    if( out.size() == 2 ) break;
  }
  return out;
}

double NIsoMuon::MC_Weight(Event ev, AnalyzerParameter param, vector<Lepton*> muons) {
  double out = 1.;
  out *= weight_norm_1invpb * ev.GetTriggerLumi("Full"); // Is IsoMu24 unprescaled?
  out *= ev.MCweight();
  out *= weight_Prefire;
  
  for(unsigned int i=0; i<muons.size(); i++) {
    Muon *mu = (Muon*)muons[i];

    double this_idsf = mcCorr->MuonID_SF(param.Muon_ID_SF_Key, mu->Eta(), mu->MiniAODPt());
    double this_isosf = 1.;
    //if( param.Muon_Tight_ID == "POGTightWithTightIso" )
    //  this_isosf = mcCorr->MuonID_SF(param.Muon_ISO_SF_Key, muons.at(i).Eta(), muons.at(i).MiniAODPt());
    out *= this_idsf*this_isosf;
  }
  return out;
}

NIsoMuon::NIsoMuon(){

}

NIsoMuon::~NIsoMuon(){

}

