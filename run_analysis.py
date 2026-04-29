#!/usr/bin/env python3
"""
Batch Static Analysis Script (Semgrep + cppcheck)
--------------------------------------------------
Scans all generated .c files across all task directories,
runs both Semgrep and cppcheck on each one, and logs all
findings to CSV files.

Usage:
    python run_analysis.py

Output:
    analysis/semgrep_results.csv  — one row per Semgrep finding
    analysis/cppcheck_results.csv — one row per cppcheck finding
    analysis/summary.csv          — one row per file (both tools combined)

Directory structure expected:
    tasks/
        task01_read_username/
            chatgpt/
                minimal_1.c
                minimal_2.c
                detailed_1.c
                detailed_2.c
                secure_1.c
                secure_2.c
            claude/
                minimal_1.c
                ...
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
import xml.etree.ElementTree as ET
from datetime import datetime

# ─────────────────────────────────────────
# CONFIGURATION — edit these if needed
# ─────────────────────────────────────────

TASKS_DIR = "tasks"
OUTPUT_DIR = "analysis"
SEMGREP_RULESET = "p/c"

SEMGREP_RESULTS_CSV  = os.path.join(OUTPUT_DIR, "semgrep_results.csv")
CPPCHECK_RESULTS_CSV = os.path.join(OUTPUT_DIR, "cppcheck_results.csv")
SUMMARY_CSV          = os.path.join(OUTPUT_DIR, "summary.csv")

VALID_MODELS       = ["Chatgpt", "Claude"]
VALID_PROMPT_TYPES = ["minimal", "detailed", "secure"]

# ─────────────────────────────────────────
# INSTALL CHECKS
# ─────────────────────────────────────────

def check_semgrep_installed():
    result = subprocess.run(
        ["semgrep", "--version"],
        capture_output=True, text=True
    )
    if result.returncode != 0:
        print("ERROR: Semgrep is not installed.")
        print("Install it with: pip install semgrep")
        sys.exit(1)
    print(f"Semgrep found:   {result.stdout.strip()}")


def check_cppcheck_installed():
    result = subprocess.run(
        ["cppcheck", "--version"],
        capture_output=True, text=True
    )
    if result.returncode != 0:
        print("ERROR: cppcheck is not installed.")
        print("Install it with: brew install cppcheck")
        sys.exit(1)
    print(f"cppcheck found:  {result.stdout.strip()}")


# ─────────────────────────────────────────
# HELPER FUNCTIONS
# ─────────────────────────────────────────

def parse_filename(filename):
    """
    Parse prompt_type and run number from filename.
    Expected format: {prompt_type}_{run}.c
    e.g. minimal_1.c -> prompt_type=minimal, run=1
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


def collect_c_files():
    """
    Walk the tasks directory and collect all .c files.
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
# SEMGREP
# ─────────────────────────────────────────

def run_semgrep(filepath):
    """
    Run Semgrep on a single .c file.
    Returns a list of finding dicts, or empty list if none found.
    """
    result = subprocess.run(
        ["semgrep", "--config", SEMGREP_RULESET,
         "--no-git-ignore", "--json", filepath],
        capture_output=True, text=True
    )

    try:
        data = json.loads(result.stdout)
        return data.get("results", [])
    except json.JSONDecodeError:
        print(f"    WARNING: Could not parse Semgrep output for {filepath}")
        return []


# ─────────────────────────────────────────
# CPPCHECK
# ─────────────────────────────────────────

def run_cppcheck(filepath):
    """
    Run cppcheck on a single .c file using XML output for easy parsing.
    Returns a list of finding dicts, or empty list if none found.
    """
    result = subprocess.run(
        [
            "cppcheck",
            "--enable=all",         # enable all checks
            "--inconclusive",       # include inconclusive findings
            "--xml",                # output as XML for reliable parsing
            "--xml-version=2",
            filepath
        ],
        capture_output=True, text=True
    )

    # cppcheck writes XML to stderr, not stdout
    xml_output = result.stderr

    findings = []
    try:
        root = ET.fromstring(xml_output)
        for error in root.iter("error"):
            # skip nofile errors (informational messages)
            error_id = error.get("id", "")
            if error_id in ["missingIncludeSystem", "missingInclude"]:
                continue

            # get location info if available
            location = error.find("location")
            line = location.get("line", "") if location is not None else ""

            findings.append({
                "rule_id":  error_id,
                "severity": error.get("severity", "unknown"),
                "message":  error.get("msg", "").replace("\n", " "),
                "line":     line,
            })
    except ET.ParseError:
        print(f"    WARNING: Could not parse cppcheck output for {filepath}")

    return findings


# ─────────────────────────────────────────
# MAIN
# ─────────────────────────────────────────

def main():
    print("=" * 60)
    print("  Batch Static Analysis — Semgrep + cppcheck")
    print(f"  Started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 60)

    # 1. Check tools are installed
    check_semgrep_installed()
    check_cppcheck_installed()
    print()

    # 2. Create output directory
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # 3. Collect all .c files
    c_files = collect_c_files()
    print(f"Found {len(c_files)} .c files across all task directories.\n")

    if len(c_files) == 0:
        print("No .c files found. Make sure your task folders contain generated code.")
        sys.exit(0)

    # 4. Run both tools on each file
    all_semgrep_findings  = []
    all_cppcheck_findings = []
    all_summaries         = []

    for task_name, model, filename, filepath in c_files:
        prompt_type, run = parse_filename(filename)

        if prompt_type is None:
            print(f"  SKIPPED (unrecognized filename): {filepath}")
            continue

        print(f"  Scanning: {task_name}/{model}/{filename}")

        # --- Semgrep ---
        semgrep_findings = run_semgrep(filepath)
        for f in semgrep_findings:
            all_semgrep_findings.append({
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
        print(f"    Semgrep:   {len(semgrep_findings)} finding(s)")

        # --- cppcheck ---
        cppcheck_findings = run_cppcheck(filepath)
        for f in cppcheck_findings:
            all_cppcheck_findings.append({
                "task":        task_name,
                "model":       model,
                "filename":    filename,
                "prompt_type": prompt_type,
                "run":         run,
                "rule_id":     f["rule_id"],
                "severity":    f["severity"],
                "message":     f["message"],
                "line":        f["line"],
            })
        print(f"    cppcheck:  {len(cppcheck_findings)} finding(s)")

        # --- Summary row ---
        semgrep_vuln  = len(semgrep_findings) > 0
        cppcheck_vuln = len(cppcheck_findings) > 0
        either_vuln   = semgrep_vuln or cppcheck_vuln

        all_summaries.append({
            "task":                    task_name,
            "model":                   model,
            "filename":                filename,
            "prompt_type":             prompt_type,
            "run":                     run,
            "semgrep_findings":        len(semgrep_findings),
            "cppcheck_findings":       len(cppcheck_findings),
            "semgrep_vulnerable":      "YES" if semgrep_vuln  else "NO",
            "cppcheck_vulnerable":     "YES" if cppcheck_vuln else "NO",
            "either_tool_vulnerable":  "YES" if either_vuln   else "NO",
        })

    print()

    # 5. Write semgrep_results.csv
    if all_semgrep_findings:
        with open(SEMGREP_RESULTS_CSV, "w", newline="") as f:
            fieldnames = ["task", "model", "filename", "prompt_type",
                          "run", "rule_id", "severity", "message", "line"]
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            writer.writerows(all_semgrep_findings)
        print(f"Semgrep findings saved to:  {SEMGREP_RESULTS_CSV}")
    else:
        print("Semgrep: no findings across all files.")

    # 6. Write cppcheck_results.csv
    if all_cppcheck_findings:
        with open(CPPCHECK_RESULTS_CSV, "w", newline="") as f:
            fieldnames = ["task", "model", "filename", "prompt_type",
                          "run", "rule_id", "severity", "message", "line"]
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            writer.writerows(all_cppcheck_findings)
        print(f"cppcheck findings saved to: {CPPCHECK_RESULTS_CSV}")
    else:
        print("cppcheck: no findings across all files.")

    # 7. Write summary.csv
    with open(SUMMARY_CSV, "w", newline="") as f:
        fieldnames = ["task", "model", "filename", "prompt_type", "run",
                      "semgrep_findings", "cppcheck_findings",
                      "semgrep_vulnerable", "cppcheck_vulnerable",
                      "either_tool_vulnerable"]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(all_summaries)
    print(f"Summary saved to:           {SUMMARY_CSV}")

    # 8. Print quick stats
    print()
    print("=" * 60)
    print("  QUICK STATS")
    print("=" * 60)

    total = len(all_summaries)
    print(f"  Total files scanned:   {total}")
    print(f"  Semgrep findings:      {len(all_semgrep_findings)}")
    print(f"  cppcheck findings:     {len(all_cppcheck_findings)}")
    print()

    # By prompt type
    print("  Vulnerable files by prompt type (either tool):")
    for ptype in VALID_PROMPT_TYPES:
        subset = [r for r in all_summaries if r["prompt_type"] == ptype]
        if subset:
            vuln = sum(1 for r in subset if r["either_tool_vulnerable"] == "YES")
            rate = (vuln / len(subset)) * 100
            print(f"    {ptype:<12} {vuln}/{len(subset)}  ({rate:.0f}%)")

    print()

    # By model
    print("  Vulnerable files by model (either tool):")
    for model in VALID_MODELS:
        subset = [r for r in all_summaries if r["model"] == model]
        if subset:
            vuln = sum(1 for r in subset if r["either_tool_vulnerable"] == "YES")
            rate = (vuln / len(subset)) * 100
            print(f"    {model:<12} {vuln}/{len(subset)}  ({rate:.0f}%)")

    print()

    # Tool agreement
    both_flagged = sum(
        1 for r in all_summaries
        if r["semgrep_vulnerable"] == "YES" and r["cppcheck_vulnerable"] == "YES"
    )
    only_semgrep = sum(
        1 for r in all_summaries
        if r["semgrep_vulnerable"] == "YES" and r["cppcheck_vulnerable"] == "NO"
    )
    only_cppcheck = sum(
        1 for r in all_summaries
        if r["semgrep_vulnerable"] == "NO" and r["cppcheck_vulnerable"] == "YES"
    )
    print("  Tool agreement:")
    print(f"    Both tools flagged:       {both_flagged} files")
    print(f"    Only Semgrep flagged:     {only_semgrep} files")
    print(f"    Only cppcheck flagged:    {only_cppcheck} files")

    print()
    print(f"  Finished: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 60)


if __name__ == "__main__":
    main()