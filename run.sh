#make clean
make

Analyzer="NIsoMuon"
#Analyzer="NIsoMuon"
#Analyzer="DijetTEST"
#Analyzer="Dielectron"
#Analyzer="Dimuon"
#Analyzer="NIsoElectron"

#Trigger="SingleMuon"
#Trigger="DoubleMuon"
Trigger="HighPtMuon"
#Trigger="DoubleMuon-HighPtCut"
#Trigger="SingleMuon-HighPtCut"
#Trigger="DoubleElectron_NoIso"
#Trigger="DoubleElectron_NoIso"

#InputSample="DoubleMuon"
InputSample="SingleMuon"

# "DoubleMuon", "DoubleEG", "SingleMuon", "SingleElectron", "SinglePhoton", "MuonEG", "EGamma"

#Year=2016
Year=2016

nBatch=10
#SkimTree="NIsoMuon"

SKFlat.py -a $Analyzer -l MCsamples.txt -t $Trigger -y $Year -n $nBatch &>submit_MC.log&

sleep 60

SKFlat.py -a $Analyzer -i $InputSample -t $Trigger -y $Year -n $nBatch &>submit_DT.log&

sleep 60

Trigger="SingleMuon"

SKFlat.py -a $Analyzer -l MCsamples.txt -t $Trigger -y $Year -n $nBatch &>submit_MC.log&

sleep 60

SKFlat.py -a $Analyzer -i $InputSample -t $Trigger -y $Year -n $nBatch &>submit_DT.log&

sleep 60

Trigger="DoubleMuon"
InputSample="DoubleMuon"

SKFlat.py -a $Analyzer -l MCsamples.txt -t $Trigger -y $Year -n $nBatch &>submit_MC.log&

sleep 60

SKFlat.py -a $Analyzer -i $InputSample -t $Trigger -y $Year -n $nBatch &>submit_DT.log&



#SKFlat.py -a SkimTree_NIsoMuon -l FULLsamples.txt -n 0 -y 2016 --nmax 100 &>submit_skim.log&



#sleep 60

#echo ""
#echo "### ----- Job submitted ----- ###"
#echo ""

disown -a

