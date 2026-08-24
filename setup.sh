export SKFlat_WD=`pwd`
export SKFlat_LIB_PATH=$SKFlat_WD/lib/
mkdir -p $SKFlat_LIB_PATH
mkdir -p $SKFlat_WD/tar

# Input/sample-metadata campaign version.
# Keep this unchanged unless the data/<SKFlatV>/... metadata tree is also changed.
export SKFlatV="Run2UltraLegacy_v3"
mkdir -p $SKFlat_WD/data/$SKFlatV
export DATA_DIR=$SKFlat_WD/data/$SKFlatV

# Output campaign label.
# This is intentionally separated from SKFlatV so that input metadata can remain
# under data/Run2UltraLegacy_v3 while analyzer outputs are written under
# SKOutput/Run2UL_v3_Run3_v13.
export SKFlatOutputV="Run2UL_v3_Run3_v13"

#### use cvmfs for root ####
export CMS_PATH=/cvmfs/cms.cern.ch
source $CMS_PATH/cmsset_default.sh
export SCRAM_ARCH=el9_amd64_gcc12
export cmsswrel='cmssw/CMSSW_15_0_1'
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/src
echo "@@@@ SCRAM_ARCH = "$SCRAM_ARCH
echo "@@@@ cmsswrel = "$cmsswrel
echo "@@@@ scram..."
eval `scramv1 runtime -sh`
cd -
source /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/external/$SCRAM_ARCH/bin/thisroot.sh

export CORRECTIONLIB_INCLUDE=/cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-correctionlib/2.6.4-1de964987efb91372e4d54c3ac6e84ea/lib/python3.9/site-packages/correctionlib/include
export CORRECTIONLIB_LIBDIR=/cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-correctionlib/2.6.4-1de964987efb91372e4d54c3ac6e84ea/lib/python3.9/site-packages/correctionlib/lib
export LD_LIBRARY_PATH=${CORRECTIONLIB_LIBDIR}:${LD_LIBRARY_PATH}

if [[ $HOSTNAME == *"ui"*".sdfarm.kr"* ]]; then

  echo "@@@@ Working on KISTI"
  export SKFlatRunlogDir="/cms/ldap_home/$USER/SKRunlog/"
  export SKFlatOutputDir="/cms/ldap_home/$USER/SKOutput/"

elif [[ $HOSTNAME == *"tamsa1"* ]]; then

  echo "@@@@ Working on tamsa1"
  export SKFlatRunlogDir="/data6/Users/$USER/SKRunlog/"
  export SKFlatOutputDir="/data6/Users/$USER/SKOutput/"

elif [[ $HOSTNAME == *"tamsa2"* ]]; then

  echo "@@@@ Working on tamsa2"
  export SKFlatRunlogDir="/data6/Users/$USER/SKRunlog/"
  export SKFlatOutputDir="/data6/Users/$USER/SKOutput/"

elif [[ $HOSTNAME == *"knu"* ]]; then

  echo "@@@@ Working on KNU"
  export SKFlatRunlogDir="/d0/scratch/$USER/SKRunlog/"
  export SKFlatOutputDir="/d0/scratch/$USER/SKOutput/"

fi

mkdir -p $SKFlatRunlogDir
mkdir -p $SKFlatOutputDir
mkdir -p $SKFlatOutputDir/$SKFlatOutputV

alias skout="cd $SKFlatOutputDir/$SKFlatOutputV/"
alias sklog="cd $SKFlatRunlogDir/"

export MYBIN=$SKFlat_WD/bin/
export PYTHONDIR=$SKFlat_WD/python/
export PATH=${MYBIN}:${PYTHONDIR}:${PATH}

export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$SKFlat_WD/DataFormats/include/:$SKFlat_WD/AnalyzerTools/include/:$SKFlat_WD/Analyzers/include/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SKFlat_LIB_PATH
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/bin/python2.7:$LD_LIBRARY_PATH

source $SKFlat_WD/bin/BashColorSets.sh

## submodules ##
#source bin/CheckSubmodules.sh

if [ "$1" = "-q" ];then
    return
fi

alias python="python3"
## Todo list ##
python python/PrintToDoLists.py
source $SKFlat_WD/tmp/ToDoLists.sh
rm $SKFlat_WD/tmp/ToDoLists.sh

CurrentGitBranch=`git branch | grep \* | cut -d ' ' -f2`
printf "> Current SKFlatAnalyzer branch : "${BRed}$CurrentGitBranch${Color_Off}"\n"
echo "-----------------------------------------------------------------"

## Log Dir ##
#python python/PrintOldLogs.py
#source $SKFlat_WD/tmp/OldLogs.sh
#rm $SKFlat_WD/tmp/OldLogs.sh
echo "* Your Log Directory Usage (ctrl+c to skip)"
du -sh $SKFlatRunlogDir
