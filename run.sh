make clean
make

Analyzer="NIsoMuon"
#Analyzer="NIsoMuon"
#Analyzer="DijetTEST"
#Analyzer="Dielectron"
#Analyzer="Dimuon"
#Analyzer="NIsoElectron"

#InputSamples=("DoubleMuon" "SingleMuon")
#InputSamples=("SingleElectron")
#InputSample="DoubleMuon"
InputSamples=("SingleMuon" "DoubleMuon")

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

#Years=(2018)
Years=(2016 2017 2018)

nBatch=10

#SkimTree="NIsoMuon"

for InputSample in ${InputSamples[@]}
do

  if [ $InputSample = "SingleMuon" ]
  then
    Triggers=("HighPtMuon")
    #Triggers=("SingleMuon" "HighPtMuon")
  elif [ $InputSample = "DoubleMuon" ]
  then
    Triggers=("DoubleMuon")
  fi

  for Trigger in ${Triggers[@]}
  do

    for Year in ${Years[@]}
    do

      #if [ $Year -eq 2016 ] || [ $Year -eq 2017 ]
      #then
      #  MCsample="MC2016samples.txt"
      #elif [ $Year -eq 2018 ]
      #then
      #  MCsample="MC2018samples.txt"
      #fi
      MCsample="MCqcd.txt"

      SKFlat.py -a $Analyzer -l $MCsample -t $Trigger -y $Year -n $nBatch &>submit_${Year}_MC_${Trigger}.log&
      #sleep 60
      #SKFlat.py -a $Analyzer -i $InputSample -t $Trigger -y $Year -n $nBatch &>submit_${Year}_${InputSample}_${Trigger}.log&
      disown -a
      sleep 600

      nJobs=`ps aux | grep -v "grep" | grep "joonblee" | grep -c "condor_shadow"`
      while [ $nJobs -ge 100 ]
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

################################################################

#SKFlat.py -a SkimTree_NIsoMuon -l FULLsamples.txt -n 0 -y 2016 --nmax 100 &>submit_skim.log&

################################################################

#echo ""
#echo "### ----- Job submitted ----- ###"
#echo ""

disown -a

