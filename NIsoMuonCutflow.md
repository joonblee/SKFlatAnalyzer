# NIsoMuon cutflow and DY jet-veto-map study

Run from the repository root after the usual environment setup.

```bash
bash run.sh --era Run2 --flags Cutflow,JetVetoMapStudy --dry-run
bash run.sh --era Run2 --cutflow --memory 4096
bash run.sh --era Run2 --jet-veto-map-study --memory 8192
```

`--cutflow` (or `cutflow`) submits the signal list alone. The new modes use
pre-skim inputs and their own output directories. Each submission has a
unique run-log directory. If a merge fails, the per-job ROOT files remain in
that directory. For a larger Condor memory request, pass
`--memory 8192` (in MiB) to `run.sh`. Each era's signal list
includes all 13 masses and the corresponding hard-process pT slices.

On TAMSA, both Run 2 studies publish merged ROOT files under
`/data6/Users/$USER/SKOutput/Run2UL_v3_Run3_v13/NIsoMuon/<study>/<era>/`.
This location takes precedence over `SKFlatOutputV` and `-o` for these studies.
Each rerun submits the full selected sample list. Successful merges replace
the corresponding final ROOT files, including the 2017 DYJets10to50 output.
Finish the rerun before collecting the campaign with SKPlotMaker.

`Cutflow/Reco` contains the cumulative, nominal generator-normalised weights.
The bin centres are 0 (input), 1 (trigger and event quality), 2 (two separated
tight AK4 jets), 3 (DeepJet Medium tag), 4 (OS muon kinematics and Medium ID),
5 (both muons in the other jet, with the nominal pair mass floor), and 6
(nominal candidate in the 11–80 GeV fit range). `Cutflow/RecoNominalYield`
has the fully corrected yield at step 6 for the nominal template check.

SKFlat signal files expose generator particles and reconstructed jets. A
particle-level jet collection with hadron flavour is required for the
fiducial denominator. The SKFlat output gives the reco selection cutflow
relative to the GEN-filtered input sample.

`--jet-veto-map-study` submits the two nominal DY samples alone. The two
branches of the study run on the same input events and use the nominal
trigger, cleaning filters, candidate selection and scale-factor prescriptions.
The map-on branch removes flagged jets before candidate selection. The
map-off branch passes the uncleaned collection to the same selection.
The mass spectra are `JetVetoMap/BJet_on`, `JetVetoMap/BJet_off`,
`JetVetoMap/LightJet_on` and `JetVetoMap/LightJet_off`. Add the nominal DY
slices within each era before computing the absolute yield and on/off ratio.
For Run-level yield per luminosity, divide the sum of the era yields by the
sum of their luminosities.
