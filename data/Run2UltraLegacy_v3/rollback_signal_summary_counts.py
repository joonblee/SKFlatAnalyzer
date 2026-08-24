#!/usr/bin/env python3

from pathlib import Path
import argparse

ALL_ERAS = ["2018", "2017", "2016preVFP", "2016postVFP"]


def tokens(line):
    """Compare lines by whitespace-separated tokens."""
    return line.strip().lstrip("#").split()


def parse_signal_summary(path):
    """
    Return mapping:
      sample -> {
          "new_tokens": tokens of active line,
          "old": uncommented old line,
          "old_tokens": tokens of commented line
      }
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
                active[sample] = toks

    mapping = {}

    for sample, new_tokens in active.items():
        if sample not in commented:
            print(f"WARNING: {path}: no commented old line found for {sample}; skip")
            continue

        old_tokens = commented[sample]

        mapping[sample] = {
            "new_tokens": new_tokens,
            "old": " ".join(old_tokens),
            "old_tokens": old_tokens,
        }

    return mapping


def rollback_file(path, mapping, apply=False):
    if not path.exists():
        print(f"WARNING: missing file: {path}")
        return 0, 0

    lines = path.read_text().splitlines()
    new_lines = []

    n_rollback = 0
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

        new_tokens = mapping[sample]["new_tokens"]
        old_tokens = mapping[sample]["old_tokens"]
        old_line = mapping[sample]["old"]

        if toks == old_tokens:
            # Already rolled back.
            new_lines.append(line)
            continue

        if toks == new_tokens:
            new_lines.append(old_line)
            n_rollback += 1
            print(f"ROLLBACK: {path}: {sample}")
            continue

        print(f"WARNING: {path}: {sample} is neither current new nor old; not modified")
        print(f"  current: {line}")
        print(f"  expected new: {' '.join(new_tokens)}")
        print(f"  rollback old: {old_line}")
        new_lines.append(line)
        n_warning += 1

    if apply and n_rollback > 0:
        path.write_text("\n".join(new_lines) + "\n")

    return n_rollback, n_warning


def rollback_era(era, apply=False):
    print(f"\n=== Rollback {era} ===")

    summary_path = Path(f"./SampleSummary_signal_{era}.txt")

    if not summary_path.exists():
        print(f"WARNING: missing signal summary file: {summary_path}")
        return 0, 1

    mapping = parse_signal_summary(summary_path)

    if not mapping:
        print(f"WARNING: no usable Zp_M lines found in {summary_path}")
        return 0, 1

    total_rollback = 0
    total_warning = 0

    # 1. Rollback ./<ERA>/Sample/SampleSummary_MC.txt
    mc_path = Path(f"./{era}/Sample/SampleSummary_MC.txt")
    n_rollback, n_warning = rollback_file(mc_path, mapping, apply=apply)
    total_rollback += n_rollback
    total_warning += n_warning

    # 2. Rollback ./<ERA>/Sample/CommonSampleInfo/Zp_M-*_Pt-*to*_hw7.txt
    common_dir = Path(f"./{era}/Sample/CommonSampleInfo")

    for sample in sorted(mapping):
        common_path = common_dir / f"{sample}.txt"
        n_rollback, n_warning = rollback_file(
            common_path,
            {sample: mapping[sample]},
            apply=apply,
        )
        total_rollback += n_rollback
        total_warning += n_warning

    return total_rollback, total_warning


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--era",
        choices=ALL_ERAS + ["all"],
        default="all",
        help="era to rollback. Default: all",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="actually modify files. Without this option, only dry-run is performed.",
    )

    args = parser.parse_args()

    eras = ALL_ERAS if args.era == "all" else [args.era]

    total_rollback = 0
    total_warning = 0

    for era in eras:
        n_rollback, n_warning = rollback_era(era, apply=args.apply)
        total_rollback += n_rollback
        total_warning += n_warning

    print("\n=== Summary ===")
    print(f"rollbacks: {total_rollback}")
    print(f"warnings:  {total_warning}")

    if not args.apply:
        print("\nDry-run only. Run with --apply to actually modify files.")


if __name__ == "__main__":
    main()
