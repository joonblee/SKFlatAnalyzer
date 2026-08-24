#!/bin/bash

input_list="skim_root_list.txt"

src_base="/gv0/DATA/SKFlat/Run2UltraLegacy_v3"
out_base="/data6/Users/joonblee/SKFlatAnalyzer/data/Run2UltraLegacy_v3"

eras=(2016preVFP 2016postVFP 2017 2018)

if [[ ! -f "$input_list" ]]; then
    echo "[ERROR] Input list not found: $input_list" >&2
    exit 1
fi

echo "[INFO] Reading paths directly from: $input_list"
echo "[INFO] No filesystem search will be performed."

# 기존 sample-list 삭제
for era in "${eras[@]}"; do
    out_dir="${out_base}/${era}/Sample/ForSNU"
    mkdir -p "$out_dir"

    rm -f "${out_dir}"/SkimTree_NIsoMuon_Zp_M-*_Pt-*_hw7.txt
done

current_dir=""
count=0
skipped=0

while IFS= read -r line || [[ -n "$line" ]]; do

    # CRLF file인 경우 마지막 carriage return 제거
    line="${line%$'\r'}"

    # 빈 줄 무시
    [[ -z "$line" ]] && continue

    # Directory line:
    # 2018/MC_SkimTree_NIsoMuon/.../2026_07_25_225131:
    if [[ "$line" == *: ]]; then
        current_dir="${line%:}"
        continue
    fi

    # ROOT filename이 아닌 줄은 무시
    [[ "$line" != *.root ]] && continue

    if [[ -z "$current_dir" ]]; then
        echo "[WARNING] ROOT filename without directory: $line" >&2
        ((skipped += 1))
        continue
    fi

    relative_path="${current_dir}/${line}"

    # 2026_07_25_* directory만 허용
    if [[ "$relative_path" =~ ^(2016preVFP|2016postVFP|2017|2018)/MC_SkimTree_NIsoMuon/Zp_M-([0-9]+)_Pt-([0-9]+)to([0-9]+)_hw7/2026_07_25_[^/]+/[^/]+\.root$ ]]; then

        era="${BASH_REMATCH[1]}"
        mass="${BASH_REMATCH[2]}"
        pt_low="${BASH_REMATCH[3]}"
        pt_high="${BASH_REMATCH[4]}"

        rootfile="${src_base}/${relative_path}"

        out_file="${out_base}/${era}/Sample/ForSNU/SkimTree_NIsoMuon_Zp_M-${mass}_Pt-${pt_low}to${pt_high}_hw7.txt"

        printf '%s\n' "$rootfile" >> "$out_file"

        ((count += 1))

        if (( count % 25 == 0 )); then
            echo "[PROGRESS] $count ROOT paths written"
        fi
    else
        echo "[SKIP] $relative_path"
        ((skipped += 1))
    fi

done < "$input_list"

echo
echo "============================================================"
echo "[DONE] ROOT paths written : $count"
echo "[DONE] Entries skipped    : $skipped"
echo "============================================================"

for era in "${eras[@]}"; do
    n_lists=$(
        find "${out_base}/${era}/Sample/ForSNU" \
            -maxdepth 1 \
            -type f \
            -name 'SkimTree_NIsoMuon_Zp_M-*_Pt-*_hw7.txt' |
        wc -l
    )

    echo "[SUMMARY] $era: $n_lists sample-list files"
done
