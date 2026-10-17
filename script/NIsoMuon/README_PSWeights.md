# NIsoMuon PS-weight comparisons

`RunSyst` fills four independent ISR/FSR templates in the existing OS BJet
NIsoDimuon category, for every ordinary analysis histogram (dimuon mass/pT,
both muons' pT/eta/phi and both jets' pT/eta/phi). The selection, object momenta,
luminosity normalisation and nominal corrections are identical to central.
Each variation multiplies the nominal event weight by one stored relative PS
weight. There is no second division by `gen_weight`, no clipping and no
per-template yield normalisation in the analyser.

## Verified producer convention

Reference: CMSSNU/SKFlatMaker commit
[`cc83aaf`](https://github.com/CMSSNU/SKFlatMaker/tree/cc83aafc60ecb6fea1e068b49cc48befdacc5e7b).

| `weight_PSSyst` index | UL18 ttbar generator index | Variation | Histogram suffix |
|---|---|---|---|
| 0 | 4 | FSR mu_R x 0.5 | `_Syst_PSFSRMuRDown` |
| 1 | 5 | FSR mu_R x 2 | `_Syst_PSFSRMuRUp` |
| 2 | 26 | ISR mu_R x 0.5 | `_Syst_PSISRMuRDown` |
| 3 | 27 | ISR mu_R x 2 | `_Syst_PSISRMuRUp` |

Up/Down refers to the scale, not the event yield or radiation amount.
This is **not** the NanoAOD `PSWeight` ordering.

Evidence in that repository:

- `SKFlatMaker/script/Weight/parseLog.py`: selects the four named variations
  in the above order.
- `SKFlatMaker/script/Weight/data/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8__RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2.txt`:
  `PSSyst 4,5,26,27`.
- The matching file under `script/Weight/logs/` lists the generator index names.
- `SKFlatMaker/src/SKFlatMaker.cc`, `fillGENInfo`: stores
  `genEvtInfo->weights().at(id) / gen_weight`.

The current producer documents this convention; a custom production with a
different ordering needs its own mapping. Vector length alone cannot certify it.
`SKFlatNtuple` already binds this branch and `SkimTree_NIsoMuon` clones it.

## Run and plot

After the usual `source setup.sh`, run:

```bash
bash run.sh RunSyst
```

This builds the analyser and submits the existing Run2Syst sample list for the
configured eras and trigger. The positional argument overrides the `flags`
array. Data is excluded. Each completed sample is merged by `SKFlat.py`, then
the PS plotter runs automatically via `--ps-plots`.

ROOT output remains under
`$SKFlatOutputDir/$SKFlatOutputV/NIsoMuon/RunSyst/<era>/`.
Plots are written below that directory in
`PSWeightPlots/<ROOT-file-stem>/<region>/`, with a separate sample log under
`PSWeightPlots/`. A plotting failure is reported there and leaves ROOT output
available for a retry. Merge failures stop postprocessing and retain job files.

To redraw an existing merged output, without resubmitting:

```bash
python3 script/NIsoMuon/plot_ps_weights.py \
  "$SKFlatOutputDir/$SKFlatOutputV/NIsoMuon/RunSyst/2018/Skim_NIsoMuon_TTLJ_powheg.root" \
  --output plots/PSWeights --label 'TTLJ_powheg 2018'
```

Multiple input files are plotted separately. To compare combined ttbar, first
`hadd` the TTJJ, TTLJ and TTLL RunSyst outputs and pass the merged file.
Do not merge nominal-production files into RunSyst outputs, since RunSyst
already includes its own central histograms.

All available observables are plotted by default; use e.g.
`--hist Lepton_ --hist Dilepton_Mass` to select a subset. Default rebinning is
variable 1–5 GeV for dimuon mass (see below), 20 GeV for lepton pT, 0.2 for lepton eta/phi, and a factor
of 2 otherwise. `--rebin N` overrides this and must divide each histogram's bin
count. Both PDF and PNG are produced by default (`--formats pdf` for PDF only).

Each observable has two comparisons, each with four variation/nominal curves:

- `*_yield`: original weighted yields, preserving rate and shape changes.
- `*_shape`: each distribution divided by its full regular-bin integral.

`summary.csv` records nominal and varied yields and yield ratios before
rebinning, plus underflow/overflow contents and PS input counts. Integrals and
shape normalisation exclude underflow/overflow. Empty-denominator ratio bins
are omitted; unit-area plots with non-positive integrals are explicitly skipped.
No independent-sample statistical error is assigned to the correlated ratio
curves. Finite MC statistics still affect these curves, especially sparse tails.

## Missing weights and matched reference

`PSWeightDiagnostics/Status` counts input events before analysis cuts:
bin 1 = valid four weights, bin 2 = branch missing, bin 3 = empty/unavailable vector.
Missing/empty weights produce a warning and no PS fill for that event; the
ordinary central and experimental templates are still filled. Unexpected
non-empty vector lengths or non-finite weights stop the job.

`_Syst_PSNominal` is a central-weight reference filled on exactly the same
PS-valid events as the four variations. The plotter uses it as the denominator,
so missing weights cannot fake a yield shift. Its yield and the all-event
central yield are both recorded in the CSV. Partial PS coverage is reported
explicitly and must be resolved before interpreting it as a full-sample
uncertainty. With complete weights the two references coincide. Samples with
no selected PS-valid events are explicitly skipped by automatic plotting.

## Checks

```bash
g++ -std=c++11 -IAnalyzers/include test/test_nisomuon_ps_weights.cpp -o /tmp/test_ps
/tmp/test_ps
python3 test/test_nisomuon_ps_plotter.py
```

The ROOT test uses synthetic events with negative weights, zero weights, empty
bins and incomplete PS coverage. It checks numerical yield ratios and image
creation. Production validation still requires running NIsoMuon with the
site's CMSSW environment, corrections and actual ntuples.

## Hard-process scale comparisons

The legend calls the central curve `Nominal`. PS comparisons still use the
PS-valid event subset described above.

ME scale templates are produced by **RunXSecSyst**, not RunSyst. Merge the
TTJJ, TTLJ and TTLL files within the same era's RunXSecSyst directory, then run:

```bash
python3 script/NIsoMuon/plot_ps_weights.py /path/to/RunXSecSyst/2018/ttbar_scale.root \
  --systematic scale --output plots/PSWeights --label "ttbar 2018"
```

This produces separate ME scale figures in `ttbar_scale/MEScale/`, with the
same mass display below 80 GeV and ratio range 0–2. It does not combine ME and
PS uncertainties or overlay their curves. Yield and unit-area plots use the
central histogram from the same RunXSecSyst file as their reference.

The implemented mapping is for POWHEG ttbar UL, verified in the upstream
[2018 TTLJ production log](https://github.com/CMSSNU/SKFlatMaker/blob/master/SKFlatMaker/script/Weight/logs/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8__RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2.log).
Indices 0–8 correspond to relative (muR, muF) factors:
(1,1), (1,2), (1,0.5), (2,1), (2,2), (2,0.5), (0.5,1), (0.5,2), (0.5,0.5).
Default plots show indices 1,2,3,4,6,8 (the six variations in a 7-point
prescription). `--scale-all` additionally shows antipodal indices 5 and 7.
These are individual curves, not an envelope. Do not assume this index mapping
for other generators without checking their production metadata.

## Overlay PS and correlated ME scale variations

```bash
python3 script/NIsoMuon/plot_ps_weights.py /path/to/RunSyst/2018/ttbar_PS.root \
  --me-input /path/to/RunXSecSyst/2018/ttbar_scale.root \
  --output plots/PSWeights --label "ttbar 2018"
```

This draws one nominal, the four PS variations, and ME scale (muR,muF)
(0.5,0.5) and (2,2) on each figure. Results go under
`ttbar_PS/PSAndMEScale/`. Input files must contain identical sample mixtures,
eras and selections. The plotter checks PS-reference and ME-central bin
contents and statistical errors before overlaying; it rejects different
nominals, including incomplete PS event coverage. Do not hadd RunSyst and
RunXSecSyst files together, as that would double the central histogram.

All plotting modes show a grey nominal MC statistical band: the stored
histogram errors (sum of squared event weights) in the upper panel, and
sigma/abs(nominal) around one in the ratio panel. Zero-denominator bins are
omitted there. Unit-area statistical errors include the correlation between
a bin and the full regular-bin normalisation sum; the band uses the full
normalisation range even when the displayed mass range ends at 80 GeV.
No independent-template statistical errors are assigned to systematic curves.

## Mass display and variable binning

The upper panel uses log-y for all observables. Dimuon mass is displayed over
10–80 GeV with default widths:

| Mass interval (GeV) | Width (GeV) |
| --- | --- |
| 10–20 | 1 |
| 20–30 | 2 |
| 30–45 | 3 |
| 45–65 | 4 |
| 65–80 | 5 |

Mass curves and statistical errors are divided by bin width (events/GeV or
fraction/GeV); ratios are unchanged by this common division. Full input-range
integrals remain the basis of shape normalisation and CSV yields. Outside
mass bins are retained for that purpose but hidden. An explicit `--rebin`
overrides the default variable binning. Non-positive values cannot be shown
on log-y, but remain in the histogram calculations and linear ratio panel.
