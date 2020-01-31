#include "Dielectron.h"

void Dielectron::initializeAnalyzer(){
  // Measurements of differential Z boson production cross sections in pp collisions at sqrt(s) = 13 TeV
  // AN2017_282_v7
  // AN2017_203_v9
  ElectronIDs = {
    "2016CutBasedLooseNoIso",
    "2016CutBasedMediumNoIso",
    "2016CutBasedTightNoIso"
  }; // Cut-based Medium ID

  IsoEleTriggerName = {
    "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"
  }; // double electron (loosely isolated - PF iso < 0.15 * electron pT) 
  TriggerSafePtCut = 25.;

}

void Dielectron::executeEvent(){
  AllElectrons = GetAllElectrons();

  weight_Prefire = GetPrefireWeight(0);

  AnalyzerParameter param;

  for(unsigned int it_ElectronID=0; it_ElectronID<ElectronIDs.size(); it_ElectronID++) {
    TString ElectronID = ElectronIDs.at(it_ElectronID);

    param.Clear();

    param.Name = ElectronID;

    param.Electron_Tight_ID = ElectronID;

    executeEventFromParameter(param);
  }
}

void Dielectron::executeEventFromParameter(AnalyzerParameter param) {

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?

  Event ev = GetEvent();

  if( !(ev.PassTrigger(IsoEleTriggerName) ) ) return;

  vector<Electron> electrons = SelectElectrons(AllElectrons, param.Electron_Tight_ID, 15., 2.5);

  vector<Lepton*> dielectron;


  // ----- Event Selection ----- //
  dielectron = DielectronSelection( electrons );
  if( dielectron.size() != 2 ) return;
  
  
  double weight = 1.;
  if(!IsDATA) weight = MCweight( ev );

  // - + - + - + - + - + - Plotting - + - + - + - + - //
  FillLeptonPlots( dielectron , param.Name , weight );
  FillDileptonPlots( dielectron , param.Name , weight );

 
  // --- mass cut --- //
  double Mass = (*dielectron[0] + *dielectron[1]).M();
  if( 3. < Mass && Mass < 3.2 ) {
    FillLeptonPlots( dielectron , param.Name+"_M_Jpsi", weight);
    FillDileptonPlots( dielectron , param.Name+"_M_Jpsi", weight);
  }
  else if( 9.2 < Mass && Mass < 10.5 ) {
    FillLeptonPlots( dielectron , param.Name+"_M_Upsilon", weight);
    FillDileptonPlots( dielectron , param.Name+"_M_Upsilon", weight);
  }
  else {
    FillLeptonPlots( dielectron , param.Name+"_M_Veto", weight);
    FillDileptonPlots( dielectron , param.Name+"_M_Veto", weight);
  }

}

// ----- Event Selection ----- //
std::vector<Lepton*> Dielectron::DielectronSelection(vector<Electron>& electrons) {
  std::vector<Lepton*> out;
  if( electrons.size() < 2 ) return out;
  for(unsigned i=0; i<electrons.size()-1; i++) {
    if( electrons[i].Pt() < TriggerSafePtCut ) break;
    for(unsigned j=i+1; j<electrons.size(); j++) {
      if( electrons[i].Charge() * electrons[j].Charge() == -1 ) { 
        out.push_back( &electrons[i] );
        out.push_back( &electrons[j] );
        break;
      }
    }   
    if( out.size() == 2 ) break;
  } 
  return out;  
}

double Dielectron::MCweight(Event ev) {
  double out = 1.;
  out *= weight_norm_1invpb * ev.GetTriggerLumi("Full"); // Is IsoMu24 unprescaled?
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

Dielectron::Dielectron(){

}

Dielectron::~Dielectron(){

}

