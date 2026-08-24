#!/bin/bash

##############
### setups ###
##############

# run setups
RUN_DT=true
RUN_MC=true
RUN_QCDonly=false   # Available only with 'RUN_MC=true'
RUN_SIG=true

UseSkim=true

# -----------------------------------------------------------------------------
# NIsoMuon analysis / systematic modes
#
# Each entry is run independently.
#
#   ""                    : nominal NIsoDimuon analysis
#   "RunSyst"             : experimental-systematic variations
#   "RunXSecSyst"         : generator PDF/scale/alpha_s variations
#   "MuonIDEfficiency"    : muon-ID tag-and-probe mode (central only)
#   "TriggerEfficiency"   : HighPtMuon-trigger tag-and-probe mode (central only)
#
# Examples:
#   flags=("" "RunSyst" "RunXSecSyst")
#   flags=("MuonIDEfficiency" "TriggerEfficiency")
#   flags=("" "RunSyst" "RunXSecSyst" "MuonIDEfficiency" "TriggerEfficiency")
#
# Current selection:
flags=("" "RunSyst" "RunXSecSyst")

# analysis setups
analysis="NIsoMuon"
skim="SkimTree_NIsoMuon"   # Skim name used when UseSkim=true
TriggerSets=("HighPtMuon") # ("NonIsolatedDoubleMuon") # HighPtMuon SingleMuon MuonEG MET200
#TriggerSets=("SingleMuon") # ("NonIsolatedDoubleMuon") # HighPtMuon SingleMuon MuonEG MET200

#make distclean
#make clean
make

signalset="SampleLists/Run2signal.txt"
dataset=""
mcset="SampleLists/Run2mc.txt"
systset="SampleLists/Run2Syst.txt"
xsecsystset="SampleLists/Run2XSecSyst.txt"
if $RUN_QCDonly; then
  mcset="SampleLists/Run2qcd.txt"
fi

Eras=(2016preVFP 2016postVFP 2017 2018)

nBatch=50
batchname=""       # e.g. "NIsoMuon"
extra_args=()      # e.g. ("--no_exec") or ("--memory" "4000")

mkdir -p log

is_efficiency_mode() {
  local flag="$1"
  [[ "$flag" == "MuonIDEfficiency" || "$flag" == "TriggerEfficiency" ]]
}

is_systematic_mode() {
  local flag="$1"
  [[ "$flag" == "RunSyst" || "$flag" == "RunXSecSyst" ]]
}

echo ""
echo "// ------------------------------------------------------ //"
echo "// ----------------- Run SKFlatAnalyzer ----------------- //"
echo "// ------------------------------------------------------ //"
echo ""
echo -n "// time: "
date
echo ""

for trig in "${TriggerSets[@]}"; do

  if [[ $trig == *"DoubleMuon"* ]]; then
    dataset="DoubleMuon"
  elif [[ $trig == "HighPtMuon" ]] || [[ $trig == *"SingleMuon"* ]]; then
    dataset="SingleMuon"
  elif [[ $trig == *"MET"* ]]; then
    dataset="MET"
  else
    dataset="$trig"
  fi

  for era in "${Eras[@]}"; do
    for flag in "${flags[@]}"; do

      cmd_common=(SKFlat.py -a "$analysis" -t "$trig" -e "$era" -n "$nBatch" --nmax 4000)
      if $UseSkim; then
        cmd_common+=(--skim "$skim")   # use skim
      fi

      mcset_this="$mcset"
      if [[ "$flag" == "RunSyst" ]]; then
        mcset_this="$systset"
      elif [[ "$flag" == "RunXSecSyst" ]]; then
        mcset_this="$xsecsystset"
      fi

      echo " - Era: $era"
      echo " - Trigger: $trig"
      echo " - Dataset: $dataset"
      echo " - MCset: $mcset_this"

      if [[ -n "$flag" ]]; then
        cmd_common+=(--userflags "$flag")
        echo " - flag: $flag"
        if is_efficiency_mode "$flag"; then
          echo " - mode: central-only efficiency measurement"
        elif [[ "$flag" == "RunSyst" ]]; then
          echo " - mode: NIsoDimuon experimental systematics"
        elif [[ "$flag" == "RunXSecSyst" ]]; then
          echo " - mode: NIsoDimuon generator theory systematics"
        fi
      else
        echo " - flag: <none>"
        echo " - mode: nominal NIsoDimuon"
      fi
      echo ""

      # MuonIDEfficiency and TriggerEfficiency are central-only modes and use
      # the ordinary nominal MC list, never the RunXSecSyst list.

      if [[ -n "$batchname" ]]; then
        cmd_common+=(--batchname "$batchname")
      fi

      if ((${#extra_args[@]})); then
        cmd_common+=("${extra_args[@]}")
      fi

      # Data is never submitted for either systematic-production mode.
      if $RUN_DT && ! is_systematic_mode "$flag"; then
        "${cmd_common[@]}" -i "$dataset" &> "log/submit_${era}_${dataset}_${trig}${flag:+__${flag}}.log" &
        echo "[SKFlat.py] Run analyzer for data: $dataset trigger: $trig era: $era"
      else
        echo "[SKFlat.py] Do not make DATA samples"
      fi

      if $RUN_MC; then
        "${cmd_common[@]}" -l "$mcset_this" &> "log/submit_${era}_MC_${trig}${flag:+__${flag}}.log" &
        echo "[SKFlat.py] Run analyzer for MC backgrounds: $mcset_this trigger: $trig era: $era"
      else
        echo "[SKFlat.py] Do not make MC samples"
      fi

      # Signal MC supports nominal, RunSyst, and RunXSecSyst production.
      # The dedicated efficiency modes remain background/data-only.
      if $RUN_SIG && ! is_efficiency_mode "$flag"; then
        "${cmd_common[@]}" -l "$signalset" &> "log/submit_${era}_sig_${trig}${flag:+__${flag}}.log" &
        echo "[SKFlat.py] Run analyzer for signals: $signalset trigger: $trig era: $era"
      else
        echo "[SKFlat.py] Do not make signal samples"
      fi

      disown -a
      sleep 10

    done
  done
done
