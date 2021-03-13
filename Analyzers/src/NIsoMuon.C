#include "NIsoMuon.h"

void NIsoMuon::initializeAnalyzer(){
  //////////////////
  // --- Muon --- //
  //////////////////
  MuonID1s = {
    //"POGLoose",
    /*
    "POGTight",
    "NonIsolatedMuon_NoisGlobalMuon",
    "NonIsolatedMuon_NoisPFMuon",
    "NonIsolatedMuon_NoNormalizedChi2",
    "NonIsolatedMuon_NoValidMuonHits",
    "NonIsolatedMuon_NoMatchedStations",
    "NonIsolatedMuon_NodXY",
    "NonIsolatedMuon_NodZ",
    "NonIsolatedMuon_NoValidPixelHits",
    "NonIsolatedMuon_NoTrackerLayers",
    */
    "NonIsolatedMuon"
    //"NonIsolatedLooseMuon"
    //"NonIsolatedMuonNoIP3D"
    //"POGTightWithTightTrkIso"
  };

  MuonIDSFKey = "NUM_TightID_DEN_genTracks";
  MuonISOSFKey = "NUM_TightRelIso_DEN_TightIDandIPCut";

  DileptonSigns = {"OS","SS","PP","MM"};
  //DileptonSigns = {"OS","SS"};
  //DileptonSigns = {"OS"};

  /////////////////
  // --- Jet --- //
  /////////////////
  JetIDs = {"tight"}; // "tightLepVeto"

  BTagger     = Jet::DeepCSV;
  //BTaggingWPs = {Jet::Tight, Jet::Tight, Jet::Loose, Jet::Medium, Jet::Medium};
  //BTagNames   = {"",         "BJet",     "VetoBJet", "BJetMedium","VetoBJetMedium"};
  BTaggingWPs = {Jet::Medium, Jet::Medium, Jet::Loose, Jet::Loose};
  BTagNames   = {"",          "BJet",      "VetoBJet", "TwoVetoBJets"};
  //BTaggingWPs = {Jet::Tight};
  //BTagNames   = {""        };


  bool period_dependent = false;
  if(IsDATA && DataYear == 2017)
    period_dependent = true;
  SetupBTagger({BTagger},BTaggingWPs,true,period_dependent);

  /////////////////
  // --- MET --- //
  /////////////////
  //METCuts     = {0, 40};
  //METCutNames = {"", "HighMET"};
  METCuts     = {0};
  METCutNames = {""};

  /////////////////////
  // --- Trigger --- //
  /////////////////////
  TriggerName = {};
  Leading_Muon_Pt = 999.;
  Subleading_Muon_Pt = 10.; // *** CHANGE SUBLEADING MUON PT *** //

  TriggerSetup(TriggerName, Leading_Muon_Pt, Subleading_Muon_Pt);
  //Leading_Muon_Pt = 40.;
  //Subleading_Muon_Pt = 30.;
}

void NIsoMuon::executeEvent(){
  AllMuons = JBGetAllMuons();
  AllJets = GetAllJets();
  AllFatJets = GetAllFatJets();

  weight_Prefire = GetPrefireWeight(0);

  //cout<<endl;
  //cout<<"run: "<<run<<", event: "<<event<<endl;

  AnalyzerParameter param;
  JBAnalyzerParameter JBparam;

  for(unsigned int it_MuonID1=0; it_MuonID1<MuonID1s.size(); it_MuonID1++) {
    TString MuonID1 = MuonID1s.at(it_MuonID1);

    param.Clear();

    param.Muon_Tight_ID = MuonID1;

    param.Muon_ID_SF_Key = MuonIDSFKey;
    param.Muon_ISO_SF_Key = MuonISOSFKey;

    for(unsigned it_JetID=0; it_JetID<JetIDs.size(); it_JetID++) {
      param.Jet_ID = JetIDs[it_JetID];

      for(unsigned it_BJet=0; it_BJet<BTagNames.size(); it_BJet++) {
        JBparam.Clear();
  
        JBparam.BTagger    = BTagger;
        JBparam.BTaggingWP = BTaggingWPs[it_BJet];
        JBparam.BTagName   = BTagNames[it_BJet];
  
        for(unsigned it_Sign=0; it_Sign<DileptonSigns.size(); it_Sign++) {
          JBparam.DileptonSign = DileptonSigns[it_Sign];
  
          for(unsigned it_METCut=0; it_METCut<METCutNames.size(); it_METCut++) {
            JBparam.METCut= METCuts[it_METCut];
            JBparam.METCutName = METCutNames[it_METCut];
  
            param.Name = "";
            // if( JBparam.METCutName != "" ) param.Name += JBparam.METCutName+"_";          
            param.Name += JBparam.DileptonSign+"_"+MuonID1+"_"+param.Jet_ID;
            if( JBparam.BTagName != "" ) param.Name += "_"+JBparam.BTagName;
            if( JBparam.METCutName != "" ) param.Name += "_"+JBparam.METCutName; 
 
            executeEventFromParameter(param, JBparam);
          }
        }
      }
    }
  }
}

void NIsoMuon::executeEventFromParameter(AnalyzerParameter param, JBAnalyzerParameter JBparam){

  // if(!PassMETFilter()) return; // What's this? Do I need this filter in all analyses?

  Event ev = GetEvent();

  //GenLevelAnalysis(ev, param, JBparam);

  if( !(ev.PassTrigger(TriggerName) ) ) return;

  if( JBparam.METCutName != "" ) {
    if( !PassMETFilter() ) return;
    double MET = ev.GetMETVector().Pt();
    if( !(MET >= JBparam.METCut) ) return;
  }

  vector<Muon> muons = SelectMuons(AllMuons, param.Muon_Tight_ID, Subleading_Muon_Pt, 2.4);
  vector<Jet> jets = SelectJets(AllJets, param.Jet_ID, 30.,2.4);
  //vector<FatJet> fatjets = SelectFatJets(AllFatJets, "tight", 30, 2.4);
  //vector<FatJet> fatjetsLSF = SelectFatJets(AllFatJets, "tightLSF", 30, 2.4);

  if( JBparam.BTagName=="TwoVetoBJets" && jets.size()!=2 ) return;

  if(JBparam.BTagName == "BJet")
    jets = SelectBJets(jets, JBparam.BTagger, JBparam.BTaggingWP, true, 0);
  //else if(JBparam.BTagName == "VetoBJet")
  else if(JBparam.BTagName.Index("VetoBJet")!=kNPOS)
    jets = SelectLightJets(jets, JBparam.BTagger, JBparam.BTaggingWP, true, 0);

  if( JBparam.BTagName=="TwoVetoBJets" && jets.size()!=2 ) return;


  // Does ExampleRun.C code contain muon scaling/smearing & JEC?
  //std::sort(muons.begin(), muons.end(), PtComparing);
  //std::sort(jets.begin(), jets.end(), PtComparing);

  // if( JBparam.BTagName == "" && JBparam.METCutName == "" )
  NIsoDimuon(ev, param, JBparam, muons, jets);
}


// ====================================================================================== //
// weight                                                                                 //
// ====================================================================================== //
double NIsoMuon::MC_Weight(Event ev, AnalyzerParameter param) {
  double out = 1.;
  out *= weight_norm_1invpb * ev.GetTriggerLumi("Full"); // Is IsoMu24 unprescaled?
  out *= ev.MCweight();
  out *= weight_Prefire;
  
  return out;
}

double NIsoMuon::Muon_Weight(AnalyzerParameter param, vector<Lepton*> muons) {
  double out = 1.;

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



// ====================================================================================== //
// Start analysis                                                                         //
// ====================================================================================== //
void NIsoMuon::NIsoDimuon(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam, vector<Muon> muons, vector<Jet> jets) {
  JBparam.AnalysisName = "NIsoDimuon";
  TString this_region = param.Name+"_"+JBparam.AnalysisName;

  vector<Lepton*> dimuon;
  vector<Jet> this_jet;
  vector<FatJet> this_fatjet;

  //cout<<"Run NIsoDimuon"<<endl;
  //cout<<"nmuons: "<<muons.size()<<",  njets: "<<jets.size()<<endl;

  if( (muons.size() < 2) || (jets.size() == 0) ) return;

  for(unsigned iJet=0; iJet<jets.size(); iJet++) {
    for(unsigned iMu=0; iMu<muons.size(); iMu++) {
      if( !(muons[iMu].Pt() > Leading_Muon_Pt) ) return;
      if( !(jets[iJet].DeltaR(muons[iMu]) < 0.3) ) continue;
      for(unsigned jMu=iMu+1; jMu<muons.size(); jMu++) {
        if( !DimuonCharge(JBparam, muons[iMu], muons[jMu]) ) continue;
        if( !(jets[iJet].DeltaR(muons[jMu]) < 0.3) ) continue;

        dimuon.push_back( &muons[iMu] );
        dimuon.push_back( &muons[jMu] );
        this_jet.push_back( jets[iJet] );
        break;
      }
      if( this_jet.size() ) break;
    }
    if( this_jet.size() ) break;
  }

  if( !this_jet.size() ) return;


  double weight = 1.;
  if(!IsDATA) {
    weight = MC_Weight( ev , param );
    weight *= Muon_Weight( param , dimuon );
  }

  LeptonPlotter( dimuon, this_region, weight );
  JetPlotter(this_jet, this_fatjet, this_region, weight);
  DileptonPlotter( dimuon , this_region, weight );

  for(unsigned i=0; i<2; i++) {
    FillHist(this_region+"/Lepton_RelIso___"+this_region, dimuon[i]->RelIso(), weight, 500, 0., 5.);
  }

  FillHist(this_region+"/LeptonJet_DeltaR___"+this_region, dimuon[0]->DeltaR( this_jet[0] ), weight, 100, 0., 1.);
  FillHist(this_region+"/LeptonJet_DeltaR___"+this_region, dimuon[1]->DeltaR( this_jet[0] ), weight, 100, 0., 1.);

  FillHist(this_region+"/DileptonJet_DeltaR___"+this_region, this_jet[0].DeltaR( *dimuon[0] + *dimuon[1] ), weight, 100, 0., 1.);

  // FillHist(this_region+"/Jet_0_PtminusSumMuPt___"+this_region, this_jet[0].Pt()-dimuon[0]->Pt()-dimuon[1]->Pt(), weight, 2100, -100., 2000.);


  double DimuonMass = (*dimuon[0] + *dimuon[1]).M();
  if( 3. < DimuonMass && DimuonMass < 3.2 ) {
    LeptonPlotter( dimuon, this_region+"_JPsi", weight);
    FillHist(this_region+"_JPsi/LeptonJet_DeltaR___"+this_region+"_JPsi", this_jet[0].DeltaR(*dimuon[0]), weight, 60, 0., .3);
    FillHist(this_region+"_JPsi/LeptonJet_DeltaR___"+this_region+"_JPsi", this_jet[0].DeltaR(*dimuon[1]), weight, 60, 0., .3);
    FillHist(this_region+"_JPsi/Dilepton_dXY___"+this_region+"_JPsi", fabs(dimuon[0]->dXY()-dimuon[1]->dXY()), weight, 40, 0., .2);
    FillHist(this_region+"_JPsi/Dilepton_dZ___"+this_region+"_JPsi", fabs(dimuon[0]->dZ()-dimuon[1]->dZ()), weight, 40, 0., .2);
  }
  else if( 2. < DimuonMass && DimuonMass < 2.8 ) {
    LeptonPlotter( dimuon, this_region+"_JPsiL", weight);
    FillHist(this_region+"_JPsiL/LeptonJet_DeltaR___"+this_region+"_JPsiL", this_jet[0].DeltaR(*dimuon[0]), weight, 60, 0., .3);
    FillHist(this_region+"_JPsiL/LeptonJet_DeltaR___"+this_region+"_JPsiL", this_jet[0].DeltaR(*dimuon[1]), weight, 60, 0., .3);
    FillHist(this_region+"_JPsiL/Dilepton_dXY___"+this_region+"_JPsiL", fabs(dimuon[0]->dXY()-dimuon[1]->dXY()), weight, 40, 0., .2);
    FillHist(this_region+"_JPsiL/Dilepton_dZ___"+this_region+"_JPsiL", fabs(dimuon[0]->dZ()-dimuon[1]->dZ()), weight, 40, 0., .2);
  }
  else if( 3.3 < DimuonMass && DimuonMass < 3.5 ) {
    LeptonPlotter( dimuon, this_region+"_JPsiR", weight);
    FillHist(this_region+"_JPsiR/LeptonJet_DeltaR___"+this_region+"_JPsiR", this_jet[0].DeltaR(*dimuon[0]), weight, 60, 0., .3);
    FillHist(this_region+"_JPsiR/LeptonJet_DeltaR___"+this_region+"_JPsiR", this_jet[0].DeltaR(*dimuon[1]), weight, 60, 0., .3);
    FillHist(this_region+"_JPsiR/Dilepton_dXY___"+this_region+"_JPsiR", fabs(dimuon[0]->dXY()-dimuon[1]->dXY()), weight, 40, 0., .2);
    FillHist(this_region+"_JPsiR/Dilepton_dZ___"+this_region+"_JPsiR", fabs(dimuon[0]->dZ()-dimuon[1]->dZ()), weight, 40, 0., .2);
  }
  //else if( 9.2 < DimuonMass && DimuonMass < 10.5 ) {
  //  FillHist(this_region+"_Upsilon/LeptonJet_DeltaR___"+this_region+"_Upsilon", this_jet[0].DeltaR(*dimuon[0]), weight, 60, 0., .3);
  //  FillHist(this_region+"_Upsilon/LeptonJet_DeltaR___"+this_region+"_Upsilon", this_jet[0].DeltaR(*dimuon[1]), weight, 60, 0., .3);
  //}


  /*
  // --- Gen-level heavy flavor matching --- //
  if( MCSample.Index("QCD")==kNPOS ) return;
  JBparam.AnalysisName = "NIsoDimuon_HeavyFlavorQCD";
  this_region = param.Name+"_"+JBparam.AnalysisName;

  vector<Gen> gens = GetGens();
  vector<bool> muon_gen_match={false,false};
  vector<bool> muon_from_HF={false,false};

  for(unsigned igen=0; igen<gens.size(); igen++) {
    Gen gen = gens[igen];
    if( !(gen.isLastCopy()&&fabs(gen.PID())==13) ) continue;
    for(unsigned iMu=0; iMu<dimuon.size(); iMu++) {
      if( !(gen.Charge()==dimuon[iMu]->Charge()) ) continue;
      if( !( dimuon[iMu]->DeltaR(gen)<0.02 || 
             (dimuon[iMu]->DeltaR(gen)<0.3&&fabs(dimuon[iMu]->Pt()/gen.Pt()-1)<0.2) )
        ) continue;
      muon_gen_match[(dimuon[iMu]->Charge()+1)/2]=true;
      if( !(fromBottomMeson(gen, gens) || fromCharmMeson(gen, gens)) ) continue;
      muon_from_HF[(dimuon[iMu]->Charge()+1)/2]=true;
    }
    if(muon_gen_match[0]&&muon_gen_match[1]) break;
  }
  if( !(muon_from_HF[0]&&muon_from_HF[0]) ) return;

  DileptonPlotter( dimuon , this_region, weight );
  */


/*
  for(unsigned i=0; i<2; i++) {
    Muon* muon = (Muon*)dimuon[i];
    if( 10 < muon->Pt() && muon->Pt() < 15 ) {
      FillHist(this_region+"/Lepton10_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton10_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton10_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton10_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 20 < muon->Pt() && muon->Pt() < 25 ) {
      FillHist(this_region+"/Lepton20_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton20_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton20_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton20_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 30 < muon->Pt() && muon->Pt() < 40 ) {
      FillHist(this_region+"/Lepton30_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton30_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton30_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton30_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 50 < muon->Pt() && muon->Pt() < 70 ) {
      FillHist(this_region+"/Lepton50_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton50_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton50_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton50_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
    else if( 100 < muon->Pt() && muon->Pt() < 150 ) {
      FillHist(this_region+"/Lepton100_RelIso___"+this_region, muon->RelIso(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton100_TrkRelIso___"+this_region, muon->TrkIso()/muon->TuneP4().Pt(), weight, 500, 0., 5.);
      FillHist(this_region+"/Lepton100_PFIso___"+this_region, muon->RelIso()*muon->Pt(), weight, 2000, 0., 2000.);
      FillHist(this_region+"/Lepton100_TrkIso___"+this_region, muon->TrkIso(), weight, 2000, 0., 2000.);
    }
  }
*/


  // -------------------------------------------------------------------------------------- //
  Particle MET = ev.GetMETVector();

  
  FillHist(this_region+"/MET_Pt___"+this_region, MET.Pt(), weight, 1000,0.,1000.);
  FillHist(this_region+"/JetMET_DeltaPhi___"+this_region, fabs(MET.Phi()-this_jet[0].Phi()), weight, 320, 0, 3.2);

  if( JBparam.METCutName == "" ) return;
  if( !(fabs( MET.Phi() - this_jet[0].Phi() ) < 0.3) ) return;
  JBparam.AnalysisName = "NIsoDimuonMET";

  // LeptonPlotter( dimuon, this_region, weight );
  JetPlotter(this_jet, this_fatjet, this_region, weight);
  DileptonPlotter( dimuon , this_region, weight );
  // FillHist(this_region+"/MET_Pt___"+this_region, MET.Pt(), weight, 1000,0.,1000.);
  FillHist(this_region+"/DileptonMET_Mass___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
 

  /* 
  if( 11. < DimuonMass ) {
    FillHist(this_region+"/DileptonMET_Mass_M11___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  else if( 9.2 < DimuonMass && DimuonMass < 10.5 ) {
    FillHist(this_region+"/DileptonMET_Mass_Upsilon___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  else if( 3.0 < DimuonMass && DimuonMass < 3.2 ) {
    FillHist(this_region+"/DileptonMET_Mass_JPsi___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  else if( 3.6 < DimuonMass && DimuonMass < 3.8 ) {
    FillHist(this_region+"/DileptonMET_Mass_Psi2S___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  else if( .74 < DimuonMass && DimuonMass < .84 ) {
    FillHist(this_region+"/DileptonMET_Mass_rho___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  else if( .98 < DimuonMass && DimuonMass < 1.06 ) {
    FillHist(this_region+"/DileptonMET_Mass_phi___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  else { 
    FillHist(this_region+"/DileptonMET_Mass_Msmall___"+this_region, (*dimuon[0] + *dimuon[1] + MET).M(), weight, 1000,0.,1000.);
  }
  */


  /*
  if( MCSample.Index("QCD")==kNPOS ) return;
  if( !( ( *dimuon[0] + *dimuon[1] ).M() > 12 ) ) return;

  cout<<fixed;
  cout<<setprecision(2);
  cout<<endl<<endl;
  cout<<"+++++++++++++++++++++++++++++++ "<<JBparam.DileptonSign<<" M="<<(*dimuon[0]+*dimuon[1]).M()<<endl;
  for(unsigned iMu=0; iMu<dimuon.size(); iMu++) {
    Muon* muon = (Muon*)dimuon[iMu];
    cout<<"Mu  ";
    PrintPtEtaPhi( *muon );
    cout<<" iso: "<<setw(6)<<muon->TrkIso()/muon->Pt()<<endl;
  }
  cout<<"Jet ";
  PrintPtEtaPhi( this_jet[0] );
  cout<<" mu frac: "<<this_jet[0].muonFraction()<<endl;
  cout<<"MET ";
  PrintPtEtaPhi( MET );
  cout<<endl;

  cout<<" - - - - - - - - - - - - - - - "<<endl;
  cout<<"Event run: "<<run<<", event: "<<event<<endl;
  vector<Gen> gens = GetGens();
  vector<int> gennums;
  if( gens.size() == 0 ) return;
  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];

    if( !(gen.Status()==1&&gen.MotherIndex()!=0&&gen.MotherIndex()!=1) ) continue;
    if( !(this_jet[0].DeltaR( gen ) < 0.5 && gen.Pt() > 5) ) continue;

    gennums.push_back( iGen );
    vector<Gen> History = TrackMotherParticles(gen, gens);

    for(unsigned iHistory=0; iHistory<History.size(); iHistory++) {
      Gen TrackingGen = History[iHistory];
      GenName( TrackingGen );
      cout<<"<"<<setw(3)<<TrackingGen.Index()<<"> ";
      PrintPtEtaPhi( TrackingGen );
      cout<<" ~ ";
    }
    cout<<endl;
  }

  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];
    cout<<setw(3)<<gen.Index()<<"<"; GenName( gen ); cout<<"> status: "<<gen.Status()<<" PFS: "<<gen.isPromptFinalState()<<" ";
    PrintPtEtaPhi( gen );
    cout<<" <- "<<setw(3)<<gen.MotherIndex();
    for(unsigned igennum=0; igennum<gennums.size(); igennum++) {
      if( iGen == gennums[igennum] ) {
        cout<<"  *";
        break;
      }
    }
    cout<<endl;
  }
  */

}

void NIsoMuon::GenLevelAnalysis(Event ev, AnalyzerParameter param, JBAnalyzerParameter JBparam) {
  JBparam.AnalysisName = "GenLevel";
  TString this_region = param.Name+"_"+JBparam.AnalysisName;

  //cout<<endl<<endl;
  //cout<<"+++++++++++++++++++++++++++++++"<<endl;
  vector<Gen> gens = GetGens();
  vector<GenJet> genjets = GetAllGenJets();

  /*
  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];
    if(gen.MotherIndex()>-1) {
      cout<<setw(4)<<gen.Index()<<", ID:"<<setw(4)<<gen.PID()<<", "; PrintPtEtaPhi(gen); cout<<", status:"<<setw(3)<<gen.Status()<<", m:"<<setw(4)<<gen.MotherIndex()<<", mID:"<<gens.at(gen.MotherIndex()).PID()<<", hard"<<gen.isHardProcess()<<" last"<<gen.isLastCopy()<<" HPFS"<<gen.fromHardProcessFinalState();
      if(gen.isLastCopy()&&fabs(gen.PID())==13) {
        Gen mother = MotherParticle(gen,gens);
        cout<<" mI:"<<mother.Index()<<" mID:"<<mother.PID();
        if(mother.PID()==23) cout<<" << "<<mother.Index();
      }
      cout<<endl;

    }
    else {
      cout<<setw(4)<<gen.Index()<<", ID:"<<setw(4)<<gen.PID()<<", "; PrintPtEtaPhi(gen); cout<<", status:"<<setw(3)<<gen.Status()<<", m:"<<setw(4)<<gen.MotherIndex()<<", mID: -1, hard"<<gen.isHardProcess()<<" last"<<gen.isLastCopy()<<" HPFS"<<gen.fromHardProcessFinalState()<<endl;
    }

    if( !(gen.Status()==1&&gen.MotherIndex()!=0&&gen.MotherIndex()!=1) ) continue;
    vector<Gen> History = TrackMotherParticles(gen, gens);
    for(unsigned iHistory=0; iHistory<History.size(); iHistory++) {
      Gen TrackingGen = History[iHistory];
      GenName( TrackingGen );
      cout<<"("<<setw(2)<<TrackingGen.Index()<<") - ";
    }
    cout<<endl;
  }
  */
  

/*
  vector<Gen> gen_muons;
  Gen Zcandidate;
  for(unsigned iGen=0; iGen<gens.size(); iGen++) {
    Gen gen = gens[iGen];
    if(abs(gen.PID())==13&&gen.isLastCopy()) {
      Gen mother=MotherParticle(gen,gens);
      if(mother.PID()==23) {
        gen_muons.push_back(gen);
        Zcandidate=mother;
      }
    }
  }
  if(gen_muons.size()<2) { cout<<"[ERROR] muon size < 2"<<endl; return; }
  if(gen_muons.size()>2) { cout<<"[ERROR] muon size > 2"<<endl; return; }


  GenJet* ptr_genjet=NULL;
  double dR=9999;
  for(unsigned i=0; i<genjets.size(); i++) {
    GenJet* genjet = &genjets[i];
    if( genjet->DeltaR(Zcandidate) < dR ) {
      dR = genjet->DeltaR(Zcandidate);
      ptr_genjet = genjet;
    }
  }
  if(ptr_genjet==NULL) return;
  GenJet this_genjet = *ptr_genjet;

  for(unsigned i=0; i<gen_muons.size(); i++) {
    if(!(this_genjet.DeltaR(gen_muons[i])<0.4)) return;
  }

  double weight = 1.;
  if(!IsDATA) {
    weight = MC_Weight( ev , param );
    //weight *= Muon_Weight( param , dimuon );
  }

  FillHist( this_region+"/GenJet_Pt___"+this_region, this_genjet.Pt(), weight, 1000, 0., 1000);
  FillHist( this_region+"/GenJet_Eta___"+this_region, this_genjet.Eta(), weight, 100, -5, 5);
  for(unsigned i=0; i<gen_muons.size(); i++) {
    FillHist( this_region+"/GenLepton_Pt___"+this_region, gen_muons[i].Pt(), weight, 1000, 0., 1000);
    FillHist( this_region+"/GenLepton_Eta___"+this_region, gen_muons[i].Eta(), weight, 100, -5, 5);
    if( this_genjet.Pt() > 0 ) FillHist( this_region+"/GenLeptonGenJet_DeltaR___"+this_region, this_genjet.DeltaR( gen_muons[i] ), weight, 100,0.,1.);
  }
  FillHist( this_region+"/GenZ_Pt___"+this_region, Zcandidate.Pt(), weight, 1000, 0., 1000);
  FillHist( this_region+"/GenZ_Eta___"+this_region, Zcandidate.Eta(), weight, 100, -5, 5);
  FillHist( this_region+"/GenDilepton_Pt___"+this_region, (gen_muons[0]+gen_muons[1]).Pt(), weight, 1000, 0., 1000);
  FillHist( this_region+"/GenDilepton_Eta___"+this_region, (gen_muons[0]+gen_muons[1]).Eta(), weight, 100, -5, 5);
  if( this_genjet.Pt() > 0 ) FillHist( this_region+"/GenDileptonGenJet_DeltaR___"+this_region, this_genjet.DeltaR( gen_muons[0]+gen_muons[1] ), weight, 100,0.,1.);
  FillHist( this_region+"/GenDilepton_DeltaR___"+this_region, gen_muons[0].DeltaR( gen_muons[1] ), weight, 100,0.,1.);
  if( this_genjet.Pt() > 0 ) FillHist( this_region+"/GenZGenJet_DeltaR___"+this_region, this_genjet.DeltaR( Zcandidate ), weight, 100,0.,1.);

  FillHist( this_region+"/Efficiency___"+this_region, 0, weight, 8,0,8.);

  bool MuonPtCut  = (gen_muons[0].Pt()>Leading_Muon_Pt && gen_muons[1].Pt()>Subleading_Muon_Pt) || (gen_muons[0].Pt()>Subleading_Muon_Pt && gen_muons[1].Pt()>Leading_Muon_Pt);
  bool MuonEtaCut = fabs(gen_muons[0].Eta())<2.4 && fabs(gen_muons[1].Eta())<2.4;
  bool JetPtCut   = this_genjet.Pt()>30;
  bool JetEtaCut  = fabs(this_genjet.Eta())<2.4;
  bool AcceptanceCut = MuonPtCut&&MuonEtaCut&&JetPtCut&&JetEtaCut;
  if(AcceptanceCut)
    FillHist( this_region+"/Efficiency___"+this_region, 1, weight, 8,0,8.);


  if(AcceptanceCut&&!ev.PassTrigger(TriggerName)) {
    cout<<endl;
    cout<<"[Event "<<event<<"] Acceptance: "<<AcceptanceCut<<", Tirg: "<<ev.PassTrigger(TriggerName)<<endl;
    cout<<"- gen -"<<endl;
    cout<<" mu1: ("<<gen_muons[0].Pt()<<", "<<gen_muons[0].Eta()<<")"<<endl;
    cout<<" mu2: ("<<gen_muons[1].Pt()<<", "<<gen_muons[1].Eta()<<"), pt: "<<MuonPtCut<<", eta: "<<MuonEtaCut<<endl;
    cout<<" jet: ("<<this_genjet.Pt()<<", "<<this_genjet.Eta()<<"), pt: "<<JetPtCut<<", eta: "<<JetEtaCut<<endl;
    cout<<"- reco -"<<endl;
    for(unsigned i=0; i<AllMuons.size(); i++) {
      if(AllMuons[i].Pt()>5&&fabs(AllMuons[i].Eta())<2.8) cout<<" mu: ("<<AllMuons[i].Pt()<<", "<<AllMuons[i].Eta()<<", "<<AllMuons[i].Phi()<<"), glb: "<<AllMuons[i].isGlobalMuon()<<", trk: "<<AllMuons[i].isTrackerMuon()<<", PF: "<<AllMuons[i].isPFMuon()<<", loose: "<<AllMuons[i].PassID("POGLoose")<<", tight: "<<AllMuons[i].PassID("POGTight")<<endl;
    }
    cout<<" dR = "<<AllMuons[0].DeltaR(AllMuons[1])<<endl;
    for(unsigned i=0; i<AllJets.size(); i++) {
      if(AllJets[i].Pt()>15&&fabs(AllJets[i].Eta())<2.8) cout<<" jet: ("<<AllJets[i].Pt()<<", "<<AllJets[i].Eta()<<")"<<endl;
    }
  }

  // --- trigger --- //
  if( !(ev.PassTrigger(TriggerName) ) ) return;
  if(AcceptanceCut) FillHist( this_region+"/Efficiency___"+this_region, 2, weight, 8,0,8.);

  // --- reco matching --- //
  vector<Muon> muons = SelectMuons(AllMuons, "", Subleading_Muon_Pt, 2.4);
  vector<Jet> jets = SelectJets(AllJets, "", 30.,2.4);
  Jet* this_jet = FindRecoParticle(this_genjet, jets);
  vector<Muon*> this_muons;
  for(unsigned i=0; i<gen_muons.size(); i++) {
    this_muons.push_back((Muon*)FindRecoParticle(gen_muons[i], muons));
  }
  if(this_jet==NULL||this_muons[0]==NULL||this_muons[1]==NULL) return;
  if(!(this_muons[0]->Pt()>Leading_Muon_Pt||this_muons[1]->Pt()>Leading_Muon_Pt)) return;
  FillHist( this_region+"/Efficiency___"+this_region, 3, weight, 8,0,8.);

  this_jet=NULL;
  jets = SelectJets(jets, param.Jet_ID, 30.,2.4);
  this_jet = FindRecoParticle(this_genjet, jets);
  if(this_jet==NULL) return;
  FillHist( this_region+"/Efficiency___"+this_region, 4, weight, 8,0,8.);

  this_muons.clear();
  muons = SelectMuons(muons, param.Muon_Tight_ID, Subleading_Muon_Pt, 2.4);
  for(unsigned i=0; i<gen_muons.size(); i++) {
    this_muons.push_back((Muon*)FindRecoParticle(gen_muons[i], muons));
  }
  if(this_muons[0]==NULL||this_muons[1]==NULL) return;
  if(!(this_muons[0]->Pt()>Leading_Muon_Pt||this_muons[1]->Pt()>Leading_Muon_Pt)) return;
  FillHist( this_region+"/Efficiency___"+this_region, 5, weight, 8,0,8.);

////////////////////////////////////////////////////////

  for(unsigned i=0; i<2; i++) {
    FillHist( this_region+"/Lepton_RelIso___"+this_region, this_muons[i]->RelIso(), weight, 500, 0., 5.);
    FillHist( this_region+"/Lepton_Iso___"+this_region, this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 100, 0., 100.);
    FillHist( this_region+"/Lepton_RelTrkIso___"+this_region, this_muons[i]->TrkIso()/this_muons[i]->Pt(), weight, 500, 0., 5.);
    FillHist( this_region+"/Lepton_TrkIso___"+this_region, this_muons[i]->TrkIso(), weight, 100, 0., 100.);

    double trkiso=this_muons[i]->TrkIso();
    unsigned j=1; if(i==1) j=0;
    if(this_muons[i]->DeltaR(*this_muons[j])<0.3&&this_muons[i]->TrkIso()>this_muons[j]->Pt()) trkiso=trkiso-this_muons[j]->Pt();
    FillHist( this_region+"/Lepton_RelTrkIsoCorr___"+this_region, trkiso/this_muons[i]->Pt(), weight, 500, 0., 5.);

    FillHist( this_region+"/Lepton_Pt_Lepton_Iso___"+this_region, this_muons[i]->Pt(), this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 20,0,100.,20,0,100.);
    FillHist( this_region+"/Lepton_Pt_Lepton_TrkIso___"+this_region, this_muons[i]->Pt(), this_muons[i]->TrkIso(), weight, 20,0,100.,100,0,100.);
    FillHist( this_region+"/Lepton_Pt_Lepton_TrkIsoCorr___"+this_region, this_muons[i]->Pt(), trkiso, weight, 20,0,100.,100,0,100.);
    FillHist( this_region+"/LeptonJet_DeltaR_Lepton_Iso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 100,0,1.,100,0,100.);
    FillHist( this_region+"/LeptonJet_DeltaR_Lepton_TrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->TrkIso(), weight, 100,0,1.,100,0,100.);
    FillHist( this_region+"/LeptonJet_DeltaR_Lepton_TrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[i]), trkiso, weight, 100,0,1.,100,0,100.);
    FillHist( this_region+"/LeptonJet_DeltaR_Lepton_RelIso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->RelIso(), weight, 100,0,1.,300,0,3.);
    FillHist( this_region+"/LeptonJet_DeltaR_Lepton_RelTrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[i]), this_muons[i]->TrkIso()/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
    FillHist( this_region+"/LeptonJet_DeltaR_Lepton_RelTrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[i]), trkiso/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
    FillHist( this_region+"/DileptonJet_DeltaR_Lepton_Iso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->RelIso()*this_muons[i]->Pt(), weight, 100,0,1.,100,0,100.);
    FillHist( this_region+"/DileptonJet_DeltaR_Lepton_TrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->TrkIso(), weight, 100,0,1.,100,0,100.);
    FillHist( this_region+"/DileptonJet_DeltaR_Lepton_TrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), trkiso, weight, 100,0,1.,100,0,100.);
    FillHist( this_region+"/DileptonJet_DeltaR_Lepton_RelIso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->RelIso(), weight, 100,0,1.,300,0,3.);
    FillHist( this_region+"/DileptonJet_DeltaR_Lepton_RelTrkIso___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), this_muons[i]->TrkIso()/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);
    FillHist( this_region+"/DileptonJet_DeltaR_Lepton_RelTrkIsoCorr___"+this_region, this_jet[0].DeltaR(*this_muons[0]+*this_muons[1]), trkiso/this_muons[i]->Pt(), weight, 100,0,1.,300,0,3.);

    FillHist( this_region+"/LeptonJet_DeltaR___"+this_region, this_muons[i]->DeltaR( this_jet[0] ), weight, 100, 0., 1.);
  }

  FillHist( this_region+"/Dilepton_DeltaR___"+this_region, this_muons[0]->DeltaR( *this_muons[1] ), weight, 100, 0., 1.);
  FillHist( this_region+"/DileptonJet_DeltaR___"+this_region, this_jet[0].DeltaR( *this_muons[0] + *this_muons[1] ), weight, 100, 0., 1.);

////////////////////////////////////////////////////////

  if(!(this_muons[0]->RelIso()>0.3&&this_muons[1]->RelIso()>0.3)) return;
  FillHist( this_region+"/Efficiency___"+this_region, 6, weight, 8,0,8.);

  if(!(this_jet->DeltaR(*this_muons[0])<0.3&&this_jet->DeltaR(*this_muons[1])<0.3)) return;
  FillHist( this_region+"/Efficiency___"+this_region, 7, weight, 8,0,8.);
*/

}

 
NIsoMuon::NIsoMuon(){

}

NIsoMuon::~NIsoMuon(){

}

