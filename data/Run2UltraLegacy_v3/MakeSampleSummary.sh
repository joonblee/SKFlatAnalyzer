for file in /data9/Users/taehee_public/HerwigSampleInfo/2018/Sample/CommonSampleInfo/*; do
  if [[ -f $file ]]; then
    tail -n 1 "$file"
  fi
done
