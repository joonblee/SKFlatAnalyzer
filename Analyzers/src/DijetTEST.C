#include "DijetTEST.h"


void DijetTEST::initializeAnalyzer(){

  JetID = "tight";

}

void DijetTEST::executeEvent(){
  AllJets = GetAllJets();

  AnalyzerParameter param;

  param.Jet_ID = JetID;

  param.Name = JetID+"_POGrecommendedMuonTriggers";

  executeEventFromParameter(param);

}

void DijetTEST::executeEventFromParameter(AnalyzerParameter param){

  // if(!PassMETFilter()) return;

  Event ev = GetEvent();

  vector<Jet> jets = SelectJets(AllJets, param.Jet_ID, 30., 2.7);

  vector<Jet> dijet;
  vector<FatJet> fatjets;

  double weight = 1.;
  if(!IsDATA) weight = MCweight( ev );

  vector<TString> sign = {"OS", "SS", "Neutrals", "Inclusive"};

  bool isPASS;

  for(unsigned iSign=0; iSign<sign.size(); iSign++) {
    // 1. simple selection
    dijet.clear();
    isPASS = false;

    for(unsigned iJet=0; iJet<jets.size(); iJet++) {
      for(unsigned jJet=iJet+1; jJet<jets.size(); jJet++) {

        if( sign[iSign] == "OS" ) { if( jets[iJet].Charge() * jets[jJet].Charge() == -1 ) isPASS = true;}
        else if( sign[iSign] == "SS" ) { if( jets[iJet].Charge() * jets[jJet].Charge() == 1 ) isPASS = true;}
        else if( sign[iSign] == "Neutrals" ) { if( jets[iJet].Charge() == 0 && jets[jJet].Charge() == 0 ) isPASS = true;}
        else
          isPASS = true;

        if( isPASS ) {

          dijet.push_back( jets[iJet] );
          dijet.push_back( jets[jJet] );
          FillDijetPlots( dijet , fatjets , sign[iSign] + "___Simple_Selection" , weight );
  
          // 2. veto third selection
          if( jets.size() == 2 ) 
            FillDijetPlots( dijet , fatjets , sign[iSign] + "___Veto_Third_Jet" , weight );
          break;

        }

      }
      if( dijet.size() == 2 ) break;
    }


  
    // 3. illinois selection : https://indico.cern.ch/event/568360/contributions/2446908/attachments/1440404/2217258/CMS-dijet-searches_v4.pdf
    dijet.clear();
    isPASS = false;

    Jet subjet;
    for(unsigned iJet=0; iJet<jets.size(); iJet++) {
      for(unsigned jJet=iJet+1; jJet<jets.size(); jJet++) {

        if( abs( jets[iJet].Eta() - jets[jJet].Eta() ) > 1.3 ) continue; // suppress t-channel QCD background

        if( sign[iSign] == "OS" ) { if( jets[iJet].Charge() * jets[jJet].Charge() == -1 ) isPASS = true;}
        else if( sign[iSign] == "SS" ) { if( jets[iJet].Charge() * jets[jJet].Charge() == 1 ) isPASS = true;}
        else if( sign[iSign] == "Neutrals" ) { if( jets[iJet].Charge() == 0 && jets[jJet].Charge() == 0 ) isPASS = true;}
        else
          isPASS = true;

        if( isPASS ) {

          dijet.push_back( jets[iJet] );
          dijet.push_back( jets[jJet] );
          FillDijetPlots( dijet , fatjets , sign[iSign] + "___Illinois_Selection" , weight );
          subjet = jets[iJet];
          subjet += jets[jJet];
  
          for(unsigned kJet=jJet+1; kJet<jets.size(); kJet++) {
            if( min(dijet[0].DeltaR( jets[kJet] ), dijet[1].DeltaR( jets[kJet] )) < 1.1 ) {
              subjet += jets[kJet];
  
              JSFillHist(sign[iSign] + "___Illinois_Selection", "Multijet_Mass___"+sign[iSign]+"___Illinois_Selection", subjet.M(), weight, 40000,0.,4000.);
              JSFillHist(sign[iSign] + "___Illinois_Selection", "Multijet_LowMass___"+sign[iSign]+"___Illinois_Selection", subjet.M(), weight, 2000,0.,20.);
            }
          }
  
          break;
        }

      }
      if( dijet.size() == 2 ) break;
    }
  }

}

double DijetTEST::MCweight(Event ev) {
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


DijetTEST::DijetTEST(){

}

DijetTEST::~DijetTEST(){

}

