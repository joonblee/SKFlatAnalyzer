"""PyROOT integration check using synthetic RunSyst output (not physics validation)."""
import csv
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

import ROOT
ROOT.gROOT.SetBatch(True)

REPO = Path(__file__).resolve().parents[1]
SCRIPT = REPO / "script/NIsoMuon/plot_ps_weights.py"
spec = importlib.util.spec_from_file_location("psplot", SCRIPT)
plotter = importlib.util.module_from_spec(spec)
spec.loader.exec_module(plotter)


class PSPlotsTest(unittest.TestCase):
    def test_ratios_and_output(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            source = tmp / "toy.root"
            output = ROOT.TFile(str(source), "RECREATE")
            region = "OS_POGMedium_tight_BJet{}_NIsoDimuon"
            # Negative nominal event weights, empty bins and a zero PS weight.
            events = [(1., 2., [2., .5, 1.1, .9]),
                      (3., -.5, [.5, 2., .8, 1.2]),
                      (7., 3., [0., 1., 1.3, .7])]
            suffixes = ["", plotter.REFERENCE] + [v[0] for v in plotter.VARIATIONS]
            for index, suffix in enumerate(suffixes):
                directory = region.format(suffix)
                output.mkdir(directory).cd()
                hist = ROOT.TH1D("Lepton_0_Pt___" + directory, "", 10, 0., 10.)
                hist.Sumw2()
                for x, weight, ps in events:
                    hist.Fill(x, weight * (ps[index - 2] if index >= 2 else 1.))
                if suffix == "":
                    hist.Fill(1., 10.)  # An event without PS information.
                hist.Write()
                output.cd()
            output.mkdir("PSWeightDiagnostics").cd()
            status = ROOT.TH1D("Status", "", 3, -.5, 2.5)
            status.SetBinContent(1, 3)
            status.SetBinContent(2, 1)
            status.Write()
            output.Close()

            result = subprocess.run(
                [sys.executable, str(SCRIPT), str(source), "--output", str(tmp / "plots"),
                 "--formats", "png", "--rebin", "1"],
                text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            self.assertEqual(result.returncode, 0, result.stdout)
            self.assertIn("PS-valid subset", result.stdout)
            with (tmp / "plots/toy/summary.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual(len(rows), 4)
            for i, row in enumerate(rows):
                expected = sum(w * ps[i] for _, w, ps in events)
                self.assertAlmostEqual(float(row["nominal_ps_valid"]), 4.5)
                self.assertAlmostEqual(float(row["nominal_all_events"]), 14.5)
                self.assertAlmostEqual(float(row["varied"]), expected)
                self.assertAlmostEqual(float(row["yield_ratio"]), expected / 4.5)
            images = list((tmp / "plots").rglob("*.png"))
            self.assertEqual(len(images), 2)
            self.assertTrue(all(image.stat().st_size > 1000 for image in images))

    def test_combined_overlay_and_stat_band(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            source = tmp / "combined.root"
            output = ROOT.TFile(str(source), "RECREATE")
            definitions = plotter.VARIATIONS + plotter.ME_OVERLAY
            for suffix in ["", plotter.REFERENCE] + [v[0] for v in definitions]:
                region = "OS_BJet" + suffix
                output.mkdir(region).cd()
                hist = ROOT.TH1D("Dilepton_Mass___" + region, "", 150, 0., 150.)
                hist.Sumw2()
                hist.Fill(20., 2.)
                hist.Fill(20., -1.)
                hist.Fill(30., 3.)
                hist.Write()
                output.cd()
            output.Close()
            result = subprocess.run(
                [sys.executable, str(SCRIPT), str(source), "--me-input", str(source),
                 "--output", str(tmp / "plots"), "--formats", "png"],
                text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            self.assertEqual(result.returncode, 0, result.stdout)
            with (tmp / "plots/combined/PSAndMEScale/summary.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual(len(rows), 6)
            self.assertTrue(all(float(row["yield_ratio"]) == 1. for row in rows))
            self.assertEqual(len(list((tmp / "plots").rglob("*.png"))), 2)
        hist = ROOT.TH1D("stat_test", "", 2, 0., 2.)
        hist.Sumw2()
        hist.Fill(.5, 2.)
        hist.Fill(.5, -1.)
        band = plotter.statistical_band(ROOT, hist, 2, ratio=True)
        self.assertEqual(band.GetN(), 1)  # Empty denominator omitted.
        self.assertAlmostEqual(band.GetErrorY(0), 5. ** .5)
        other = hist.Clone("mismatched_nominal")
        other.SetBinContent(1, 2.)
        with self.assertRaises(ValueError):
            plotter.check_same_nominal(hist, other)

    def test_scale_reference_and_output(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            source = tmp / "scale.root"
            output = ROOT.TFile(str(source), "RECREATE")
            for i in range(-1, 9):
                suffix = "" if i == -1 else "_Syst_PDFScale" + str(i)
                region = "OS_BJet" + suffix
                output.mkdir(region).cd()
                hist = ROOT.TH1D("Dilepton_Mass___" + region, "", 150, 0., 150.)
                # A different scale0 value tests that central is the denominator.
                hist.Fill(20., 10. if i == -1 else 20. + i)
                hist.Write()
                output.cd()
            output.Close()
            for extra, expected_count in (([], 6), (["--scale-all"], 8)):
                result = subprocess.run(
                    [sys.executable, str(SCRIPT), str(source), "--systematic", "scale",
                     "--output", str(tmp / "plots"), "--formats", "png", "--rebin", "1"] + extra,
                    text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                self.assertEqual(result.returncode, 0, result.stdout)
                with (tmp / "plots/scale/MEScale/summary.csv").open() as handle:
                    rows = list(csv.DictReader(handle))
                self.assertEqual(len(rows), expected_count)
                for row in rows:
                    index = int(row["variation"].replace("_Syst_PDFScale", ""))
                    self.assertAlmostEqual(float(row["yield_ratio"]), (20. + index) / 10.)
                self.assertEqual(len(list((tmp / "plots/scale/MEScale").rglob("*.png"))), 2)

    def test_variable_mass_rebin_preserves_weights(self):
        hist = ROOT.TH1D("mass_rebin_test", "", 7500, 0., 150.)
        hist.Sumw2()
        for mass, weight in [(5., 4.), (10.2, 2.), (10.4, -1.), (79., 3.), (90., 5.)]:
            hist.Fill(mass, weight)
        result = plotter.rebin_histograms([hist], "Dilepton_Mass", 0)[0]
        self.assertAlmostEqual(result.Integral(), 13.)
        self.assertAlmostEqual(sum(result.GetBinError(i) ** 2
                                   for i in range(1, result.GetNbinsX() + 1)), 55.)
        for mass, width in [(10.5, 1.), (25., 2.), (35., 3.), (50., 4.), (79., 5.)]:
            self.assertEqual(result.GetBinWidth(result.FindBin(mass)), width)
        self.assertAlmostEqual(result.GetBinError(result.FindBin(10.5)), 5. ** .5)

    def test_binning_and_rebin_checks(self):
        a = ROOT.TH1D("a", "", 10, 0., 10.)
        b = ROOT.TH1D("b", "", 10, 0., 20.)
        with self.assertRaises(ValueError):
            plotter.check_binning(a, b)
        with self.assertRaises(ValueError):
            plotter.rebin_factor("Lepton_0_Pt", 10, 3)

    def test_missing_and_corrupt_templates(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = str(Path(tmp) / "bad.root")
            source = ROOT.TFile(path, "RECREATE")
            hist = ROOT.TH1D("bad", "", 10, 0., 10.)
            hist.SetBinContent(1, float("nan"))
            hist.Write()
            with self.assertRaises(ValueError):
                plotter.load_histogram(source, "absent", "clone")
            with self.assertRaises(ValueError):
                plotter.load_histogram(source, "bad", "clone")
            source.Close()


if __name__ == "__main__":
    unittest.main()
