#include "SkimTree_NIsoMuon.h"

void SkimTree_NIsoMuon::initializeAnalyzer(){

  outfile->cd();
  cout << "[SkimTree_NIsoMuon::initializeAnalyzer()] gDirectory = " << gDirectory->GetName() << endl;
  newtree = fChain->CloneTree(0);

  triggers.clear();
  if(DataYear==2016){
    triggers = {

      // Double Muon Trigger, pT > 10-20, feta < 2.4, Loose/Medium/Tight ID, Loose PF comb. rel. iso (dBeta)
      //"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
      //"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
      //"HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
      //"HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
      //"HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
      //"HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",

      // Single Muon Trigger, pT > 23, feta < 2.4, Loose/Medium/Tight ID, Loose PF comb. rel. iso (dBeta)
      //"HLT_IsoMu24_v",
      //"HLT_IsoTkMu24_v", //

      // Single Muon High Pt Trigger, HighPt ID
      "HLT_Mu50_v",
      "HLT_TkMu50_v", // pT > 52, feta < 2.4, HighPtID

      // Remarks in Muon HLT in 2016, "https://twiki.cern.ch/twiki/bin/view/CMS/MuonHLT2016"
      // Offline muons selected for analysis should always be associated to the HLT objects that triggered the event. Recommended matching criterion: ΔR(HLT object, offline muon) < 0.1. 
      // the offline muons are associated to the online objects that triggered the event, using the same matching criteria used in tag-and-probe, namely ΔR(L3, off. muon) < 0.1. 
      //"HLT_Mu17_Mu8_SameSign_DZ_v",
      "HLT_Mu30_TkMu11_v",

      // name act_lumi eff_lumi
      // MET
      "HLT_MET200_v",	//36.47	36.47
      "HLT_MET250_v",	//36.47	36.47
      "HLT_MET300_v",	//36.47	36.47
      "HLT_MET600_v",	//36.47	36.47
      "HLT_MET700_v",	//36.47	36.47
      
      "HLT_PFMET170_BeamHaloCleaned_v",	//33.64	6.91
      "HLT_PFMET170_HBHECleaned_v",	//36.47	36.47	
      "HLT_PFMET170_HBHE_BeamHaloCleaned_v",	//23.09	23.09	
      "HLT_PFMET170_JetIdCleaned_v",	//8.19	8.19
      "HLT_PFMET170_NoiseCleaned_v",	//8.19	8.19
      "HLT_PFMET170_NotCleaned_v",	//36.47	9.74
      "HLT_PFMET300_v",	//36.47	36.47	
      "HLT_PFMET400_v",	//36.47	36.47	
      "HLT_PFMET500_v",	//36.47	36.47	
      "HLT_PFMET600_v",	//36.47	36.47	

      // Jet
      "HLT_PFJet900_v",
      "HLT_PFJet500_v",
      "HLT_PFJet450_v",
      "HLT_PFJet400_v",
      "HLT_PFJet320_v",
      "HLT_PFJet260_v",
      "HLT_PFJet200_v",
      "HLT_PFJet140_v",
      "HLT_PFJet80_v",
      "HLT_PFJet60_v",
      "HLT_PFJet40_v"
    };
  }
  else if(DataYear==2017){
    triggers = {
        //"HLT_IsoMu27_v",
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v", // L = 41.5 /fb
        //"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
        //"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v",
        "HLT_Mu37_TkMu27_v", // L = 27.1 /fb
        //"DST_DoubleMu3_noVtx_CaloScouting_v" // ScoutingCaloMuon L = 35.4 /fb
    };
  }
  else if(DataYear==2018){
    triggers = {
        //"HLT_IsoMu24_v",
        "HLT_Mu50_v", "HLT_OldMu100_v", "HLT_TkMu100_v",
        //"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
        "HLT_Mu37_TkMu27_v",
        //"HLT_Mu20_Mu10_SameSign_DZ_v",
        //"HLT_Mu20_Mu10_SameSign_v",
        //"HLT_Mu23_Mu12_SameSign_v",
        //"HLT_Mu23_Mu12_SameSign_DZ_v",
        //"DST_DoubleMu3_noVtx_CaloScouting_v", // ScoutingCaloMuon L = 59.8 /fb
        //"DST_DoubleMu1_noVtx_CaloScouting_v", // ScoutingCaloMuon
        //"DST_DoubleMu3_noVtx_Mass10_PFScouting_v", // ScoutingPFMuon
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
  AllJets = GetAllJets();

  Event ev = GetEvent();

  /*
  if( AllMuons.size() < 2 ) return;

  //if( !ev.PassTrigger(triggers) ) return;

  for(unsigned int i=0; i<AllMuons.size(); i++) {
    if( !(AllMuons[i].Pt() > 20.) ) return;
    if( !(fabs(AllMuons[i].Eta()) < 2.4) ) continue;
    if( !(AllMuons[i].PassID("POGLoose")) ) continue;

    //if( AllMuons[i].PassID("POGTightWithTightIso") ) {
    if( AllMuons[i].RelIso()<0.4 || AllMuons[i].TrkIso()/AllMuons[i].TuneP4().Pt()<0.1 ) {
      for(unsigned int j=i+1; j<AllMuons.size(); j++) {
        if( !(AllMuons[j].Pt() > 10. && fabs(AllMuons[j].Eta()) < 2.4 && AllMuons[j].PassID("POGLoose")) ) continue;
        if( AllMuons[j].RelIso()>0.05 || AllMuons[j].TrkIso()/AllMuons[j].TuneP4().Pt()>0.05 ) {
          newtree->Fill();
          return;
        }
      }
    }
    //else if( AllMuons[i].PassID("NonIsolatedLooseMuon") ) {
    else if( AllMuons[i].RelIso()>0.05 || AllMuons[i].TrkIso()/AllMuons[i].TuneP4().Pt()>0.05 ) {
      for(unsigned int j=i+1; j<AllMuons.size(); j++) {
        if( !(AllMuons[j].Pt() > 10. && fabs(AllMuons[j].Eta()) < 2.4 && AllMuons[j].PassID("POGLoose")) ) continue;
        if( AllMuons[j].RelIso()<0.4 || AllMuons[j].TrkIso()/AllMuons[j].TuneP4().Pt()<0.1 ) {
          newtree->Fill();
          return;
        }
        else if( AllMuons[j].RelIso()>0.05 || AllMuons[j].TrkIso()/AllMuons[j].TuneP4().Pt()>0.05 ) {
          newtree->Fill();
          return;
        }
      }
    }
  }
  */

  if( AllMuons.size() < 2 ) return;
  if( !ev.PassTrigger(triggers) ) return;

  std::vector<Muon> muons;
  for(auto muon:AllMuons) {
    if( !(muon.Pt() > 9.) ) break;
    if( !(fabs(muon.Eta()) < 2.5) ) continue;
    muons.push_back(muon);
  }
  if( muons.size() < 2 ) return;

  for(auto jet:AllJets) {
    if( !(jet.Pt() > 20. ) ) break;
    if( !(fabs(jet.Eta()) < 2.6) ) continue;
    for(auto muon:muons) {
      if( !(jet.DeltaR(muon) < 0.5) ) continue;
      newtree->Fill();
      return;
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


