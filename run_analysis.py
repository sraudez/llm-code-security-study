#!/usr/bin/env python3
"""
Batch Semgrep Analysis Script
-------------------------------
Scans all generated .c files across all task directories,
runs Semgrep on each one, and logs all findings to a CSV.

Usage:
    python run_analysis.py

Output:
    analysis/results.csv  — one row per finding
    analysis/summary.csv  — one row per file (vulnerability rate overview)

Directory structure expected:
    tasks/
        task01_read_user_input/
            chatgpt/
                minimal_1.c
                minimal_2.c
                detailed_1.c
                detailed_2.c
                secure_1.c
                secure_2.c
            claude/
                minimal_1.c
                minimal_2.c
                detailed_1.c
                detailed_2.c
                secure_1.c
                secure_2.c
        task02_string_copy/
            chatgpt/
                ...
            claude/
                ...
"""

import subprocess
import json
import os
import csv
import sys
from datetime import datetime

# ─────────────────────────────────────────
# CONFIGURATION — edit these if needed
# ─────────────────────────────────────────

TASKS_DIR = "tasks"                   # folder containing all task subfolders
OUTPUT_DIR = "analysis"               # folder where CSVs will be saved
SEMGREP_RULESET = "p/c"              # Semgrep ruleset — p/c covers C vulnerabilities
RESULTS_CSV = os.path.join(OUTPUT_DIR, "results.csv")
SUMMARY_CSV = os.path.join(OUTPUT_DIR, "summary.csv")

# Expected subfolder names inside each task folder
VALID_MODELS = ["Chatgpt", "Claude"]

# Expected file naming convention: {prompt_type}_{run}.c
# e.g. minimal_1.c, detailed_2.c, secure_1.c
VALID_PROMPT_TYPES = ["minimal", "detailed", "secure"]

# ─────────────────────────────────────────
# HELPER FUNCTIONS
# ─────────────────────────────────────────

def check_semgrep_installed():
    """Make sure Semgrep is available before running."""
    result = subprocess.run(
        ["semgrep", "--version"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("ERROR: Semgrep is not installed or not in PATH.")
        print("Install it with: pip install semgrep")
        sys.exit(1)
    print(f"Semgrep found: {result.stdout.strip()}")


def parse_filename(filename):
    """
    Parse prompt_type and run number from filename.
    Expected format: {prompt_type}_{run}.c
    e.g. minimal_1.c -> prompt_type=minimal, run=1

    Returns (prompt_type, run) or (None, None) if unrecognized.
    """
    name = filename.replace(".c", "")
    parts = name.split("_")

    if len(parts) < 2:
        return None, None

    prompt_type = parts[0]
    run = parts[1]

    if prompt_type not in VALID_PROMPT_TYPES:
        return None, None

    return prompt_type, run


def run_semgrep(filepath):
    """
    Run Semgrep on a single .c file.
    Returns a list of finding dicts, or empty list if none found.
    """
    result = subprocess.run(
        ["semgrep", "--config", SEMGREP_RULESET , "--json", filepath],
        capture_output=True,
        text=True
    )

    try:
        data = json.loads(result.stdout)
        return data.get("results", [])
    except json.JSONDecodeError:
        print(f"  WARNING: Could not parse Semgrep output for {filepath}")
        return []


def collect_c_files():
    """
    Walk the tasks directory and collect all .c files.
    Expected structure: tasks/taskXX_name/model/prompt_type_run.c

    Returns a list of (task_name, model, filename, filepath) tuples.
    """
    files = []

    if not os.path.isdir(TASKS_DIR):
        print(f"ERROR: Tasks directory '{TASKS_DIR}' not found.")
        print("Make sure you run this script from your project root folder.")
        sys.exit(1)

    for task_name in sorted(os.listdir(TASKS_DIR)):
        task_path = os.path.join(TASKS_DIR, task_name)

        if not os.path.isdir(task_path):
            continue

        for model in VALID_MODELS:
            model_path = os.path.join(task_path, model)

            if not os.path.isdir(model_path):
                print(f"  WARNING: Expected folder not found: {model_path}")
                continue

            for filename in sorted(os.listdir(model_path)):
                if filename.endswith(".c"):
                    filepath = os.path.join(model_path, filename)
                    files.append((task_name, model, filename, filepath))

    return files


# ─────────────────────────────────────────
# MAIN
# ─────────────────────────────────────────

def main():
    print("=" * 60)
    print("  Batch Semgrep Analysis")
    print(f"  Started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 60)

    # 1. Check Semgrep is installed
    check_semgrep_installed()
    print()

    # 2. Create output directory if it doesn't exist
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # 3. Collect all .c files
    c_files = collect_c_files()
    print(f"Found {len(c_files)} .c files across all task directories.\n")

    if len(c_files) == 0:
        print("No .c files found. Make sure your task folders contain generated code.")
        sys.exit(0)

    # 4. Run Semgrep on each file and collect results
    all_findings = []    # one row per vulnerability finding
    all_summaries = []   # one row per file

    for task_name, model, filename, filepath in c_files:
        prompt_type, run = parse_filename(filename)

        if prompt_type is None:
            print(f"  SKIPPED (unrecognized filename format): {filepath}")
            continue

        print(f"  Scanning: {task_name}/{model}/{filename} ...")
        findings = run_semgrep(filepath)

        # Summary row — one per file regardless of finding count
        all_summaries.append({
            "task":           task_name,
            "model":          model,
            "filename":       filename,
            "prompt_type":    prompt_type,
            "run":            run,
            "vulnerable":     "YES" if len(findings) > 0 else "NO",
            "finding_count":  len(findings),
        })

        # Detail rows — one per individual finding
        if findings:
            for f in findings:
                all_findings.append({
                    "task":        task_name,
                    "model":       model,
                    "filename":    filename,
                    "prompt_type": prompt_type,
                    "run":         run,
                    "rule_id":     f.get("check_id", "unknown"),
                    "severity":    f.get("extra", {}).get("severity", "unknown"),
                    "message":     f.get("extra", {}).get("message", "").replace("\n", " "),
                    "line":        f.get("start", {}).get("line", ""),
                })
            print(f"    → {len(findings)} finding(s) flagged")
        else:
            print(f"    → No findings")

    print()

    # 5. Write results.csv — detailed findings
    if all_findings:
        with open(RESULTS_CSV, "w", newline="") as f:
            fieldnames = ["task", "model", "filename", "prompt_type",
                          "run", "rule_id", "severity", "message", "line"]
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            writer.writerows(all_findings)
        print(f"Detailed findings saved to: {RESULTS_CSV}")
    else:
        print("No findings to write to results.csv")

    # 6. Write summary.csv — one row per file
    with open(SUMMARY_CSV, "w", newline="") as f:
        fieldnames = ["task", "model", "filename", "prompt_type",
                      "run", "vulnerable", "finding_count"]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(all_summaries)
    print(f"Summary saved to:           {SUMMARY_CSV}")

    # 7. Print a quick stats overview to the terminal
    print()
    print("=" * 60)
    print("  QUICK STATS")
    print("=" * 60)

    total_files = len(all_summaries)
    total_vulnerable = sum(1 for r in all_summaries if r["vulnerable"] == "YES")
    total_findings = sum(r["finding_count"] for r in all_summaries)

    print(f"  Total files scanned:    {total_files}")
    print(f"  Files with findings:    {total_vulnerable}")
    print(f"  Total findings:         {total_findings}")
    print()

    # Break down vulnerability rate by prompt type
    print("  Vulnerability rate by prompt type:")
    for ptype in VALID_PROMPT_TYPES:
        subset = [r for r in all_summaries if r["prompt_type"] == ptype]
        if subset:
            vuln_count = sum(1 for r in subset if r["vulnerable"] == "YES")
            rate = (vuln_count / len(subset)) * 100
            print(f"    {ptype:<12} {vuln_count}/{len(subset)} files vulnerable  ({rate:.0f}%)")

    print()

    # Break down vulnerability rate by model
    print("  Vulnerability rate by model:")
    for model in VALID_MODELS:
        subset = [r for r in all_summaries if r["model"] == model]
        if subset:
            vuln_count = sum(1 for r in subset if r["vulnerable"] == "YES")
            rate = (vuln_count / len(subset)) * 100
            print(f"    {model:<12} {vuln_count}/{len(subset)} files vulnerable  ({rate:.0f}%)")

    print()
    print(f"  Finished: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 60)


if __name__ == "__main__":
    main()