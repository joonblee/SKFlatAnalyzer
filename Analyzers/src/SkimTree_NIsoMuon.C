#include "SkimTree_NIsoMuon.h"

void SkimTree_NIsoMuon::initializeAnalyzer(){

  outfile->cd();
  cout << "[SkimTree_NIsoMuon::initializeAnalyzer()] gDirectory = " << gDirectory->GetName() << endl;
  newtree = fChain->CloneTree(0);

  triggers.clear();
  if(DataYear==2016){
    triggers = {

      // Double Muon Trigger, pT > 10-20, feta < 2.4, Loose/Medium/Tight ID, Loose PF comb. rel. iso (dBeta)
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
      "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
      "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",

      // Single Muon Trigger, pT > 23, feta < 2.4, Loose/Medium/Tight ID, Loose PF comb. rel. iso (dBeta)
      "HLT_IsoMu24_v",
      "HLT_IsoTkMu24_v", //

      // Single Muon High Pt Trigger, HighPt ID
      "HLT_Mu50_v",
      "HLT_TkMu50_v" // pT > 52, feta < 2.4, HighPtID

      // Remarks in Muon HLT in 2016, "https://twiki.cern.ch/twiki/bin/view/CMS/MuonHLT2016"
      // Offline muons selected for analysis should always be associated to the HLT objects that triggered the event. Recommended matching criterion: ΔR(HLT object, offline muon) < 0.1. 
      // the offline muons are associated to the online objects that triggered the event, using the same matching criteria used in tag-and-probe, namely ΔR(L3, off. muon) < 0.1. 

    };
  }
  else if(DataYear==2017){
    triggers = {
        "HLT_IsoMu27_v",
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v"
    };
  }
  else if(DataYear==2018){
    triggers = {
        "HLT_IsoMu24_v",
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"
    };
  }
  else{
    cout << "[SkimTree_NIsoMuon::initializeAnalyzer] DataYear is wrong : " << DataYear << endl;
  }

  cout << "[SkimTree_NIsoMuon::initializeAnalyzer] triggers to skim = " << endl;
  for(unsigned int i=0; i<triggers.size(); i++){
    cout << "[SkimTree_NIsoMuon::initializeAnalyzer]   " << triggers.at(i) << endl;
  }


}

void SkimTree_NIsoMuon::executeEvent(){
  AllMuons = GetAllMuons();

  Event ev = GetEvent();

  // Event ev;
  // ev.SetTrigger(*HLT_TriggerName);

  if( AllMuons.size() < 2 ) return;

  if( ev.PassTrigger(triggers) ){

    for(unsigned int i=0; i<AllMuons.size(); i++) {
      if( !(AllMuons[i].Pt() > 20.) ) return;
      if( !(fabs(AllMuons[i].Eta()) < 2.4) ) continue;
      if( AllMuons[i].PassID("NonIsolatedLooseMuon") ) {
        for(unsigned int j=i+1; j<AllMuons.size(); j++) {
          if( !(AllMuons[j].Pt() > 10. && fabs(AllMuons[j].Eta()) < 2.4) ) continue;
          if( AllMuons[j].PassID("NonIsolatedLooseMuon") ) {
            newtree->Fill();
            return;
          }
          else if( AllMuons[j].PassID("POGTightWithTightIso") ) {
            newtree->Fill();
            return;
          }
        }
      }
      else if( AllMuons[i].PassID("POGTightWithTightIso") ) {
        for(unsigned int j=i+1; j<AllMuons.size(); j++) {
          if( !(AllMuons[j].Pt() > 10. && fabs(AllMuons[j].Eta()) < 2.4) ) continue;
          if( AllMuons[j].PassID("NonIsolatedLooseMuon") ) {
            newtree->Fill();
            return;
          }
        }
      }
    }

  }

}

void SkimTree_NIsoMuon::executeEventFromParameter(AnalyzerParameter param){

}

SkimTree_NIsoMuon::SkimTree_NIsoMuon(){

  newtree = NULL;

}

SkimTree_NIsoMuon::~SkimTree_NIsoMuon(){

}

void SkimTree_NIsoMuon::WriteHist(){

  outfile->mkdir("recoTree");
  outfile->cd("recoTree");
  newtree->Write();
  outfile->cd();

}


