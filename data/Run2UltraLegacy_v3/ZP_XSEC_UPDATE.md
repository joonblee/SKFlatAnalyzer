# Zp cross-section update

Date: 2026-08-24

The `xsec` values in `Zp_M-*_Pt-*_hw7.txt` were updated using:

```text
updated xsec = original xsec * (4 * pi)
```

This covers every available `Pt` bin for masses `M-12`, `M-15`, `M-20`,
`M-25`, `M-30`, `M-35`, `M-40`, `M-45`, `M-50`, `M-55`, `M-60`, `M-65`,
and `M-70` in `2016preVFP`, `2016postVFP`, `2017`, and `2018`.

In total, 428 `CommonSampleInfo` files were updated. Only the third column
(`xsec`) was changed; all other fields were preserved. For example:

```text
2016preVFP/Zp_M-15_Pt-100to110_hw7
139769.34999999998 -> 1756393.4526280819
```

The corresponding entries in each era's `Sample/SampleSummary_MC.txt`
were then synchronized with the complete updated `CommonSampleInfo` rows.
This refreshed 97 existing entries and added 10 previously missing `M-12`
entries per era: 107 entries per era and 428 entries overall.

All 428 `CommonSampleInfo` conversions were checked against the original
values. Every affected summary entry was also verified to occur exactly
once and to match its corresponding `CommonSampleInfo` row exactly.
