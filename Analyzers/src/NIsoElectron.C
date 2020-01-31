#include "NIsoElectron.h"

void NIsoElectron::initializeAnalyzer(){
  // Measurements of differential Z boson production cross sections in pp collisions at sqrt(s) = 13 TeV
  // AN2017_282_v7
  // AN2017_203_v9
  ElectronIDs = {
    "2016CutBasedMedium",
    //"2016CutBasedLooseNoIso",
    "2016CutBasedMediumNoIso",
    //"2016CutBasedTightNoIso"
    "2016CutBasedMediumVetoIso"
  }; // Cut-based Medium ID

  NIsoElectronIDs = {
    "2016CutBasedMedium",
    "2016CutBasedMediumNoIso",
    "2016CutBasedMediumVetoIso"
  }; 

  IsoEleTriggerName = {};
  TriggerSafePtCut = 999.;
  TriggerSecondPtCut = 999.;

  JetID = "tight";

  //cout<<endl<<TriggerInput<<endl<<endl;

  if( TriggerInput == "DoubleElectron_IsoVL" ) {
    IsoEleTriggerName = {
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"
    };
    TriggerSafePtCut = 25.;
    TriggerSecondPtCut = 15.;
  }
  else if( TriggerInput == "DoubleElectron_NoIso" ) {
    IsoEleTriggerName = {
      "HLT_DoubleEle33_CaloIdL_MW_v"
    };
    TriggerSafePtCut = 35.;
    TriggerSecondPtCut = 35.;
  }


}

void NIsoElectron::executeEvent(){
  AllElectrons = GetAllElectrons();
  AllJets = GetAllJets();

  weight_Prefire = GetPrefireWeight(0);

  AnalyzerParameter param;

  for(unsigned int it_ElectronID=0; it_ElectronID<ElectronIDs.size(); it_ElectronID++) {
    TString ElectronID = ElectronIDs.at(it_ElectronID);

    param.Clear();

    param.Electron_Tight_ID = ElectronID;

    param.Jet_ID = JetID;

    for(unsigned int it_NIsoElectronID=0; it_NIsoElectronID<NIsoElectronIDs.size(); it_NIsoElectronID++) {
      TString NIsoElectronID = NIsoElectronIDs.at(it_NIsoElectronID);

      param.Electron_Veto_ID = NIsoElectronID;

      param.Name = ElectronID+"_"+NIsoElectronID;

      executeEventFromParameter(param);
    }

  }
}

void NIsoElectron::executeEventFromParameter(AnalyzerParameter param) {

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?

  Event ev = GetEvent();

  if( TriggerInput == "DoubleElectron_NoIso" ) {
    if( !( run==276453 || run==278822 ) ) {
      IsoEleTriggerName = {
        "HLT_DoubleEle33_CaloIdL_MW_v"
      };
    }
    else {
      IsoEleTriggerName = {
        "HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v"
      };
    }
  }


  if( !(ev.PassTrigger(IsoEleTriggerName) ) ) return;

  vector<Electron> electrons = SelectElectrons(AllElectrons, param.Electron_Tight_ID, TriggerSecondPtCut, 2.5);
  vector<Electron> Velectrons = SelectElectrons(AllElectrons, param.Electron_Veto_ID, TriggerSecondPtCut, 2.5);
  vector<Jet> jets = SelectJets(AllJets, param.Jet_ID, 30.,2.7);

  vector<Lepton*> dielectron;


  double weight = 1.;
  if(!IsDATA) weight = MCweight( ev );

  vector<int> sign = {-1, 1};
  for(int it_sign = 0; it_sign < 2; it_sign++) {
    TString str_sign;
    if( it_sign ) str_sign = "SS_"; else str_sign = "OS_";

    // ----- Event Selection ----- //
    dielectron = NIsoElectronSelection( electrons, Velectrons, sign[it_sign] );
    if( dielectron.size() != 2 ) return;


    // - + - + - + - + - + - Plotting - + - + - + - + - //
    FillLeptonPlots( dielectron , str_sign+param.Name , weight );
    FillDileptonPlots( dielectron , str_sign+param.Name , weight );

    // --- jet analysis --- //
    // Count # of jets
    JSFillHist( str_sign+param.Name, "Njets___"+str_sign+param.Name, jets.size(), weight, 10, 0., 10.);

    if( jets.empty() ) return;

    // Plotting 1 ~ 4 jet evt
    for(unsigned Njet = 1; Njet < 5; Njet++) {
      if( jets.size() == Njet ) {
        TString str_Njet = to_string( Njet );
        FillLeptonPlots( dielectron , str_sign+param.Name+"_"+str_Njet+"jet" , weight );
        FillDileptonPlots( dielectron , str_sign+param.Name+"_"+str_Njet+"jet" , weight );
        break;
      }
    }

    // dR(el,jet) 2D histogram
    double dR_EleJet = 999.;
    int min_dR_jet = 999;
    int min_dR_el  = 999;
    for(unsigned i = 0; i < 2; i++) {
      for(unsigned j = 0; j < jets.size(); j++) {
        if( dielectron[i]->DeltaR( jets[j] ) < dR_EleJet ) {
          dR_EleJet = dielectron[i]->DeltaR( jets[j] );
          min_dR_jet = j;
          min_dR_el  = i;
        }
      }
    }

    JSFillHist( str_sign+param.Name, "dR_EleJet"+str_sign+param.Name, jets[min_dR_jet].DeltaR( *dielectron[ dielectron.size() - 1 - min_dR_el ] ), jets[min_dR_jet].DeltaR( *dielectron[min_dR_el] ), weight, 400, 0., 4., 400, 0., 4.);

  }


}

// ----- Event Selection ----- //
std::vector<Lepton*> NIsoElectron::NIsoElectronSelection(vector<Electron>& electrons, vector<Electron>& Velectrons, int sgn) {
  std::vector<Lepton*> out;
  if( electrons.size() < 1 || Velectrons.size() < 1 ) return out;
  for(unsigned i=0; i<electrons.size(); i++) {
    if( electrons[i].Pt() < TriggerSafePtCut ) break;
    for(unsigned j=0; j<Velectrons.size(); j++) {
      if( electrons[i].Pt() != Velectrons[j].Pt() && electrons[i].Charge() * Velectrons[j].Charge() == sgn ) {
        out.push_back( &electrons[i] );
        out.push_back( &Velectrons[j] );
        break;
      }
    }
    if( out.size() == 2 ) break;
  }
  return out;
}


double NIsoElectron::MCweight(Event ev) {
  double out = 1.;
  out *= weight_norm_1invpb * ev.GetTriggerLumi("Full");
  out *= ev.MCweight();
  // out *= weight_Prefire;
  
  /*
  for(unsigned int i=0; i<electrons.size(); i++) {
    double this_idsf = mcCorr->ElectronID_SF(param.Electron_ID_SF_Key, electrons.at(i).Eta(), electrons.at(i).MiniAODPt());
    double this_isosf = 1.;
    if( param.Electron_Tight_ID == "POGTightWithTightIso" )
      this_isosf = mcCorr->ElectronID_SF(param.Electron_ISO_SF_Key, electrons.at(i).Eta(), electrons.at(i).MiniAODPt());
    out *= this_idsf*this_isosf;
  }
  */
  return out;
}

NIsoElectron::NIsoElectron(){

}

NIsoElectron::~NIsoElectron(){

}

