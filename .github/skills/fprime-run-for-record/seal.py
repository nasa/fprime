#!/usr/bin/env python3
"""Run-for-record mechanics: manifest, verify, draw, seal-id, hash-record,
verify-record, unseal. Python 3 stdlib only. See SKILL.md beside this file."""

import argparse
import getpass
import hashlib
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

VERDICTS = ("PASS", "FAIL", "UNVERIFIED")
EXPORTS = ("events.jsonl", "channels.jsonl", "commands.jsonl")


def sha256_file(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def sha256_text(text):
    return hashlib.sha256(text.encode()).hexdigest()


def git(path, *args):
    try:
        out = subprocess.run(
            ["git", "-C", str(path), *args], text=True, capture_output=True, check=True
        )
    except (OSError, subprocess.CalledProcessError) as e:
        sys.exit(f"git {' '.join(args)} failed in {path}: {e}")
    return out.stdout.strip()


def repo_entry(path):
    if git(path, "status", "--porcelain"):
        sys.exit(f"refusing: {path} has uncommitted or untracked files")
    return {"path": str(path), "sha": git(path, "rev-parse", "HEAD"), "dirty": False}


def file_entry(path):
    if not Path(path).is_file():
        sys.exit(f"missing file: {path}")
    return {"path": str(path), "sha256": sha256_file(path)}


def plan_tests(plan_path):
    ids = re.findall(r"^ID:\s+(\S+)", Path(plan_path).read_text(), re.MULTILINE)
    if not ids:
        sys.exit("plan has no 'ID: <test>' lines (fprime-system-test-design §3)")
    long = [i for i in ids if len(i) > 8]
    if long:
        sys.exit(f"test IDs longer than 8 chars do not fit a marker: {long}")
    return ids


def cmd_manifest(a):
    checkers = []
    for spec in a.checker:
        owner, _, p = spec.partition("=")
        if owner not in ("builder", "reviewer") or not p:
            sys.exit("--checker takes builder=<path> or reviewer=<path>")
        checkers.append({"owner": owner, **file_entry(p)})
    if {c["owner"] for c in checkers} != {"builder", "reviewer"}:
        sys.exit("manifest needs one builder and one reviewer checker")
    images = []
    for spec in a.image:
        dep, _, rest = spec.partition("=")
        p, _, build = rest.partition(":")
        if not dep or not p:
            sys.exit("--image takes <deployment>=<path>[:<build command>]")
        images.append({"deployment": dep, **file_entry(p), "build_cmd": build})
    if not images:
        sys.exit("manifest needs at least one --image")
    m = {
        "uut": repo_entry(a.uut),
        "harness": repo_entry(a.harness),
        "plan": {
            **file_entry(a.plan),
            "revision": a.plan_revision,
            "tests": plan_tests(a.plan),
        },
        "checkers": checkers,
        "images": images,
        "menu": file_entry(a.menu),
        "bench": file_entry(a.bench),
    }
    Path(a.output).write_text(json.dumps(m, indent=2) + "\n")
    print(
        f"manifest {sha256_file(a.output)[:8]} ({len(m['plan']['tests'])} tests) "
        f"-> {a.output}"
    )


def verify_manifest(path):
    m = json.loads(Path(path).read_text())
    bad = []
    for key in ("uut", "harness"):
        e = m[key]
        if git(e["path"], "rev-parse", "HEAD") != e["sha"]:
            bad.append(f"{key}: HEAD moved")
        if git(e["path"], "status", "--porcelain"):
            bad.append(f"{key}: tree dirty")
    for e in [m["plan"], m["menu"], m["bench"], *m["checkers"], *m["images"]]:
        if not Path(e["path"]).is_file() or sha256_file(e["path"]) != e["sha256"]:
            bad.append(f"{e['path']}: hash moved or missing")
    if plan_tests(m["plan"]["path"]) != m["plan"]["tests"]:
        bad.append("plan: test list changed")
    return m, bad


def cmd_verify(a):
    _, bad = verify_manifest(a.manifest)
    if bad:
        sys.exit("\n".join(bad))
    print("manifest verified")


def load_menu(path, tests):
    menu = json.loads(Path(path).read_text())
    pairs = []
    for knob in menu["knobs"]:
        if knob.get("expected", "FAIL") != "FAIL":
            sys.exit(f"knob {knob['name']}: planted faults must expect FAIL")
        if knob.get("class") not in ("harness-expectation", "uut-state", "stimulus"):
            sys.exit(
                f"knob {knob['name']}: class must be harness-expectation, "
                "uut-state or stimulus"
            )
        for test in knob["applies_to"]:
            if test not in tests:
                sys.exit(f"knob {knob['name']}: {test} is not a plan test")
            pairs.append((test, knob["name"]))
    if not pairs:
        sys.exit("fault menu has no (test, knob) pairs")
    return pairs


def read_seed(a):
    if a.seed_file:
        seed = Path(a.seed_file).read_text().strip()
    else:
        seed = getpass.getpass("seal seed (not echoed): ").strip()
    if len(seed) < 8:
        sys.exit("seed must be at least 8 characters")
    return seed


def cmd_draw(a):
    m, bad = verify_manifest(a.manifest)
    if bad:
        sys.exit("\n".join(bad))
    tests = m["plan"]["tests"]
    pairs = load_menu(m["menu"]["path"], tests)
    candidates = {t for t, _ in pairs}
    if len(candidates) < 2 * a.max_count:
        sys.exit(
            f"menu covers {len(candidates)} tests; need at least "
            f"{2 * a.max_count} for --max-count {a.max_count}"
        )
    seed = read_seed(a)
    msha = sha256_file(a.manifest)
    key = lambda tk: sha256_text(f"{seed}:{msha}:{tk[0]}:{tk[1]}")  # noqa: E731
    count = 1 + int(sha256_text(f"{seed}:{msha}:count"), 16) % a.max_count
    chosen, used = {}, set()
    for test, knob in sorted(pairs, key=key):
        if test not in used:
            chosen[test] = knob
            used.add(test)
        if len(chosen) == count:
            break
    run = Path(a.run_dir)
    sealed = run / "sealed"
    sealed.mkdir(parents=True, exist_ok=True)
    (sealed / "faults.json").write_text(
        json.dumps(
            {"manifest_sha256": msha, "faults": dict(sorted(chosen.items()))}, indent=2
        )
        + "\n"
    )
    (sealed / "expectations.json").write_text(
        json.dumps({t: ("FAIL" if t in used else "PASS") for t in tests}, indent=2)
        + "\n"
    )
    lines = [
        f"{sha256_file(sealed / n)}  {n}" for n in ("faults.json", "expectations.json")
    ]
    (sealed / "sealed.sha256").write_text("\n".join(lines) + "\n")
    seal_id = sha256_file(sealed / "sealed.sha256")
    (run / "seal-id.txt").write_text(seal_id + "\n")
    print(
        f"seal-id {seal_id[:8]} manifest {msha[:8]} -> {run}/sealed/ "
        f"(planted count hidden; keep the seed)"
    )


def seal_id_of(run_dir):
    return (Path(run_dir) / "seal-id.txt").read_text().strip()


def cmd_seal_id(a):
    print(seal_id_of(a.run_dir))


def record_lines(rec):
    files = sorted(p for p in rec.rglob("*") if p.is_file())
    return [f"{sha256_file(p)}  {p.relative_to(rec)}" for p in files]


def cmd_hash_record(a):
    rec = Path(a.run_dir) / "record"
    lines = record_lines(rec)
    if not lines:
        sys.exit("record directory is empty")
    out = Path(a.run_dir) / "record.sha256"
    out.write_text("\n".join(lines) + "\n")
    print(f"record {sha256_file(out)[:8]} ({len(lines)} files) -> {out}")


def verify_record(run_dir):
    run = Path(run_dir)
    rec = run / "record"
    bad = []
    expected = (run / "record.sha256").read_text().splitlines()
    if record_lines(rec) != expected:
        bad.append("record/ differs from record.sha256")
    counts_file = rec / "export-counts.json"
    if counts_file.is_file():
        counts = json.loads(counts_file.read_text())
        for name in EXPORTS:
            p = rec / name
            if name in counts and p.is_file():
                n = sum(1 for _ in open(p, "rb"))
                if n != counts[name]:
                    bad.append(
                        f"{name}: {n} rows exported, archive reports {counts[name]}"
                    )
    else:
        bad.append("record/export-counts.json missing (fprime-test-oracle §2)")
    return bad


def cmd_verify_record(a):
    bad = verify_record(a.run_dir)
    if bad:
        sys.exit("\n".join(bad))
    print("record verified")


def outcome(expected, runner, auditor):
    verdicts = {runner, auditor}
    if expected == "FAIL":
        if "PASS" in verdicts or "MISSING" in verdicts:
            return "RFR VOID: planted fault not detected"
        if verdicts == {"FAIL"}:
            return "planted fault detected"
        return "DIVERGENCE on planted fault: human reads both"
    if "MISSING" in verdicts:
        return "MISSING: test not reported"
    if runner != auditor:
        return "DIVERGENCE: human reads both"
    if runner == "PASS":
        return "nominal: spot-check"
    if runner == "UNVERIFIED":
        return "UNVERIFIED: window incomplete"
    return "REAL FAIL: human triage"


def cmd_unseal(a):
    run = Path(a.run_dir)
    sealed = run / "sealed"
    for line in (sealed / "sealed.sha256").read_text().splitlines():
        digest, name = line.split("  ", 1)
        if sha256_file(sealed / name) != digest:
            sys.exit(f"sealed/{name} was modified after the seal")
    seal_id = sha256_file(sealed / "sealed.sha256")
    if seal_id != seal_id_of(run):
        sys.exit("seal-id.txt does not match sealed/")
    faults = json.loads((sealed / "faults.json").read_text())
    msha = sha256_file(a.manifest)
    if faults["manifest_sha256"] != msha:
        sys.exit("sealed draw was made for a different manifest")
    m = json.loads(Path(a.manifest).read_text())
    bad = verify_record(run)
    if bad:
        sys.exit("\n".join(bad))
    rsha = sha256_file(run / "record.sha256")
    runner = json.loads((run / "runner" / "verdicts.json").read_text())
    auditor = json.loads((run / "auditor" / "verdicts.json").read_text())
    for who, v in (("runner", runner), ("auditor", auditor)):
        if v.get("record_sha256") != rsha:
            sys.exit(f"{who} verdicts are not about this record")
    events = (run / "record" / "events.jsonl").read_text()
    if f"RFR {runner['run']} M {msha[:8]}" not in events:
        sys.exit("record has no manifest marker for this manifest")
    if f"RFR {runner['run']} S {seal_id[:8]}" not in events:
        sys.exit("record has no seal marker for this draw")
    expectations = json.loads((sealed / "expectations.json").read_text())
    rows, triggers = [], []
    for test in m["plan"]["tests"]:
        exp = expectations.get(test, "PASS")
        r = runner["tests"].get(test, {}).get("verdict", "MISSING")
        u = auditor["tests"].get(test, {}).get("verdict", "MISSING")
        o = outcome(exp, r, u)
        ql = run / "auditor" / "quicklooks" / f"{test}.md"
        link = (
            f"[{test}.md](auditor/quicklooks/{test}.md)" if ql.is_file() else "MISSING"
        )
        if link == "MISSING" and not o.startswith("MISSING"):
            o += "; quick-look missing"
        rows.append(f"| ☐ | {test} | {exp} | {r} | {u} | {link} | {o} |")
        if not o.startswith(("nominal", "planted")):
            triggers.append(f"- {test}: {o}")
    text = [
        "# Unseal",
        "",
        f"Manifest `{msha[:8]}` · seal-id `{seal_id[:8]}` (compare with the value "
        f"printed by `draw`) · record `{rsha[:8]}` · run `{runner['run']}`",
        "",
        "## Human triggers",
        *(triggers or ["- none"]),
        "",
        "## Table",
        "| ☐ | Test | Expected | Runner | Auditor | Quick-look | Outcome |",
        "|---|---|---|---|---|---|---|",
        *rows,
        "",
    ]
    Path(a.output).write_text("\n".join(text))
    print(f"unseal -> {a.output} ({len(triggers)} triggers)")


def _git_repo(path, content):
    path.mkdir()
    (path / "f").write_text(content)
    env = ["-c", "user.name=t", "-c", "user.email=t@t"]
    subprocess.run(["git", "-C", path, "init", "-q"], check=True)
    subprocess.run(["git", "-C", path, "add", "f"], check=True)
    subprocess.run(["git", "-C", path, *env, "commit", "-q", "-m", "x"], check=True)


def cmd_selftest(_a):
    with tempfile.TemporaryDirectory() as tmp:
        d = Path(tmp)
        _git_repo(d / "uut", "uut")
        _git_repo(d / "harness", "harness")
        tests = ["L0-1", "L2-1", "L2-2", "L2-3", "L2-4", "L7-1"]
        (d / "plan.md").write_text(
            "".join(f"ID:        {t}\nTitle: t\n" for t in tests)
        )
        for name in ("a.py", "b.py", "bench.md", "image.bin"):
            (d / name).write_text(name)
        menu = {
            "knobs": [
                {
                    "name": "k1",
                    "class": "harness-expectation",
                    "applies_to": ["L2-1", "L2-2"],
                },
                {"name": "k2", "class": "uut-state", "applies_to": ["L2-3", "L7-1"]},
            ]
        }
        (d / "menu.json").write_text(json.dumps(menu))
        (d / "seed").write_text("captain-picard-1701\n")
        mf = str(d / "manifest.json")
        main(
            [
                "manifest",
                "--uut",
                str(d / "uut"),
                "--harness",
                str(d / "harness"),
                "--plan",
                str(d / "plan.md"),
                "--checker",
                f"builder={d / 'a.py'}",
                "--checker",
                f"reviewer={d / 'b.py'}",
                "--menu",
                str(d / "menu.json"),
                "--bench",
                str(d / "bench.md"),
                "--image",
                f"Ref={d / 'image.bin'}:make",
                "-o",
                mf,
            ]
        )
        main(["verify", mf])
        run = d / "rfr" / "run1"
        main(
            [
                "draw",
                "--manifest",
                mf,
                "--seed-file",
                str(d / "seed"),
                "--max-count",
                "2",
                "--run-dir",
                str(run),
            ]
        )
        exp = json.loads((run / "sealed" / "expectations.json").read_text())
        planted = [t for t, v in exp.items() if v == "FAIL"]
        assert 1 <= len(planted) <= 2 and set(exp) == set(tests), exp
        run2 = d / "rfr" / "run2"
        main(
            [
                "draw",
                "--manifest",
                mf,
                "--seed-file",
                str(d / "seed"),
                "--max-count",
                "2",
                "--run-dir",
                str(run2),
            ]
        )
        assert (run / "sealed" / "faults.json").read_text() == (
            run2 / "sealed" / "faults.json"
        ).read_text(), "draw must be reproducible"
        seal_id = seal_id_of(run)
        rec = run / "record"
        rec.mkdir()
        msha = sha256_file(mf)
        ev = [
            json.dumps({"name": "NoOpStringReceived", "args": {"message": s}})
            for s in (f"RFR run1 M {msha[:8]}", f"RFR run1 S {seal_id[:8]}")
        ]
        (rec / "events.jsonl").write_text("\n".join(ev) + "\n")
        (rec / "export-counts.json").write_text(json.dumps({"events.jsonl": 2}))
        main(["hash-record", "--run-dir", str(run)])
        main(["verify-record", "--run-dir", str(run)])
        rsha = sha256_file(run / "record.sha256")
        verd = {t: {"verdict": exp[t]} for t in tests}
        for who, v in (
            ("runner", verd),
            ("auditor", {**verd, "L0-1": {"verdict": "FAIL"}}),
        ):
            (run / who).mkdir()
            (run / who / "verdicts.json").write_text(
                json.dumps({"run": "run1", "record_sha256": rsha, "tests": v})
            )
        main(
            [
                "unseal",
                "--run-dir",
                str(run),
                "--manifest",
                mf,
                "-o",
                str(d / "unseal.md"),
            ]
        )
        text = (d / "unseal.md").read_text()
        assert "DIVERGENCE" in text and "RFR VOID" not in text, text
        assert text.count("planted fault detected") == len(planted), text
        assert "nominal: spot-check" in text, text
        (rec / "events.jsonl").write_text("tampered\n")
        assert verify_record(run), "verify-record must detect a modified record"
        (d / "plan.md").write_text("ID:        L0-1\n")
        _, bad = verify_manifest(mf)
        assert bad, "verify must detect a changed plan"
    print("selftest ok")


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = p.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser(
        "manifest", help="freeze UUT, harness, plan, checkers, images, menu"
    )
    s.add_argument("--uut", required=True)
    s.add_argument("--harness", required=True)
    s.add_argument("--plan", required=True)
    s.add_argument("--plan-revision", default="")
    s.add_argument("--checker", action="append", default=[], metavar="OWNER=PATH")
    s.add_argument(
        "--image", action="append", default=[], metavar="DEPLOYMENT=PATH[:BUILD]"
    )
    s.add_argument("--menu", required=True)
    s.add_argument("--bench", required=True)
    s.add_argument("-o", "--output", default="manifest.json")
    s.set_defaults(fn=cmd_manifest)
    s = sub.add_parser("verify", help="refuse if anything on the manifest moved")
    s.add_argument("manifest")
    s.set_defaults(fn=cmd_verify)
    s = sub.add_parser("draw", help="human-seeded draw of planted faults")
    s.add_argument("--manifest", required=True)
    s.add_argument(
        "--seed-file", help="file holding the seed; omitted = prompt, no echo"
    )
    s.add_argument("--max-count", type=int, default=3)
    s.add_argument("--run-dir", required=True, help="rfr/<run-id>, outside both repos")
    s.set_defaults(fn=cmd_draw)
    s = sub.add_parser("seal-id", help="print the seal id (readable by everyone)")
    s.add_argument("--run-dir", required=True)
    s.set_defaults(fn=cmd_seal_id)
    s = sub.add_parser("hash-record", help="write <run>/record.sha256")
    s.add_argument("--run-dir", required=True)
    s.set_defaults(fn=cmd_hash_record)
    s = sub.add_parser(
        "verify-record", help="re-hash the record and check export counts"
    )
    s.add_argument("--run-dir", required=True)
    s.set_defaults(fn=cmd_verify_record)
    s = sub.add_parser("unseal", help="join expectations, runner and auditor verdicts")
    s.add_argument("--run-dir", required=True)
    s.add_argument("--manifest", required=True)
    s.add_argument("-o", "--output", default="unseal.md")
    s.set_defaults(fn=cmd_unseal)
    sub.add_parser(
        "selftest", help="exercise every subcommand in a temp dir"
    ).set_defaults(fn=cmd_selftest)
    a = p.parse_args(argv)
    a.fn(a)


if __name__ == "__main__":
    main()
