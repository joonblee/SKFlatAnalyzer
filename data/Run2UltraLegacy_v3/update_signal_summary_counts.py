#!/usr/bin/env python3

from pathlib import Path
import argparse

ERAS = ["2018", "2017", "2016preVFP", "2016postVFP"]


def tokens(line):
    """Compare lines by whitespace-separated tokens."""
    return line.strip().lstrip("#").split()


def parse_signal_summary(path):
    """
    Return mapping:
      sample -> {"new": new_line, "old_tokens": old_tokens, "new_tokens": new_tokens}
    """
    active = {}
    commented = {}

    with path.open() as f:
        for raw in f:
            line = raw.rstrip("\n")
            if not line.strip():
                continue

            toks = tokens(line)
            if len(toks) < 6:
                continue

            sample = toks[0]

            if not sample.startswith("Zp_M-") or "_hw7" not in sample:
                continue

            if line.lstrip().startswith("#"):
                commented[sample] = toks
            else:
                active[sample] = line.strip()

    mapping = {}

    for sample, new_line in active.items():
        if sample not in commented:
            print(f"WARNING: {path}: no commented old line found for {sample}; skip")
            continue

        mapping[sample] = {
            "new": new_line,
            "old_tokens": commented[sample],
            "new_tokens": tokens(new_line),
        }

    return mapping


def update_file(path, mapping, apply=False):
    if not path.exists():
        print(f"WARNING: missing file: {path}")
        return 0, 0

    lines = path.read_text().splitlines()
    new_lines = []
    n_update = 0
    n_warning = 0

    for line in lines:
        stripped = line.strip()

        if not stripped or stripped.startswith("#"):
            new_lines.append(line)
            continue

        toks = tokens(line)
        if len(toks) < 6:
            new_lines.append(line)
            continue

        sample = toks[0]

        if sample not in mapping:
            new_lines.append(line)
            continue

        old_tokens = mapping[sample]["old_tokens"]
        new_tokens = mapping[sample]["new_tokens"]
        new_line = mapping[sample]["new"]

        if toks == new_tokens:
            new_lines.append(line)
            continue

        if toks == old_tokens:
            new_lines.append(new_line)
            n_update += 1
            print(f"UPDATE: {path}: {sample}")
            continue

        print(f"WARNING: {path}: {sample} is neither old nor new; not modified")
        print(f"  current: {line}")
        print(f"  expected old: {' '.join(old_tokens)}")
        print(f"  new: {new_line}")
        new_lines.append(line)
        n_warning += 1

    if apply and n_update > 0:
        path.write_text("\n".join(new_lines) + "\n")

    return n_update, n_warning


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--apply",
        action="store_true",
        help="actually modify files. Without this option, only dry-run is performed.",
    )
    args = parser.parse_args()

    total_update = 0
    total_warning = 0

    for era in ERAS:
        print(f"\n=== {era} ===")

        summary_path = Path(f"./SampleSummary_signal_{era}.txt")
        if not summary_path.exists():
            print(f"WARNING: missing signal summary file: {summary_path}")
            continue

        mapping = parse_signal_summary(summary_path)

        if not mapping:
            print(f"WARNING: no usable Zp_M lines found in {summary_path}")
            continue

        # 1. Update ./<ERA>/Sample/SampleSummary_MC.txt
        mc_path = Path(f"./{era}/Sample/SampleSummary_MC.txt")
        n_update, n_warning = update_file(mc_path, mapping, apply=args.apply)
        total_update += n_update
        total_warning += n_warning

        # 2. Update ./<ERA>/Sample/CommonSampleInfo/Zp_M-*_Pt-*to*_hw7.txt
        common_dir = Path(f"./{era}/Sample/CommonSampleInfo")

        for sample in sorted(mapping):
            common_path = common_dir / f"{sample}.txt"
            n_update, n_warning = update_file(common_path, {sample: mapping[sample]}, apply=args.apply)
            total_update += n_update
            total_warning += n_warning

    print("\n=== Summary ===")
    print(f"updates:  {total_update}")
    print(f"warnings: {total_warning}")

    if not args.apply:
        print("\nDry-run only. Run with --apply to actually modify files.")


if __name__ == "__main__":
    main()
