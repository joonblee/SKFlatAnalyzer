### ==================================== ###
### === setup ###
### ==================================== ###
# Trigger setup #
TurnOn_HighPtMuonTrig=false;
TurnOn_NonIsolatedDoubleMuonTrig=true;
TurnOn_SingleMuonTrig=false;
TurnOn_MuonEGTrig=false;

# sample setup #
RUN_DT=true;
RUN_MC=false;
RUN_QCDonly=false; # Available only with 'RUN_MC=true' and 2016 run
RUN_SIG=false;

# skim setup #
UseSkim=true;

Analyzer="NIsoMuon" # "DijetTEST" "Dielectron" "Dimuon" "NIsoElectron"
#Analyzer="NIsoDimuon_emuCR" 
#Analyzer="SemiNIsoDimuon"


#make distclean
#make clean
make


Triggers=()
if $TurnOn_HighPtMuonTrig
then
  Triggers+=("HighPtMuon")
fi
if $TurnOn_NonIsolatedDoubleMuonTrig
then
  Triggers+=("NonIsolatedDoubleMuon")
fi
if $TurnOn_SingleMuonTrig
then
   Triggers+=("SingleMuon")
fi
if $TurnOn_MuonEGTrig
then
   Triggers+=("MuonEG")
fi


#Triggers=("HighPtMuon" "NonIsolatedDoubleMuon")
#"SingleElectronSample_NoTrigger" "SingleMuon" "DoubleMuon" "HighPtMuon" "DoubleMuon-HighPtCut" "SingleMuon-HighPtCut" "DoubleElectron_NoIso" "DoubleElectron_NoIso" "SameSignDoubleMuon"

MCsample="" #"MC2018samples.txt"
DATAsample=""
SIGsample="samples/SIG2016samples.txt"
# SIGsample="samples/SIG2016samples_preliminary.txt"
# "DoubleMuon", "DoubleEG", "SingleMuon", "SingleElectron", "SinglePhoton", "MuonEG", "EGamma" (for 2018)

Years=(2016)
#Years=(2016 2017 2018)

nBatch=40



for Trigger in ${Triggers[@]}
do

  if [[ $Trigger == "HighPtMuon" ]] || [[ $Trigger == *"Single"* ]]
  then
    DATAsample="SingleMuon"
  elif [[ $Trigger == *"Double"* ]]
  then
    DATAsample="DoubleMuon"
  elif [[ $Trigger == *"MuonEG"* ]]
  then
    DATAsample="MuonEG"
  fi

  for Year in ${Years[@]}
  do

    if [ $Year -eq 2016 ] || [ $Year -eq 2017 ]
    then
      MCsample="samples/MC2016samples.txt"
      if $RUN_QCDonly
      then
        MCsample="samples/QCD2016samples.txt"
      fi
    elif [ $Year -eq 2018 ]
    then
      MCsample="samples/MC2018samples.txt"
    fi

    if $UseSkim
    then
      if $RUN_MC
      then 
        SKFlat.py -a $Analyzer -l $MCsample -t $Trigger --skim SkimTree_NIsoMuon -y $Year -n $nBatch &>log/submit_${Year}_MC_${Trigger}.log&
        echo "[SKFlat.py] Run analyzer with MC samples: " $MCsample " trigger: " $Trigger " year: " $Year
        sleep 60
      else
        echo "[SKFlat.py] Do not make MC samples"
      fi
      if $RUN_DT
      then
        SKFlat.py -a $Analyzer -i $DATAsample -t $Trigger --skim SkimTree_NIsoMuon -y $Year -n $nBatch &>log/submit_${Year}_${DATAsample}_${Trigger}.log&
        echo "[SKFlat.py] Run analyzer with DT: " $DATAsample " trigger: " $Trigger " year: " $Year
        disown -a
      else
        echo "[SKFlat.py] Do not make DATA samples"
      fi
    else
      if $RUN_MC
      then 
        SKFlat.py -a $Analyzer -l $MCsample -t $Trigger -y $Year -n $nBatch &>log/submit_${Year}_MC_${Trigger}.log&
        echo "[SKFlat.py] Run analyzer with MC samples: " $MCsample " trigger: " $Trigger " year: " $Year
        sleep 60
      else
        echo "[SKFlat.py] Do not make MC samples"
      fi
      if $RUN_DT
      then
        SKFlat.py -a $Analyzer -i $DATAsample -t $Trigger -y $Year -n $nBatch &>log/submit_${Year}_${DATAsample}_${Trigger}.log&
        echo "[SKFlat.py] Run analyzer with DT: " $DATAsample " trigger: " $Trigger " year: " $Year
        disown -a
      else
        echo "[SKFlat.py] Do not make DATA samples"
      fi
      if $RUN_SIG
      then
        SKFlat.py -a $Analyzer -l $SIGsample -t $Trigger -y $Year -n $nBatch &>log/submit_${Year}_SIG_${Trigger}.log&
        echo "[SKFlat.py] Run analyzer with SIG samples: " $MCsample " trigger: " $Trigger " year: " $Year
        sleep 60
      else
        echo "[SKFlat.py] Do not make SIGNAL samples"
      fi
    fi

    sleep 100

    #nJobs=`ps aux | grep -v "grep" | grep "joonblee" | grep -c "condor_shadow"`
    #while [ $nJobs -ge 50 ]
    #do
    #  #echo $nJobs
    #  #nJobs=$(($nJobs+1))
    #  sleep 100
    #  nJobs=`ps aux | grep -v "grep" | grep "joonblee" | grep -c "condor_shadow"`
    #  echo $nJobs
    #done

  done
done



# make && SKFlat.py -a GenAnalyzer_NIsoMuon -l samples/QCD2016samples.txt -t NonIsolatedDoubleMuon -y 2016 -n 20 &>run.log&
# make && SKFlat.py -a GenAnalyzer_NIsoMuon -l samples/QCD2016samples_Pt_170toInf.txt -t NonIsolatedDoubleMuon -y 2016 -n 20 &>run.log&

# SKFlat.py -a WMassMeasurement -i SingleMuon -t SingleMuon -y 2016 -n 20
# SKFlat.py -a WMassMeasurement -i WJets_MG -t SingleMuon -y 2016 -n 20

################################################################
#
#Years=(2018)
#FULLsample=""
#
#for Year in ${Years[@]}
#do
#
#  FULLsample="FULL"${Year}"samples.txt"
#
#  SKFlat.py -a SkimTree_NIsoMuon -l $FULLsample -n 0 -y $Year --nmax 150 &>submit_skim.log&
#
#done
#
################################################################

