#!/usr/bin/env python3
"""Run-for-record mechanics: manifest, draw, verify, hash-record, unseal.

Python 3 stdlib only. See SKILL.md beside this file.
"""
import argparse
import hashlib
import json
import os
import random
import subprocess
import sys
import tempfile
from pathlib import Path

VERDICTS = ("PASS", "FAIL", "UNVERIFIED")


def sha256_file(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def git(path, *args):
    return subprocess.check_output(["git", "-C", str(path), *args], text=True).strip()


def repo_entry(path):
    path = Path(path)
    sha = git(path, "rev-parse", "HEAD")
    dirty = bool(git(path, "status", "--porcelain"))
    if dirty:
        sys.exit(f"refusing: {path} has uncommitted changes")
    return {"path": str(path), "sha": sha, "dirty": False}


def file_entry(path):
    return {"path": str(path), "sha256": sha256_file(path)}


def cmd_manifest(a):
    checkers = []
    for spec in a.checker:
        owner, _, p = spec.partition("=")
        if owner not in ("builder", "reviewer") or not p:
            sys.exit("--checker takes builder=<path> or reviewer=<path>")
        checkers.append({"owner": owner, **file_entry(p)})
    if {c["owner"] for c in checkers} != {"builder", "reviewer"}:
        sys.exit("manifest needs one builder and one reviewer checker")
    m = {
        "uut": repo_entry(a.uut),
        "harness": repo_entry(a.harness),
        "plan": {**file_entry(a.plan), "revision": a.plan_revision},
        "checkers": checkers,
        "menu": file_entry(a.menu),
        "bench": file_entry(a.bench),
    }
    Path(a.output).write_text(json.dumps(m, indent=2) + "\n")
    print(f"manifest {sha256_file(a.output)[:8]} -> {a.output}")


def verify_manifest(path):
    m = json.loads(Path(path).read_text())
    bad = []
    for key in ("uut", "harness"):
        e = m[key]
        if git(e["path"], "rev-parse", "HEAD") != e["sha"] or git(e["path"], "status", "--porcelain"):
            bad.append(f"{key}: HEAD or tree moved")
    files = [m["plan"], m["menu"], m["bench"], *m["checkers"]]
    for e in files:
        if not Path(e["path"]).is_file() or sha256_file(e["path"]) != e["sha256"]:
            bad.append(f"{e['path']}: hash moved or missing")
    return m, bad


def cmd_verify(a):
    _, bad = verify_manifest(a.manifest)
    if bad:
        print("\n".join(bad))
        sys.exit(1)
    print("manifest verified")


def load_menu(path):
    menu = json.loads(Path(path).read_text())
    pairs = []
    for knob in menu["knobs"]:
        if knob.get("expected", "FAIL") != "FAIL":
            sys.exit(f"knob {knob['name']}: planted faults must expect FAIL")
        for test in knob["applies_to"]:
            pairs.append((test, knob["name"]))
    if not pairs:
        sys.exit("fault menu has no (test, knob) pairs")
    return menu, pairs


def cmd_draw(a):
    m, bad = verify_manifest(a.manifest)
    if bad:
        sys.exit("\n".join(bad))
    menu, pairs = load_menu(m["menu"]["path"])
    tests = sorted({t for t, _ in pairs} | set(menu.get("tests", [])))
    if a.count > len({t for t, _ in pairs}):
        sys.exit("count exceeds the number of tests with knobs")
    rng = random.Random(f"{a.seed}:{sha256_file(a.manifest)}")
    chosen, used = [], set()
    for test, knob in rng.sample(pairs, len(pairs)):
        if test not in used:
            chosen.append((test, knob))
            used.add(test)
        if len(chosen) == a.count:
            break
    out = Path(a.output)
    out.mkdir(parents=True, exist_ok=True)
    faults = {"manifest_sha256": sha256_file(a.manifest), "faults": dict(sorted(chosen))}
    expectations = {t: ("FAIL" if t in used else "PASS") for t in tests}
    (out / "faults.json").write_text(json.dumps(faults, indent=2) + "\n")
    (out / "expectations.json").write_text(json.dumps(expectations, indent=2) + "\n")
    lines = [f"{sha256_file(out / n)}  {n}" for n in ("faults.json", "expectations.json")]
    (out / "sealed.sha256").write_text("\n".join(lines) + "\n")
    print(f"sealed {sha256_file(out / 'sealed.sha256')[:8]} ({len(chosen)} planted) -> {out}/")


def cmd_hash_record(a):
    rec = Path(a.record)
    files = sorted(p for p in rec.rglob("*") if p.is_file())
    if not files:
        sys.exit("record directory is empty")
    lines = [f"{sha256_file(p)}  {p.relative_to(rec)}" for p in files]
    out = Path(a.output or rec.parent / "record.sha256")
    out.write_text("\n".join(lines) + "\n")
    print(f"record {sha256_file(out)[:8]} ({len(files)} files) -> {out}")


def outcome(expected, runner, auditor):
    if runner != auditor:
        return "DIVERGENCE: human reads both"
    if expected == "FAIL":
        return "planted fault detected" if runner == "FAIL" else "RFR VOID: planted fault not detected"
    if runner == "PASS":
        return "nominal: spot-check"
    if runner == "UNVERIFIED":
        return "UNVERIFIED: record incomplete"
    return "REAL FAIL: human triage"


def cmd_unseal(a):
    sealed = Path(a.sealed)
    for line in (sealed / "sealed.sha256").read_text().splitlines():
        digest, name = line.split("  ", 1)
        if sha256_file(sealed / name) != digest:
            sys.exit(f"sealed/{name} was modified after the seal")
    expectations = json.loads((sealed / "expectations.json").read_text())
    runner = json.loads(Path(a.runner).read_text())
    auditor = json.loads(Path(a.auditor).read_text())
    if runner.get("record_sha256") != auditor.get("record_sha256"):
        sys.exit("runner and auditor verdicts are not about the same record")
    rows, triggers = [], []
    for test in sorted(set(expectations) | set(runner["tests"]) | set(auditor["tests"])):
        exp = expectations.get(test, "PASS")
        r = runner["tests"].get(test, {}).get("verdict", "MISSING")
        u = auditor["tests"].get(test, {}).get("verdict", "MISSING")
        o = outcome(exp, r, u)
        rows.append(f"| {test} | {exp} | {r} | {u} | {o} |")
        if not o.startswith(("nominal", "planted")):
            triggers.append(f"- {test}: {o}")
    text = ["# Unseal", "", f"Record: `{runner.get('record_sha256', '?')}`", "",
            "## Human triggers", *(triggers or ["- none"]), "",
            "## Table", "| Test | Expected | Runner | Auditor | Outcome |", "|---|---|---|---|---|", *rows, ""]
    Path(a.output).write_text("\n".join(text))
    print(f"unseal -> {a.output} ({len(triggers)} triggers)")


def cmd_selftest(_a):
    with tempfile.TemporaryDirectory() as d:
        d = Path(d)
        for name in ("uut", "harness"):
            r = d / name
            r.mkdir()
            (r / "f").write_text(name)
            subprocess.run(["git", "-C", r, "init", "-q"], check=True)
            subprocess.run(["git", "-C", r, "-c", "user.name=t", "-c", "user.email=t@t", "commit", "-q",
                            "--allow-empty", "-m", "x"], check=True)
            subprocess.run(["git", "-C", r, "add", "f"], check=True)
            subprocess.run(["git", "-C", r, "-c", "user.name=t", "-c", "user.email=t@t", "commit", "-q",
                            "-m", "y"], check=True)
        for name in ("plan.md", "a.py", "b.py", "bench.md"):
            (d / name).write_text(name)
        menu = {"tests": ["L1", "L2", "L3", "L4"],
                "knobs": [{"name": "k1", "applies_to": ["L1", "L2"], "expected": "FAIL"},
                          {"name": "k2", "applies_to": ["L3"], "expected": "FAIL"}]}
        (d / "menu.json").write_text(json.dumps(menu))
        argv = ["manifest", "--uut", str(d / "uut"), "--harness", str(d / "harness"), "--plan", str(d / "plan.md"),
                "--checker", f"builder={d / 'a.py'}", "--checker", f"reviewer={d / 'b.py'}",
                "--menu", str(d / "menu.json"), "--bench", str(d / "bench.md"), "-o", str(d / "manifest.json")]
        main(argv)
        main(["verify", str(d / "manifest.json")])
        main(["draw", "--manifest", str(d / "manifest.json"), "--seed", "captain", "--count", "2",
              "-o", str(d / "sealed")])
        exp = json.loads((d / "sealed" / "expectations.json").read_text())
        assert sorted(exp.values()).count("FAIL") == 2, exp
        main(["draw", "--manifest", str(d / "manifest.json"), "--seed", "captain", "--count", "2",
              "-o", str(d / "sealed2")])
        assert (d / "sealed" / "faults.json").read_text() == (d / "sealed2" / "faults.json").read_text()
        rec = d / "record"
        rec.mkdir()
        (rec / "events.csv").write_text("x")
        main(["hash-record", str(rec)])
        sha = sha256_file(d / "record.sha256")
        verd = {t: {"verdict": exp[t]} for t in exp}
        (d / "r.json").write_text(json.dumps({"record_sha256": sha, "tests": verd}))
        verd2 = dict(verd)
        verd2["L4"] = {"verdict": "FAIL"}
        (d / "u.json").write_text(json.dumps({"record_sha256": sha, "tests": verd2}))
        main(["unseal", "--sealed", str(d / "sealed"), "--runner", str(d / "r.json"), "--auditor", str(d / "u.json"),
              "-o", str(d / "unseal.md")])
        text = (d / "unseal.md").read_text()
        assert "DIVERGENCE" in text and text.count("planted fault detected") == 2, text
        (d / "plan.md").write_text("moved")
        _, bad = verify_manifest(d / "manifest.json")
        assert bad, "verify must detect a moved plan"
    print("selftest ok")


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = p.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("manifest", help="freeze UUT, harness, plan, checkers, menu, bench")
    s.add_argument("--uut", required=True)
    s.add_argument("--harness", required=True)
    s.add_argument("--plan", required=True)
    s.add_argument("--plan-revision", default="")
    s.add_argument("--checker", action="append", default=[], metavar="OWNER=PATH")
    s.add_argument("--menu", required=True)
    s.add_argument("--bench", required=True)
    s.add_argument("-o", "--output", default="manifest.json")
    s.set_defaults(fn=cmd_manifest)
    s = sub.add_parser("verify", help="refuse if anything on the manifest moved")
    s.add_argument("manifest")
    s.set_defaults(fn=cmd_verify)
    s = sub.add_parser("draw", help="human-seeded draw of planted faults")
    s.add_argument("--manifest", required=True)
    s.add_argument("--seed", required=True, help="known only to the human")
    s.add_argument("--count", type=int, default=3)
    s.add_argument("-o", "--output", default="sealed")
    s.set_defaults(fn=cmd_draw)
    s = sub.add_parser("hash-record", help="write record.sha256 for a record directory")
    s.add_argument("record")
    s.add_argument("-o", "--output")
    s.set_defaults(fn=cmd_hash_record)
    s = sub.add_parser("unseal", help="join expectations, runner and auditor verdicts")
    s.add_argument("--sealed", required=True)
    s.add_argument("--runner", required=True)
    s.add_argument("--auditor", required=True)
    s.add_argument("-o", "--output", default="unseal.md")
    s.set_defaults(fn=cmd_unseal)
    sub.add_parser("selftest", help="exercise every subcommand in a temp dir").set_defaults(fn=cmd_selftest)
    a = p.parse_args(argv)
    a.fn(a)


if __name__ == "__main__":
    main()
