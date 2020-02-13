#make clean
make

Analyzer="NIsoMuon"
#Analyzer="NIsoMuon"
#Analyzer="DijetTEST"
#Analyzer="Dielectron"
#Analyzer="Dimuon"
#Analyzer="NIsoElectron"

#DATAsamples=("DoubleMuon" "SingleMuon")
#DATAsamples=("SingleElectron")
#DATAsample="DoubleMuon"
DATAsamples=("DoubleMuon" "SingleMuon")

MCsample="" #"MC2018samples.txt"

Triggers=("")
#Triggers=("SingleElectronSample_NoTrigger")
#Trigger="SingleMuon"
#Trigger="DoubleMuon"
#Trigger="HighPtMuon"
#Trigger="DoubleMuon-HighPtCut"
#Trigger="SingleMuon-HighPtCut"
#Trigger="DoubleElectron_NoIso"
#Trigger="DoubleElectron_NoIso"

# "DoubleMuon", "DoubleEG", "SingleMuon", "SingleElectron", "SinglePhoton", "MuonEG", "EGamma" (for 2018)

Years=(2016)
#Years=(2016 2017 2018)

nBatch=20

#SkimTree="NIsoMuon"

for DATAsample in ${DATAsamples[@]}
do

  if [[ $DATAsample == *"SingleMuon"* ]]
  then
    Triggers=("HighPtMuon")
    #Triggers=("SingleMuon" "HighPtMuon")
  elif [[ $DATAsample == *"DoubleMuon"* ]]
  then
    Triggers=("DoubleMuon")
  fi

  for Trigger in ${Triggers[@]}
  do

    for Year in ${Years[@]}
    do

      if [ $Year -eq 2016 ] || [ $Year -eq 2017 ]
      then
        MCsample="MC2016samples.txt"
      elif [ $Year -eq 2018 ]
      then
        MCsample="MC2018samples.txt"
      fi

      SKFlat.py -a $Analyzer -l $MCsample -t $Trigger --skim SkimTree_NIsoMuon -y $Year -n $nBatch &>submit_${Year}_MC_${Trigger}.log&
      sleep 60
      SKFlat.py -a $Analyzer -i $DATAsample -t $Trigger --skim SkimTree_NIsoMuon -y $Year -n $nBatch &>submit_${Year}_${DATAsample}_${Trigger}.log&
      disown -a
      sleep 600

      nJobs=`ps aux | grep -v "grep" | grep "joonblee" | grep -c "condor_shadow"`
      while [ $nJobs -ge 50 ]
      do
        #echo $nJobs
        #nJobs=$(($nJobs+1))
        sleep 300
        nJobs=`ps aux | grep -v "grep" | grep "joonblee" | grep -c "condor_shadow"`
        echo $nJobs
      done

    done
  done
done

'''
################################################################

Years=(2017 2018)
FULLsample=""

for Year in ${Years[@]}
do

  FULLsample="FULL"${Year}"samples.txt"

  SKFlat.py -a SkimTree_NIsoMuon -l $FULLsample -n 0 -y $Year --nmax 100 &>submit_skim.log&

done

################################################################
'''
disown -a

