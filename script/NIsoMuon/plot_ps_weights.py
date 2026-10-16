#!/usr/bin/env python3
"""Plot all NIsoMuon RunSyst PS templates, with yield and unit-area ratios."""

import argparse
import csv
import math
from pathlib import Path

REFERENCE = "_Syst_PSNominal"
VARIATIONS = (
    ("_Syst_PSFSRMuRDown", "FSR #mu_{R} #times 0.5", 632, 1),
    ("_Syst_PSFSRMuRUp", "FSR #mu_{R} #times 2", 632, 2),
    ("_Syst_PSISRMuRDown", "ISR #mu_{R} #times 0.5", 600, 1),
    ("_Syst_PSISRMuRUp", "ISR #mu_{R} #times 2", 600, 2),
)


def histogram_paths(directory, prefix=""):
    for name in sorted({key.GetName() for key in directory.GetListOfKeys()}):
        obj = directory.Get(name)
        path = prefix + name
        if obj.InheritsFrom("TDirectory"):
            yield from histogram_paths(obj, path + "/")
        elif obj.InheritsFrom("TH1") and obj.GetDimension() == 1:
            yield path


def load_histogram(source, path, name):
    obj = source.Get(path)
    if not obj or not obj.InheritsFrom("TH1") or obj.GetDimension() != 1:
        raise ValueError("Missing 1D PS template: " + path)
    hist = obj.Clone(name)
    hist.SetDirectory(0)
    for i in range(hist.GetNbinsX() + 2):
        if not (math.isfinite(hist.GetBinContent(i)) and
                math.isfinite(hist.GetBinError(i))):
            raise ValueError("Non-finite histogram content/error: " + path)
    return hist


def check_binning(reference, variation):
    if reference.GetNbinsX() != variation.GetNbinsX():
        raise ValueError("PS template bin counts differ")
    for i in range(1, reference.GetNbinsX() + 2):
        if reference.GetBinLowEdge(i) != variation.GetBinLowEdge(i):
            raise ValueError("PS template bin edges differ")


def axis_title(observable):
    if observable == "Dilepton_Mass":
        return "m_{#mu#mu} [GeV]"
    if observable == "Dilepton_pT":
        return "p_{T}(#mu#mu) [GeV]"
    fields = observable.split("_")
    if len(fields) == 3 and fields[0] in ("Lepton", "Jet"):
        particle = ("#mu_{" if fields[0] == "Lepton" else "j_{") + str(int(fields[1]) + 1) + "}"
        if fields[2] == "Pt":
            return "p_{T}(" + particle + ") [GeV]"
        if fields[2] in ("Eta", "Phi"):
            return "#" + fields[2].lower() + "(" + particle + ")"
    return observable


def rebin_factor(observable, nbins, requested):
    if requested:
        factor = requested
    elif observable == "Dilepton_Mass":
        factor = 50  # 0.02 GeV -> 1 GeV
    elif observable.startswith("Lepton_") and observable.endswith("_Pt"):
        factor = 2  # 10 GeV -> 20 GeV
    elif observable.startswith("Lepton_"):
        factor = 20  # eta/phi: 0.01 -> 0.2
    else:
        factor = 2
    if nbins % factor:
        if requested:
            raise ValueError("--rebin must divide every selected histogram's bin count")
        return 1
    return factor


def draw(ROOT, histograms, observable, title, destination, mode, formats):
    histograms = [h.Clone("display_" + str(i)) for i, h in enumerate(histograms)]
    for hist in histograms:
        hist.SetDirectory(0)
    integrals = [h.Integral() for h in histograms]
    if mode == "shape":
        if any(value <= 0 for value in integrals):
            print("[SKIP] Unit-area plot needs positive integrals:", observable)
            return False
        for hist, integral in zip(histograms, integrals):
            hist.Scale(1.0 / integral)

    canvas = ROOT.TCanvas("ps_canvas", "", 900, 850)
    upper = ROOT.TPad("upper", "", 0, 0.31, 1, 1)
    lower = ROOT.TPad("lower", "", 0, 0, 1, 0.31)
    for pad in (upper, lower):
        pad.SetLeftMargin(0.13)
        pad.SetRightMargin(0.04)
        pad.Draw()
    upper.SetTopMargin(0.13)
    upper.SetBottomMargin(0.025)
    lower.SetTopMargin(0.025)
    lower.SetBottomMargin(0.30)

    upper.cd()
    nominal = histograms[0]
    # Trim empty high-pT tails for display only; integrals use all regular bins.
    last = max((i for h in histograms for i in range(1, h.GetNbinsX() + 1)
                if h.GetBinContent(i) != 0), default=nominal.GetNbinsX())
    if "Pt" in observable or "pT" in observable:
        last = min(nominal.GetNbinsX(), last + 2)
    else:
        last = nominal.GetNbinsX()
    ymax = max(h.GetMaximum() for h in histograms)
    ymin = min(0., min(h.GetMinimum() for h in histograms))
    nominal.SetTitle("")
    nominal.SetLineColor(1)
    nominal.SetLineWidth(2)
    nominal.GetXaxis().SetRange(1, last)
    nominal.GetXaxis().SetLabelSize(0)
    nominal.GetYaxis().SetTitle("Weighted events / bin" if mode == "yield"
                                else "Fraction / bin")
    nominal.SetMinimum(ymin * 1.2)
    nominal.SetMaximum(ymax * 1.35 if ymax > 0 else 1.)
    nominal.Draw("HIST")
    legend = ROOT.TLegend(0.51, 0.62, 0.94, 0.85)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.028)
    legend.AddEntry(nominal, "Nominal (PS-valid events)", "l")
    for hist, (_, label, colour, style) in zip(histograms[1:], VARIATIONS):
        hist.SetLineColor(colour)
        hist.SetLineStyle(style)
        hist.SetLineWidth(2)
        hist.Draw("HIST SAME")
        legend.AddEntry(hist, label, "l")
    legend.Draw()
    text = ROOT.TLatex()
    text.SetNDC(True)
    text.SetTextSize(0.027)
    text.DrawLatex(0.13, 0.95, title)
    text.DrawLatex(0.13, 0.90, observable + " | " + mode)

    lower.cd()
    frame = nominal.Clone("ratio_frame")
    frame.Reset()
    frame.GetXaxis().SetLabelSize(0.09)
    frame.GetXaxis().SetTitleSize(0.10)
    frame.GetXaxis().SetTitle(axis_title(observable))
    frame.GetYaxis().SetLabelSize(0.08)
    frame.GetYaxis().SetTitleSize(0.085)
    frame.GetYaxis().SetTitleOffset(0.65)
    frame.GetYaxis().SetNdivisions(505)
    frame.GetYaxis().SetTitle("Variation / nominal")
    graphs, ratios = [], []
    for hist in histograms[1:]:
        graph = ROOT.TGraph()
        graph.SetLineColor(hist.GetLineColor())
        graph.SetLineStyle(hist.GetLineStyle())
        graph.SetLineWidth(2)
        for i in range(1, last + 1):
            denominator = nominal.GetBinContent(i)
            if denominator == 0:
                continue  # Undefined ratios are omitted, never filled with 1.
            ratio = hist.GetBinContent(i) / denominator
            graph.SetPoint(graph.GetN(), nominal.GetBinCenter(i), ratio)
            ratios.append(ratio)
        graphs.append(graph)
    low, high = min([0.9] + ratios), max([1.1] + ratios)
    margin = max(0.05, 0.08 * (high - low))
    frame.SetMinimum(low - margin)
    frame.SetMaximum(high + margin)
    frame.Draw("AXIS")
    line = ROOT.TLine(nominal.GetBinLowEdge(1), 1.,
                      nominal.GetBinLowEdge(last + 1), 1.)
    line.SetLineColor(920)
    line.Draw()
    # Same-event templates are correlated. Do not assign independent-histogram
    # TH1::Divide errors to these systematic-ratio curves.
    for graph in graphs:
        graph.Draw("L SAME")
    for extension in formats:
        canvas.SaveAs(str(destination) + "_" + mode + "." + extension)
    canvas.Close()
    return True


def plot_file(ROOT, filename, args):
    source = ROOT.TFile.Open(str(filename), "READ")
    if not source or source.IsZombie():
        raise ValueError("Cannot open " + str(filename))
    try:
        status = source.Get("PSWeightDiagnostics/Status")
        counts = [status.GetBinContent(i) for i in (1, 2, 3)] if status else None
        print("[INPUT]", filename, "PS usable/missing/empty =", counts)
        if counts and counts[1] + counts[2] > 0:
            print("[WARNING] PS plots cover only the PS-valid subset of this input")
        paths = [p for p in histogram_paths(source) if REFERENCE in p]
        if not paths:
            message = "No selected PS-reference histograms in " + str(filename)
            if args.allow_empty:
                print("[SKIP]", message)
                return 0
            raise ValueError(message)
        output = args.output / filename.stem
        rows, plotted = [], 0
        for path in paths:
            observable = path.rsplit("/", 1)[-1].split("___", 1)[0]
            if args.hist and not any(pattern in observable for pattern in args.hist):
                continue
            nominal = load_histogram(source, path, "nominal")
            variations = [load_histogram(source, path.replace(REFERENCE, suffix), suffix)
                          for suffix, _, _, _ in VARIATIONS]
            for hist in variations:
                check_binning(nominal, hist)
            all_hists = [nominal] + variations
            central = source.Get(path.replace(REFERENCE, ""))
            nominal_yield = nominal.Integral()
            for hist, (suffix, _, _, _) in zip(variations, VARIATIONS):
                value = hist.Integral()
                rows.append({
                    "histogram": path, "variation": suffix,
                    "nominal_ps_valid": nominal_yield, "varied": value,
                    "yield_ratio": value / nominal_yield if nominal_yield else "",
                    "nominal_all_events": central.Integral() if central else "",
                    "nominal_underflow": nominal.GetBinContent(0),
                    "nominal_overflow": nominal.GetBinContent(nominal.GetNbinsX() + 1),
                    "varied_underflow": hist.GetBinContent(0),
                    "varied_overflow": hist.GetBinContent(hist.GetNbinsX() + 1),
                    "input_ps_usable": counts[0] if counts else "",
                    "input_ps_missing": counts[1] if counts else "",
                    "input_ps_empty": counts[2] if counts else "",
                })
            factor = rebin_factor(observable, nominal.GetNbinsX(), args.rebin)
            for hist in all_hists:
                hist.Rebin(factor)
            region = path.split("/", 1)[0].replace(REFERENCE, "")
            destination = output / region / observable
            destination.parent.mkdir(parents=True, exist_ok=True)
            title = (args.label or filename.stem) + " | " + region
            for mode in ("yield", "shape"):
                plotted += draw(ROOT, all_hists, observable, title, destination,
                                mode, args.formats)
        if not rows:
            raise ValueError("No histograms matched --hist in " + str(filename))
        with (output / "summary.csv").open("w", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=list(rows[0]))
            writer.writeheader()
            writer.writerows(rows)
        print("[DONE]", plotted, "comparison plots:", output)
        return plotted
    finally:
        source.Close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("inputs", nargs="+", type=Path, help="Merged RunSyst ROOT files")
    parser.add_argument("--output", type=Path, default=Path("plots/PSWeights"))
    parser.add_argument("--label", default="", help="Sample/era title")
    parser.add_argument("--hist", action="append", help="Observable substring (repeatable)")
    parser.add_argument("--rebin", type=int, default=0, help="0: observable-dependent defaults")
    parser.add_argument("--formats", nargs="+", choices=("pdf", "png"), default=["pdf", "png"])
    parser.add_argument("--allow-empty", action="store_true",
                        help="Skip samples with no selected PS histograms (batch postprocessing)")
    args = parser.parse_args()
    if args.rebin < 0:
        parser.error("--rebin must be non-negative")
    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    for filename in args.inputs:
        plot_file(ROOT, filename, args)


if __name__ == "__main__":
    main()
