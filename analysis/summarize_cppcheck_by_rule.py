#!/usr/bin/env python3
"""
Summarize cppcheck_results.csv by (task, rule_id, prompt_type, model),
excluding noisy cppcheck categories. Only severity warning or error rows
are included.

Usage (from project root):
    python analysis/summarize_cppcheck_by_rule.py
"""

from __future__ import annotations

import argparse
import csv
from collections import Counter, defaultdict
from pathlib import Path


IGNORE_RULE_IDS = frozenset(
    {
        "unusedFunction",
        "staticFunction",
        "missingInclude",
        "missingIncludeSystem",
        "checkersReport",
        "normalCheckLevelMaxBranches",
        "constParameterPointer",
        "constVariable",
        "constVariablePointer",
        "syntaxError",
    }
)

PROMPT_ORDER = {"minimal": 0, "detailed": 1, "secure": 2}
INCLUDE_SEVERITIES = frozenset({"error", "warning"})


def worst_severity(severity_counts: Counter) -> str:
    if severity_counts.get("error", 0) > 0:
        return "error"
    if severity_counts.get("warning", 0) > 0:
        return "warning"
    return "unknown"


def file_key(row: dict[str, str]) -> str:
    """Unique source file within a (task, rule_id, prompt_type, model) bucket."""
    return row.get("filename", "") or ""


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-i",
        "--input",
        type=Path,
        default=Path("analysis/cppcheck_results.csv"),
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("analysis/cppcheck_summary_by_rule.csv"),
    )
    parser.add_argument(
        "--no-write",
        action="store_true",
        help="Print summary only, do not write CSV",
    )
    args = parser.parse_args()

    input_path: Path = args.input
    if not input_path.is_file():
        raise SystemExit(f"Input not found: {input_path.resolve()}")

    stats: dict[tuple[str, str, str, str], dict] = defaultdict(
        lambda: {
            "count": 0,
            "severity": Counter(),
            "files": set(),
            "example_message": "",
        }
    )

    total_rows = 0
    ignored_rule = 0
    ignored_severity = 0

    with input_path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        required = {
            "task",
            "model",
            "filename",
            "prompt_type",
            "rule_id",
            "severity",
            "message",
        }
        missing = required - set(reader.fieldnames or ())
        if missing:
            raise SystemExit(f"CSV missing columns: {sorted(missing)}")

        for row in reader:
            total_rows += 1
            rule_id = (row.get("rule_id") or "").strip()
            if rule_id in IGNORE_RULE_IDS:
                ignored_rule += 1
                continue

            sev = (row.get("severity") or "").strip().lower() or "unknown"
            if sev not in INCLUDE_SEVERITIES:
                ignored_severity += 1
                continue

            task = (row.get("task") or "").strip() or "unknown"
            prompt_type = (row.get("prompt_type") or "").strip() or "unknown"
            model = (row.get("model") or "").strip() or "unknown"
            key = (task, rule_id, prompt_type, model)
            bucket = stats[key]
            bucket["count"] += 1
            bucket["severity"][sev] += 1
            bucket["files"].add(file_key(row))
            if not bucket["example_message"]:
                bucket["example_message"] = (
                    (row.get("message") or "").replace("\n", " ").strip()
                )

    def sort_key(k: tuple[str, str, str, str]) -> tuple:
        task, rule_id, prompt_type, model = k
        po = PROMPT_ORDER.get(prompt_type, 99)
        return (task, rule_id, po, model.lower(), -stats[k]["count"])

    ordered = sorted(stats.keys(), key=sort_key)

    print(f"Input: {input_path.resolve()}")
    print(
        f"Rows: {total_rows} | ignored rule_id: {ignored_rule} | "
        f"ignored severity: {ignored_severity} | summary rows: {len(ordered)}"
    )
    print()

    hdr = (
        f"{'task':<28} {'rule_id':<26} {'prompt':<8} {'model':<10} "
        f"{'sev':<8} {'cnt':>4} {'files':>5}"
    )
    print(hdr)
    print("-" * len(hdr))
    for task, rid, ptype, model in ordered:
        b = stats[(task, rid, ptype, model)]
        sev = b["severity"]
        ws = worst_severity(sev)
        print(
            f"{task:<28} {rid:<26} {ptype:<8} {model:<10} {ws:<8} "
            f"{b['count']:>4} {len(b['files']):>5}"
        )

    if args.no_write:
        return

    out_path: Path = args.output
    out_path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = (
        "task",
        "rule_id",
        "prompt_type",
        "model",
        "severity",
        "count",
        "distinct_files",
        "error",
        "warning",
        "style",
        "information",
        "unknown",
        "example_message",
    )
    with out_path.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        for task, rid, ptype, model in ordered:
            b = stats[(task, rid, ptype, model)]
            sev = b["severity"]
            w.writerow(
                {
                    "task": task,
                    "rule_id": rid,
                    "prompt_type": ptype,
                    "model": model,
                    "severity": worst_severity(sev),
                    "count": b["count"],
                    "distinct_files": len(b["files"]),
                    "error": sev.get("error", 0),
                    "warning": sev.get("warning", 0),
                    "style": sev.get("style", 0),
                    "information": sev.get("information", 0),
                    "unknown": sev.get("unknown", 0),
                    "example_message": b["example_message"],
                }
            )
    print()
    print(f"Wrote: {out_path.resolve()}")


if __name__ == "__main__":
    main()
