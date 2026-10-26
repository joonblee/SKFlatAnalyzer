#!/bin/bash
# Source-safe entry point: all settings and exits stay in this subshell.
(

##############
### setups ###
##############

# run setups
RUN_DT=true
RUN_MC=true
RUN_SIG=false

UseSkim=true

# analysis setups
analysis="NIsoMuon"
skim="SkimTree_NIsoMuon"   # Skim name used when UseSkim=true
TriggerSets=("HighPtMuon") # ("NonIsolatedDoubleMuon") # HighPtMuon SingleMuon MuonEG MET200
#TriggerSets=("SingleMuon") # ("NonIsolatedDoubleMuon") # HighPtMuon SingleMuon MuonEG MET200


signalset="SampleLists/Run2signal.txt"
dataset=""
mcset="SampleLists/Run2mc.txt"
systset="SampleLists/Run2Syst.txt"


nBatch=50
batchname=""       # e.g. "NIsoMuon"
extra_args=()      # e.g. ("--no_exec") or ("--memory" "4000")


# CLI options are local to the subshell, including when this file is sourced.
era_selection="Run2"
flag_selection="nominal"
top_pt_reweight="${TOP_PT_REWEIGHT:-false}"
dry_run=false
print_usage() {
  cat <<'HELP'
Usage: source run.sh [options]     (bash run.sh also works)
  --era, --eras LIST   Run2 (default), ERA, or comma-separated eras/runs
  --flags LIST        nominal (default), RunSyst, RunXSecSyst, or all
                      Comma-separated modes are submitted independently.
                      Existing efficiency/diagnostic mode names are accepted.
  --top-pt-reweight [true|false]   Optional ttbar-only study (default: false)
  --no-top-pt-reweight            Explicitly disable the study
  --dry-run                      Print commands without building/submitting
  -h, --help
Examples:
  source run.sh
  source run.sh --era 2018 --flags nominal,RunSyst
  source run.sh --era Run2 --flags all --top-pt-reweight
  TOP_PT_REWEIGHT=true bash run.sh --flags all
HELP
}
while (( $# )); do
  case "$1" in
    --era|--eras|--flags)
      if (( $# < 2 )) || [[ "$2" == --* ]]; then echo "[ERROR] $1 requires a value" >&2; exit 2; fi
      if [[ "$1" == "--flags" ]]; then flag_selection="$2"; else era_selection="$2"; fi
      shift 2 ;;
    --top-pt-reweight)
      top_pt_reweight=true
      if [[ "${2:-}" == "true" || "${2:-}" == "false" ]]; then top_pt_reweight="$2"; shift; fi
      shift ;;
    --no-top-pt-reweight) top_pt_reweight=false; shift ;;
    --dry-run) dry_run=true; shift ;;
    -h|--help) print_usage; exit 0 ;;
    *) echo "[ERROR] Unknown option: $1" >&2; print_usage; exit 2 ;;
  esac
done
case "$top_pt_reweight" in true|false) ;; *) echo "[ERROR] TOP_PT_REWEIGHT must be true or false" >&2; exit 2 ;; esac

Eras=()
IFS=',' read -r -a selections <<< "$era_selection"
for selection in "${selections[@]}"; do
  case "$selection" in
    Run2|run2) expanded=(2016preVFP 2016postVFP 2017 2018) ;;
    Run3|run3) expanded=(2022 2022EE 2023 2023BPix) ;;
    Run2+3|run2+3|all) expanded=(2016preVFP 2016postVFP 2017 2018 2022 2022EE 2023 2023BPix) ;;
    2016preVFP|2016postVFP|2017|2018|2022|2022EE|2023|2023BPix) expanded=("$selection") ;;
    *) echo "[ERROR] Unknown era: $selection" >&2; exit 2 ;;
  esac
  for era in "${expanded[@]}"; do
    case "$era" in 2016preVFP|2016postVFP|2017|2018) ;; *) echo "[ERROR] SKFlatAnalyzer supports Run-2 eras only" >&2; exit 2 ;; esac
    if [[ " ${Eras[*]} " != *" $era "* ]]; then Eras+=("$era"); fi
  done
done
flags=()
IFS=',' read -r -a selections <<< "$flag_selection"
for selection in "${selections[@]}"; do
  case "$selection" in
    nominal|Nominal) expanded=("") ;;
    all) expanded=("" "RunSyst" "RunXSecSyst") ;;
    RunSyst|RunXSecSyst|MuonIDEfficiency|TriggerEfficiency|ConvenerStudy|DYValidationDRStudy) expanded=("$selection") ;;
    *) echo "[ERROR] Unknown flag: $selection" >&2; exit 2 ;;
  esac
  for flag in "${expanded[@]}"; do
    if $top_pt_reweight; then
      case "$flag" in
        ""|RunSyst|RunXSecSyst) flag="TopPtReweight${flag:+,$flag}" ;;
        *) echo "[ERROR] TopPtReweight supports nominal, RunSyst and RunXSecSyst only" >&2; exit 2 ;;
      esac
    fi
    duplicate=false
    for existing in "${flags[@]}"; do [[ "$existing" == "$flag" ]] && duplicate=true; done
    $duplicate || flags+=("$flag")
  done
done
if (( ${#Eras[@]} == 0 || ${#flags[@]} == 0 )); then echo "[ERROR] Empty era/mode selection" >&2; exit 2; fi

submit_job() {
  local logfile="$1"
  shift
  if $dry_run; then
    printf '[dry-run] '
    printf '%q ' "$@"
    printf '\n'
  else
    "$@" &> "$logfile" &
  fi
}

if ! $dry_run; then
  mkdir -p log
  make || exit $?
fi

is_efficiency_mode() {
  local flag="$1"
  [[ "$flag" == "MuonIDEfficiency" || "$flag" == "TriggerEfficiency" ]]
}

is_systematic_mode() {
  local flag="$1"
  [[ "$flag" == "RunSyst" || "$flag" == "RunXSecSyst" ]]
}

is_diagnostic_mode() {
  local flag="$1"
  [[ "$flag" == "ConvenerStudy" || "$flag" == "DYValidationDRStudy" ]]
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

      # Submit only the POWHEG ttbar samples for the optional study.
      # Keep all ordinary production branches below unchanged.
      if [[ ",$flag," == *",TopPtReweight,"* ]]; then
        if $RUN_MC; then
          if [[ -n "$batchname" ]]; then cmd_common+=(--batchname "$batchname"); fi
          if $dry_run; then
            echo "[dry-run] ttbar sample list from $mcset:"
            awk '/^[[:space:]]*TT(JJ|LJ|LL)_powheg([_[:alnum:]]*)[[:space:]]*$/ {print "  " $1}' "$mcset"
          fi
          submit_job "log/submit_${era}_ttbar_${trig}__${flag}.log" \
            "${cmd_common[@]}" --userflags "$flag" "${extra_args[@]}" \
            -l <(awk '/^[[:space:]]*TT(JJ|LJ|LL)_powheg([_[:alnum:]]*)[[:space:]]*$/ {print $1}' "$mcset")
          echo "[SKFlat.py] Run ttbar TopPtReweight study: $flag era: $era"
          disown -a
          if ! $dry_run; then sleep 10; fi
        fi
        continue
      fi

      mcset_this="$mcset"
      if [[ "$flag" == "RunSyst" ]] || [[ "$flag" == "RunXSecSyst" ]]; then
        mcset_this="$systset"
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
        elif is_diagnostic_mode "$flag"; then
          echo " - mode: NIsoDimuon convener diagnostic study"
          echo " - output: NIsoMuon/$flag/$era/"
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
        submit_job "log/submit_${era}_${dataset}_${trig}${flag:+__${flag}}.log" "${cmd_common[@]}" -i "$dataset"
        echo "[SKFlat.py] Run analyzer for data: $dataset trigger: $trig era: $era"
      else
        echo "[SKFlat.py] Do not make DATA samples"
      fi

      if $RUN_MC; then
        submit_job "log/submit_${era}_MC_${trig}${flag:+__${flag}}.log" "${cmd_common[@]}" -l "$mcset_this"
        echo "[SKFlat.py] Run analyzer for MC backgrounds: $mcset_this trigger: $trig era: $era"
      else
        echo "[SKFlat.py] Do not make MC samples"
      fi

      # Signal MC supports nominal, RunSyst, and RunXSecSyst production.
      # The dedicated efficiency modes remain background/data-only.
      if $RUN_SIG && ! is_efficiency_mode "$flag" && [[ "$flag" != "RunXSecSyst" ]]; then
        cmd_signal=("${cmd_common[@]}")
        cmd_signal[8]=1
        submit_job "log/submit_${era}_sig_${trig}${flag:+__${flag}}.log" "${cmd_signal[@]}" -l "$signalset"
        echo "[SKFlat.py] Run analyzer for signals: $signalset trigger: $trig era: $era"
      else
        echo "[SKFlat.py] Do not make signal samples"
      fi

      disown -a
      if ! $dry_run; then sleep 10; fi

    done
  done
done


)
