#!/usr/bin/env python3
"""Plot NIsoMuon PS or hard-process scale templates, with yield and unit-area ratios."""

import argparse
from array import array
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


# POWHEG ttbar UL LHE IDs 1001..1009; factors relative to nominal.
SCALE_FACTORS = ((1, 1), (1, 2), (1, .5), (2, 1), (2, 2),
                 (2, .5), (.5, 1), (.5, 2), (.5, .5))


def scale_variations(include_antipodal=False):
    indices = (1, 2, 3, 4, 5, 6, 7, 8) if include_antipodal else (1, 2, 3, 4, 6, 8)
    colours = (632, 632, 600, 416 + 2, 800 + 7, 600, 880, 416 + 2)
    return tuple(("_Syst_PDFScale" + str(i),
                  "ME (#mu_{R}, #mu_{F}) = (%g, %g)" % SCALE_FACTORS[i],
                  colours[i - 1], 1 if i in (1, 3, 4, 5) else 2) for i in indices)


ME_OVERLAY = (
    ("_Syst_PDFScale8", "ME (#mu_{R}, #mu_{F}) = (0.5, 0.5)", 416 + 2, 1),
    ("_Syst_PDFScale4", "ME (#mu_{R}, #mu_{F}) = (2, 2)", 416 + 2, 2),
)


def check_same_nominal(reference, other):
    check_binning(reference, other)
    for i in range(reference.GetNbinsX() + 2):
        for getter in ("GetBinContent", "GetBinError"):
            if not math.isclose(getattr(reference, getter)(i), getattr(other, getter)(i),
                                rel_tol=1.e-5, abs_tol=1.e-7):
                raise ValueError("PS and ME nominal templates differ: use the same samples, "
                                 "era, selection and PS-valid event coverage")


def statistical_band(ROOT, nominal, last, ratio=False):
    band = ROOT.TGraphErrors()
    for i in range(1, last + 1):
        value = nominal.GetBinContent(i)
        if ratio and value == 0:
            continue
        point = band.GetN()
        band.SetPoint(point, nominal.GetBinCenter(i), 1. if ratio else value)
        band.SetPointError(point, nominal.GetBinWidth(i) / 2.,
                           nominal.GetBinError(i) / abs(value) if ratio else nominal.GetBinError(i))
    band.SetFillColor(920)
    band.SetFillStyle(1001)
    band.SetLineColor(920)
    return band


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
        raise ValueError("Missing 1D template: " + path)
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



def rebin_histograms(histograms, observable, requested):
    if observable != "Dilepton_Mass" or requested:
        factor = rebin_factor(observable, histograms[0].GetNbinsX(), requested)
        for hist in histograms:
            hist.Rebin(factor)
        return histograms
    # Widths 1, 2, 3, 5, 10 GeV; outside bins retain full-range integrals.
    mass_edges = (list(range(10, 21)) + list(range(22, 31, 2))
                  + list(range(33, 46, 3)) + list(range(50, 61, 5))
                  + [70, 80])
    axis = histograms[0].GetXaxis()
    xmin, xmax = axis.GetXmin(), axis.GetXmax()
    edges = sorted(set([xmin, xmax] + [float(e) for e in mass_edges if xmin <= e <= xmax]))
    for edge in edges:
        index = axis.FindFixBin(edge)
        if not math.isclose(axis.GetBinLowEdge(index), edge, abs_tol=1.e-7):
            raise ValueError("Mass rebin edge does not align with input binning: " + str(edge))
    result = []
    for i, hist in enumerate(histograms):
        rebinned = hist.Rebin(len(edges) - 1, "mass_rebinned_" + str(i), array("d", edges))
        rebinned.SetDirectory(0)
        result.append(rebinned)
    return result


def symmetrise_ps(histograms, definitions):
    nominal = histograms[0]
    templates = {definition[0]: hist for definition, hist
                 in zip(definitions, histograms[1:])}
    for shower in ("FSR", "ISR"):
        down = templates.get("_Syst_PS" + shower + "MuRDown")
        up = templates.get("_Syst_PS" + shower + "MuRUp")
        if down is None or up is None:
            continue
        for i in range(1, nominal.GetNbinsX() + 1):
            centre = nominal.GetBinContent(i)
            old_down, old_up = down.GetBinContent(i), up.GetBinContent(i)
            delta = (abs(old_up - centre) + abs(old_down - centre)) / 2.
            # MuR labels describe scales, so retain their relative response direction.
            direction = 1. if old_up >= old_down else -1.
            up.SetBinContent(i, centre + direction * delta)
            down.SetBinContent(i, centre - direction * delta)

def draw(ROOT, histograms, observable, title, destination, mode, formats,
         variations=VARIATIONS):
    histograms = [h.Clone("display_" + str(i)) for i, h in enumerate(histograms)]
    for hist in histograms:
        hist.SetDirectory(0)
    integrals = [h.Integral() for h in histograms]
    if mode == "shape":
        if any(value <= 0 for value in integrals):
            print("[SKIP] Unit-area plot needs positive integrals:", observable)
            return False
        for hist, integral in zip(histograms, integrals):
            # Propagate the shared unit-area denominator (independent bin sums).
            variance_sum = sum(hist.GetBinError(i) ** 2
                               for i in range(1, hist.GetNbinsX() + 1))
            errors = []
            for i in range(1, hist.GetNbinsX() + 1):
                value, variance = hist.GetBinContent(i), hist.GetBinError(i) ** 2
                errors.append(math.sqrt(max(0., variance * (integral - value) ** 2
                                            + value ** 2 * (variance_sum - variance)))
                              / integral ** 2)
            hist.Scale(1.0 / integral)
            for i, error in enumerate(errors, 1):
                hist.SetBinError(i, error)

    # Symmetrise display clones after rebinning and any shape normalisation.
    # Do not renormalise again, as that would spoil binwise symmetry.
    symmetrise_ps(histograms, variations)
    if observable == "Dilepton_Mass":
        for hist in histograms:
            hist.Scale(1., "width")

    canvas = ROOT.TCanvas("ps_canvas", "", 900, 850)
    upper = ROOT.TPad("upper", "", 0, 0.31, 1, 1)
    lower = ROOT.TPad("lower", "", 0, 0, 1, 0.31)
    for pad in (upper, lower):
        pad.SetLeftMargin(0.13)
        pad.SetRightMargin(0.04)
        pad.Draw()
    upper.SetLogy(True)
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
    first = 1
    if observable == "Dilepton_Mass":
        # Display 10 <= m_mumu < 80 GeV; retain full-range normalisation.
        first = max(1, nominal.GetXaxis().FindFixBin(10.0))
        last = min(last, nominal.GetXaxis().FindFixBin(80.0) - 1)
    if last < first:
        canvas.Close()
        print("[SKIP] No bins in display range:", observable)
        return False
    visible = [h.GetBinContent(i) for h in histograms for i in range(first, last + 1)]
    positive = [value for value in visible if value > 0]
    if not positive:
        canvas.Close()
        print("[SKIP] Log-y plot needs positive bins:", observable)
        return False
    ymax = max(max(positive), max(nominal.GetBinContent(i) + nominal.GetBinError(i)
                                  for i in range(first, last + 1)))
    nominal.SetTitle("")
    nominal.SetLineColor(1)
    nominal.SetLineWidth(2)
    nominal.GetXaxis().SetRange(first, last)
    nominal.GetXaxis().SetLabelSize(0)
    unit = "GeV" if observable == "Dilepton_Mass" else "bin"
    nominal.GetYaxis().SetTitle(("Weighted events / " if mode == "yield" else "Fraction / ") + unit)
    nominal.SetMinimum(min(positive) * 0.3)
    nominal.SetMaximum(ymax * 30.)
    nominal.Draw("HIST")
    upper_band = statistical_band(ROOT, nominal, last)
    upper_band.Draw("2 SAME")
    nominal.Draw("HIST SAME")
    legend = ROOT.TLegend(0.49, 0.85 - 0.043 * (len(variations) + 2), 0.94, 0.85)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.028)
    legend.AddEntry(nominal, "Nominal", "l")
    legend.AddEntry(upper_band, "MC stat.", "f")
    for hist, (_, label, colour, style) in zip(histograms[1:], variations):
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
    graphs = []
    for hist in histograms[1:]:
        graph = ROOT.TGraph()
        graph.SetLineColor(hist.GetLineColor())
        graph.SetLineStyle(hist.GetLineStyle())
        graph.SetLineWidth(2)
        for i in range(first, last + 1):
            denominator = nominal.GetBinContent(i)
            if denominator == 0:
                continue  # Undefined ratios are omitted, never filled with 1.
            ratio = hist.GetBinContent(i) / denominator
            graph.SetPoint(graph.GetN(), nominal.GetBinCenter(i), ratio)
        graphs.append(graph)
    frame.SetMinimum(0.5)
    frame.SetMaximum(1.5)
    frame.Draw("AXIS")
    ratio_band = statistical_band(ROOT, nominal, last, ratio=True)
    ratio_band.Draw("2 SAME")
    line = ROOT.TLine(nominal.GetBinLowEdge(first), 1.,
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
    systematic = getattr(args, "systematic", "ps")
    is_scale = systematic == "scale"
    reference = "_Syst_PDFScale0" if is_scale else REFERENCE
    definitions = scale_variations(getattr(args, "scale_all", False)) if is_scale else VARIATIONS
    source = ROOT.TFile.Open(str(filename), "READ")
    if not source or source.IsZombie():
        raise ValueError("Cannot open " + str(filename))
    me_source = None
    me_input = getattr(args, "me_input", None)
    try:
        if me_input:
            me_source = ROOT.TFile.Open(str(me_input), "READ")
            if not me_source or me_source.IsZombie():
                raise ValueError("Cannot open ME input " + str(me_input))
            definitions = VARIATIONS + ME_OVERLAY
        status = source.Get("PSWeightDiagnostics/Status") if not is_scale else None
        counts = [status.GetBinContent(i) for i in (1, 2, 3)] if status else None
        print("[INPUT]", filename, "systematic =", systematic, "PS usable/missing/empty =", counts)
        if counts and counts[1] + counts[2] > 0:
            print("[WARNING] PS plots cover only the PS-valid subset of this input")
        paths = [p for p in histogram_paths(source) if reference in p]
        if not paths:
            message = "No selected " + systematic + " reference histograms in " + str(filename)
            if args.allow_empty:
                print("[SKIP]", message)
                return 0
            raise ValueError(message)
        output = args.output / filename.stem
        if is_scale:
            output = output / "MEScale"
        elif me_input:
            output = output / "PSAndMEScale"
        rows, plotted = [], 0
        for path in paths:
            observable = path.rsplit("/", 1)[-1].split("___", 1)[0]
            if args.hist and not any(pattern in observable for pattern in args.hist):
                continue
            # Scale mode uses the actual central template as its denominator.
            nominal_path = path.replace(reference, "") if is_scale else path
            nominal = load_histogram(source, nominal_path, "nominal")
            if me_source:
                me_central = load_histogram(me_source, path.replace(reference, ""), "me_central")
                check_same_nominal(nominal, me_central)
            variations = [load_histogram(
                me_source if me_source and suffix.startswith("_Syst_PDFScale") else source,
                path.replace(reference, suffix), suffix) for suffix, _, _, _ in definitions]
            for hist in variations:
                check_binning(nominal, hist)
            all_hists = [nominal] + variations
            central = source.Get(path.replace(reference, ""))
            nominal_yield = nominal.Integral()
            for hist, (suffix, _, _, _) in zip(variations, definitions):
                value = hist.Integral()
                rows.append({
                    "histogram": path, "variation": suffix,
                    "nominal_reference": nominal_yield,
                    "nominal_ps_valid": nominal_yield if not is_scale else "", "varied": value,
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
            all_hists = rebin_histograms(all_hists, observable, args.rebin)
            region = path.split("/", 1)[0].replace(reference, "")
            destination = output / region / observable
            destination.parent.mkdir(parents=True, exist_ok=True)
            title = (args.label or filename.stem) + " | " + region
            for mode in ("yield", "shape"):
                plotted += draw(ROOT, all_hists, observable, title, destination,
                                mode, args.formats, definitions)
        if not rows:
            raise ValueError("No histograms matched --hist in " + str(filename))
        with (output / "summary.csv").open("w", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=list(rows[0]))
            writer.writeheader()
            writer.writerows(rows)
        print("[DONE]", plotted, "comparison plots:", output)
        return plotted
    finally:
        if me_source:
            me_source.Close()
        source.Close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("inputs", nargs="+", type=Path, help="Merged RunSyst (PS) or RunXSecSyst (scale) ROOT files")
    parser.add_argument("--output", type=Path, default=Path("plots/PSWeights"))
    parser.add_argument("--systematic", choices=("ps", "scale"), default="ps",
                        help="PS weights or POWHEG ttbar UL ME scale weights")
    parser.add_argument("--me-input", type=Path,
                        help="Matched RunXSecSyst file: overlay ME (0.5,0.5), (2,2) with PS")
    parser.add_argument("--scale-all", action="store_true",
                        help="Include the two antipodal ME scale points (8 variations)")
    parser.add_argument("--label", default="", help="Sample/era title")
    parser.add_argument("--hist", action="append", help="Observable substring (repeatable)")
    parser.add_argument("--rebin", type=int, default=0, help="0: observable-dependent defaults")
    parser.add_argument("--formats", nargs="+", choices=("pdf", "png"), default=["pdf", "png"])
    parser.add_argument("--allow-empty", action="store_true",
                        help="Skip samples with no selected PS histograms (batch postprocessing)")
    args = parser.parse_args()
    if args.rebin < 0:
        parser.error("--rebin must be non-negative")
    if args.me_input and (args.systematic != "ps" or len(args.inputs) != 1):
        parser.error("--me-input requires one PS input and --systematic ps")
    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    for filename in args.inputs:
        plot_file(ROOT, filename, args)


if __name__ == "__main__":
    main()
