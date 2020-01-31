#include "Dimuon.h"

void Dimuon::initializeAnalyzer(){
  MuonIDs = {
    // "POGTightWithTightIso",
    "POGTightWithTightTrkIso",
    "POGTight",
    // "POGHighPt",
    // "POGMedium",
    "POGLoose",
    "isGlobalMuon",
    "isTrackerMuon",
    "isPFMuon",
    "NoID"
  };

  MuonIDSFKey = "NUM_TightID_DEN_genTracks";
  MuonISOSFKey = "NUM_TightRelIso_DEN_TightIDandIPCut";

  JetID = "tight";

  IsoMuTriggerName = {
    //"HLT_IsoMu24_v","HLT_IsoTkMu24_v"
    /*
    "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
    "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
    "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
    "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"
    */
  };
  TriggerSafePtCut = 10.;
}

void Dimuon::executeEvent(){
  AllMuons = GetAllMuons();
  AllJets = GetAllJets();

  weight_Prefire = GetPrefireWeight(0);

  AnalyzerParameter param;

  for(unsigned int it_MuonID=0; it_MuonID<MuonIDs.size(); it_MuonID++) {
    TString MuonID = MuonIDs.at(it_MuonID);

    param.Clear();

    param.Name = MuonID;

    param.Muon_Tight_ID = MuonID;

    param.Muon_ID_SF_Key = MuonIDSFKey;
    param.Muon_ISO_SF_Key = MuonISOSFKey;

    param.Jet_ID = JetID;

    executeEventFromParameter(param);
  }
}

void Dimuon::executeEventFromParameter(AnalyzerParameter param){

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?

  Event ev = GetEvent();

  // if( !(ev.PassTrigger(IsoMuTriggerName) ) ) return;

  vector<Muon> muons = SelectMuons(AllMuons, param.Muon_Tight_ID, 10., 2.4);
  vector<Jet> jets = SelectJets(AllJets, param.Jet_ID, 30.,2.7);

  // Does ExampleRun.C code contain muon scaling/smearing & JEC?
  //std::sort(muons.begin(), muons.end(), PtComparing);
  //std::sort(jets.begin(), jets.end(), PtComparing);

  vector<Lepton*> dimuon;
  vector<int> sign = {-1, 1};
  for(int it_sign = 0; it_sign < 2; it_sign++) {
    TString str_sign;
    if( it_sign ) str_sign = "SS_";
    else str_sign = "OS_";

    // ----- Event Selection ----- //
    dimuon = DimuonSelection( muons, sign[it_sign] );
    if( dimuon.size() != 2 ) return;
  
    // where is the definition of Pt()? Do particles save in TLorentzVector format and take Pt() out of it?
    // No Trigger Matching?
  
    double weight = 1.;
    if(!IsDATA) weight = MCweight( ev );

    // - + - + - + - + - + - Plotting - + - + - + - + - //
    FillLeptonPlots( dimuon , str_sign+param.Name , weight );
    FillDileptonPlots( dimuon , str_sign+param.Name , weight );

    // --- deltaR cut --- //
    vector<double> dR_cut = {0.02,0.1,0.2,0.3,0.6,999.};
  
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
 
    // --- mass cut --- //
    double Mass = (*dimuon[0] + *dimuon[1]).M();
    if( 3. < Mass && Mass < 3.2 ) {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Jpsi", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Jpsi", weight);
    }
    else if( 9.2 < Mass && Mass < 10.5 ) {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Upsilon", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Upsilon", weight);
    }
    else {
      FillLeptonPlots( dimuon , str_sign+param.Name+"_M_Veto", weight);
      FillDileptonPlots( dimuon , str_sign+param.Name+"_M_Veto", weight);
    }


    /*
    // --- jet analysis --- //
    // n jet evt
    JSFillHist( str_sign+param.Name, "Njets"+str_sign+param.Name, jets.size(), weight, 10, 0., 10.);

    if( jets.empty() ) return;
  
    // 1 ~ 4 jet evt
    for(unsigned Njet = 1; Njet < 5; Njet++) {
      if( jets.size() == Njet ) {
        TString str_Njet = to_string( Njet );
        FillLeptonPlots( dimuon , str_sign+param.Name+"_"+str_Njet+"jet" , weight );
        FillDileptonPlots( dimuon , str_sign+param.Name+"_"+str_Njet+"jet" , weight );
        break;
      }
    }
  
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
  
    JSFillHist( str_sign+param.Name, "dR_MuJet"+str_sign+param.Name, jets[min_dR_jet].DeltaR( *dimuon[ dimuon.size() - 1 - min_dR_mu ] ), jets[min_dR_jet].DeltaR( *dimuon[min_dR_mu] ), weight, 400, 0., 4., 400, 0., 4.);
    */


  }
}

// ----- Event Selection ----- //
std::vector<Lepton*> Dimuon::DimuonSelection(vector<Muon>& muons, int sgn) {
  std::vector<Lepton*> out;
  if( muons.size() < 2 ) return out;
  for(unsigned i=0; i<muons.size()-1; i++) {
    if( muons[i].Pt() < TriggerSafePtCut ) break;
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

double Dimuon::MCweight(Event ev) {
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

Dimuon::Dimuon(){

}

Dimuon::~Dimuon(){

}

