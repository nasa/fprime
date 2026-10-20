#!/usr/bin/env python3
"""Run-for-record mechanics: manifest, verify, draw, seal-id, hash-record,
verify-record, unseal. Python 3 stdlib only. See SKILL.md beside this file."""

import argparse
import getpass
import hashlib
import json
import os
import re
import stat
import subprocess
import sys
import tempfile
from pathlib import Path

VERDICTS = ("PASS", "FAIL", "UNVERIFIED")
EXPORTS = ("events.jsonl", "channels.jsonl", "commands.jsonl")
KNOB_CLASSES = ("harness-expectation", "uut-state", "stimulus")
REFUSED_IMAGES = (".elf", ".hex", ".axf", ".uf2")
MAX_ID = 8


def sha256_file(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def sha256_text(text):
    return hashlib.sha256(text.encode()).hexdigest()


def read_json(path, what):
    p = Path(path)
    if not p.is_file():
        sys.exit(f"missing {what}: {p}")
    try:
        return json.loads(p.read_text())
    except json.JSONDecodeError as e:
        sys.exit(f"{what} is not valid JSON: {p}: {e}")


def git(path, *args):
    try:
        out = subprocess.run(
            ["git", "-C", str(path), *args], text=True, capture_output=True, check=True
        )
    except (OSError, subprocess.CalledProcessError) as e:
        sys.exit(f"git {' '.join(args)} failed in {path}: {e}")
    return out.stdout.strip()


def describe(path):
    d = git(path, "describe", "--tags", "--always", "--dirty", "--broken")
    if "-dirty" in d or "-broken" in d:
        sys.exit(f"refusing: {path} describes as {d}")
    return d


def repo_entry(path, name=None):
    if git(path, "status", "--porcelain"):
        sys.exit(f"refusing: {path} has uncommitted or untracked files")
    e = {
        "path": str(path),
        "sha": git(path, "rev-parse", "HEAD"),
        "describe": describe(path),
    }
    e["dirty"] = False
    return {"name": name, **e} if name else e


def file_entry(path):
    if not Path(path).is_file():
        sys.exit(f"missing file: {path}")
    return {"path": str(path), "sha256": sha256_file(path)}


def plan_tests(plan_path):
    ids = re.findall(r"^ID:[ \t]+(\S+)", Path(plan_path).read_text(), re.MULTILINE)
    if not ids:
        sys.exit("plan has no 'ID: <test>' lines at column 0 (design skill §3)")
    long = [i for i in ids if len(i) > MAX_ID]
    if long:
        sys.exit(f"test IDs longer than {MAX_ID} chars do not fit a marker: {long}")
    dup = sorted({i for i in ids if ids.count(i) > 1})
    if dup:
        sys.exit(f"duplicate test IDs in plan: {dup}")
    return ids


def image_entry(spec, repos, skip_reason):
    dep, _, rest = spec.partition("=")
    p, _, build = rest.partition(":")
    if not dep or not p:
        sys.exit("--image takes <deployment>=<path.bin>[:<build command>]")
    if Path(p).suffix.lower() in REFUSED_IMAGES:
        sys.exit(f"{p}: the manifest image is the raw binary written to flash (.bin)")
    entry = {"deployment": dep, **file_entry(p), "build_cmd": build}
    data = Path(p).read_bytes()
    missing = [r["name"] for r in repos if r["describe"].encode() not in data]
    if not missing:
        entry["version_check"] = "all repo describe strings found in image"
    elif skip_reason:
        entry["version_check"] = (
            f"SKIPPED ({', '.join(missing)} not found): {skip_reason}"
        )
    else:
        sys.exit(
            f"{p}: describe string of {missing} not found in the image (Svc.Version "
            "PROJECT/FRAMEWORK/LIBRARY versions); rebuild from the manifest trees or "
            "give --no-version-check <reason>"
        )
    return entry


def load_menu(path, tests):
    menu = read_json(path, "fault menu")
    if not isinstance(menu.get("max_count"), int) or menu["max_count"] < 1:
        sys.exit("fault menu needs an integer max_count >= 1")
    knobs = menu.get("knobs", [])
    if not any(k.get("class") == "uut-state" for k in knobs):
        sys.exit("fault menu needs at least one knob of class uut-state")
    pairs, by_name = [], {}
    for k in knobs:
        name = k.get("name")
        if not name or name in by_name:
            sys.exit(f"knob without a unique name: {k}")
        if k.get("expected", "FAIL") != "FAIL":
            sys.exit(f"knob {name}: planted faults must expect FAIL")
        if k.get("class") not in KNOB_CLASSES:
            sys.exit(f"knob {name}: class must be one of {KNOB_CLASSES}")
        if not k.get("fails"):
            sys.exit(f"knob {name}: needs 'fails': [<pass-rule predicates it breaks>]")
        ripples = k.get("ripples_to", [])
        k["ripples_to"] = (
            ripples if isinstance(ripples, dict) else {t: None for t in ripples}
        )
        for t in [*k["applies_to"], *k["ripples_to"]]:
            if t not in tests:
                sys.exit(f"knob {name}: {t} is not a plan test")
        for t in k["applies_to"]:
            if t in k["ripples_to"]:
                sys.exit(f"knob {name}: {t} cannot be both target and ripple")
            pairs.append((t, name))
        by_name[name] = k
    if not pairs:
        sys.exit("fault menu has no (test, knob) pairs")
    if len({t for t, _ in pairs}) < 2 * menu["max_count"]:
        sys.exit(
            f"menu covers {len({t for t, _ in pairs})} tests; need at least "
            f"{2 * menu['max_count']} for max_count {menu['max_count']}"
        )
    return menu, pairs, by_name


def cmd_manifest(a):
    checkers = []
    for spec in a.checker:
        owner, _, p = spec.partition("=")
        if owner not in ("builder", "reviewer") or not p:
            sys.exit("--checker takes builder=<path> or reviewer=<path>")
        checkers.append({"owner": owner, **file_entry(p)})
    if {c["owner"] for c in checkers} != {"builder", "reviewer"}:
        sys.exit("manifest needs one builder and one reviewer checker")
    uut = repo_entry(a.uut)
    repos = [repo_entry(a.uut, "uut"), repo_entry(a.harness, "harness")]
    for spec in a.repo:
        name, _, path = spec.partition("=")
        if not name or not path or name in {r["name"] for r in repos}:
            sys.exit(
                "--repo takes a unique <name>=<path> (project, fprime, each library)"
            )
        repos.append(repo_entry(path, name))
    images = [image_entry(s, repos, a.no_version_check) for s in a.image]
    if not images:
        sys.exit("manifest needs at least one --image")
    tests = plan_tests(a.plan)
    load_menu(a.menu, tests)
    m = {
        "uut": uut,
        "harness": repo_entry(a.harness),
        "repos": repos,
        "plan": {**file_entry(a.plan), "revision": a.plan_revision, "tests": tests},
        "checkers": checkers,
        "images": images,
        "menu": file_entry(a.menu),
        "bench": file_entry(a.bench),
        "no_version_check": (
            a.no_version_check
            if any(i["version_check"].startswith("SKIPPED") for i in images)
            else ""
        ),
    }
    out = Path(a.output or f"rfr/manifest-{uut['sha'][:8]}.json")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(m, indent=2) + "\n")
    print(f"manifest {sha256_file(out)[:8]} ({len(tests)} tests) -> {out}")


def verify_manifest(path):
    m = read_json(path, "manifest")
    bad = []
    repos = [("uut", m["uut"]), ("harness", m["harness"])]
    repos += [(r["name"], r) for r in m.get("repos", [])]
    for key, e in repos:
        if git(e["path"], "rev-parse", "HEAD") != e["sha"]:
            bad.append(f"{key}: HEAD moved")
        if git(e["path"], "status", "--porcelain"):
            bad.append(f"{key}: tree dirty")
    for e in [m["plan"], m["menu"], m["bench"], *m["checkers"], *m["images"]]:
        if not Path(e["path"]).is_file() or sha256_file(e["path"]) != e["sha256"]:
            bad.append(f"{e['path']}: hash moved or missing")
    plan = Path(m["plan"]["path"])
    if plan.is_file() and plan_tests(plan) != m["plan"]["tests"]:
        bad.append("plan: test list changed")
    return m, bad


def cmd_verify(a):
    _, bad = verify_manifest(a.manifest)
    if bad:
        sys.exit("\n".join(bad))
    print("manifest verified")


def read_seed(a):
    if a.seed_file:
        p = Path(a.seed_file)
        if not p.is_file():
            sys.exit(f"missing seed file: {p}")
        if p.stat().st_mode & (stat.S_IRGRP | stat.S_IROTH):
            sys.exit(f"{p} is readable by others; chmod 600 it")
        seed = p.read_text().strip()
        os.remove(p)
        print(f"seed file {p} read and deleted")
    else:
        seed = getpass.getpass("seal seed (not echoed): ").strip()
    if len(seed) < 8:
        sys.exit("seed must be at least 8 characters")
    return seed


def cmd_draw(a):
    m, bad = verify_manifest(a.manifest)
    if bad:
        sys.exit("\n".join(bad))
    run = Path(a.run_dir)
    if len(run.name) > MAX_ID or not run.name:
        sys.exit(f"run id (directory name) must be 1..{MAX_ID} characters")
    tests = m["plan"]["tests"]
    menu, pairs, by_name = load_menu(m["menu"]["path"], tests)
    seed = read_seed(a)
    msha = sha256_file(a.manifest)
    count = 1 + int(sha256_text(f"{seed}:{msha}:count"), 16) % menu["max_count"]
    chosen, used = {}, set()
    for test, knob in sorted(pairs, key=lambda tk: sha256_text(f"{seed}:{msha}:{tk}")):
        if test not in used:
            chosen[test] = knob
            used.add(test)
        if len(chosen) == count:
            break
    expectations = {t: "PASS" for t in tests}
    for knob in chosen.values():
        for r in by_name[knob]["ripples_to"]:
            if expectations[r] == "PASS":
                expectations[r] = "FAIL-RIPPLE"
    for test in chosen:
        expectations[test] = "FAIL"
    sealed = run / "sealed"
    sealed.mkdir(parents=True, exist_ok=True)
    faults = {"manifest_sha256": msha, "faults": dict(sorted(chosen.items()))}
    (sealed / "faults.json").write_text(json.dumps(faults, indent=2) + "\n")
    (sealed / "expectations.json").write_text(json.dumps(expectations, indent=2) + "\n")
    lines = [
        f"{sha256_file(sealed / n)}  {n}" for n in ("faults.json", "expectations.json")
    ]
    (sealed / "sealed.sha256").write_text("\n".join(lines) + "\n")
    seal_id = sha256_file(sealed / "sealed.sha256")
    (run / "seal-id.txt").write_text(seal_id + "\n")
    print(
        f"seal-id {seal_id[:8]} manifest {msha[:8]} -> {run}/sealed/ "
        "(planted count hidden; keep the seed)"
    )


def seal_id_of(run_dir):
    p = Path(run_dir) / "seal-id.txt"
    if not p.is_file():
        sys.exit(f"missing {p}")
    return p.read_text().strip()


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
    digest = run / "record.sha256"
    if not digest.is_file():
        return [f"missing {digest}"]
    if record_lines(rec) != digest.read_text().splitlines():
        bad.append("record/ differs from record.sha256")
    counts_file = rec / "export-counts.json"
    if not counts_file.is_file():
        return bad + ["record/export-counts.json missing (fprime-test-oracle §2)"]
    counts = json.loads(counts_file.read_text())
    for name in EXPORTS:
        p = rec / name
        if not p.is_file() or name not in counts:
            bad.append(f"{name}: export or its archive count missing")
            continue
        n = sum(1 for _ in open(p, "rb"))
        if n != counts[name]:
            bad.append(f"{name}: {n} rows exported, archive reports {counts[name]}")
    return bad


def cmd_verify_record(a):
    bad = verify_record(a.run_dir)
    if bad:
        sys.exit("\n".join(bad))
    print("record verified")


def outcome(expected, runner, auditor):
    verdicts = {runner, auditor}
    if expected in ("FAIL", "FAIL-RIPPLE"):
        label = "planted fault" if expected == "FAIL" else "planted ripple"
        if "PASS" in verdicts or "MISSING" in verdicts:
            return f"RFR VOID: {label} not detected"
        if verdicts == {"FAIL"}:
            return f"{label} detected"
        if verdicts == {"UNVERIFIED"}:
            return f"UNVERIFIED on {label}: harness/bench triage"
        return f"DIVERGENCE on {label}: human reads both"
    if "MISSING" in verdicts:
        return "MISSING: test not reported"
    if runner != auditor:
        return "DIVERGENCE: human reads both"
    if runner == "PASS":
        return "nominal: spot-check"
    if runner == "UNVERIFIED":
        return "UNVERIFIED: window incomplete"
    return "REAL FAIL: human triage"


def allowed_failures(test, expected, faults, by_name):
    """Predicates a planted (or ripple) test is allowed to fail; None = no check."""
    if expected not in ("FAIL", "FAIL-RIPPLE"):
        return None
    allowed = set(by_name[faults[test]]["fails"]) if expected == "FAIL" else set()
    preds = [
        by_name[n]["ripples_to"].get(test)
        for n in faults.values()
        if test in by_name[n]["ripples_to"]
    ]
    if any(p is None for p in preds):
        return None
    return allowed.union(*preds) if preds else (allowed or None)


def dump_expectation(test, faults, by_name):
    """Only a uut-state knob changes what an independent dump should find."""
    knob = by_name.get(faults.get(test))
    if knob and knob["class"] == "uut-state":
        return "FAIL"
    for name in faults.values():
        k = by_name[name]
        if k["class"] == "uut-state" and test in k["ripples_to"]:
            return "FAIL-RIPPLE"
    return "PASS"


def final_state_rows(run, faults, by_name):
    fs = run / "auditor" / "final-state.json"
    if not fs.is_file():
        missing = "MISSING: independent final-state check not performed"
        return [(("final-state", "—", "MISSING"), missing)]
    entries = read_json(fs, "final-state")
    rows = []
    for f in entries if isinstance(entries, list) else [entries]:
        test = f.get("test", "?")
        verdict = f.get("verdict") if f.get("verdict") in VERDICTS else "MISSING"
        exp = dump_expectation(test, faults, by_name)
        o = outcome(exp, verdict, verdict)
        if verdict == "MISSING":
            o = "MISSING: final-state verdict absent"
        if verdict == "PASS" and f.get("expected_sha256") != f.get("observed_sha256"):
            o = "INCONSISTENT: final-state PASS with differing hashes"
        rows.append(((test, exp, verdict), o))
    return rows


def cmd_unseal(a):
    run = Path(a.run_dir)
    sealed = run / "sealed"
    digest = sealed / "sealed.sha256"
    if not digest.is_file():
        sys.exit(f"missing {digest}")
    for line in digest.read_text().splitlines():
        d, name = line.split("  ", 1)
        if sha256_file(sealed / name) != d:
            sys.exit(f"sealed/{name} was modified after the draw")
    seal_id = sha256_file(digest)
    if seal_id != seal_id_of(run):
        sys.exit("seal-id.txt does not match sealed/")
    faults = read_json(sealed / "faults.json", "sealed draw")
    msha = sha256_file(a.manifest)
    if faults["manifest_sha256"] != msha:
        sys.exit("sealed draw was made for a different manifest")
    m = read_json(a.manifest, "manifest")
    _, _, by_name = load_menu(m["menu"]["path"], m["plan"]["tests"])
    bad = verify_record(run)
    if bad:
        sys.exit("\n".join(bad))
    rsha = sha256_file(run / "record.sha256")
    runner = read_json(run / "runner" / "verdicts.json", "runner verdicts")
    auditor = read_json(run / "auditor" / "verdicts.json", "auditor verdicts")
    for who, v in (("runner", runner), ("auditor", auditor)):
        if v.get("record_sha256") != rsha:
            sys.exit(f"{who} verdicts are not about this record")
        if v.get("run") != run.name:
            sys.exit(
                f"{who} verdicts name run {v.get('run')!r}, directory is {run.name!r}"
            )
    events = (run / "record" / "events.jsonl").read_text()
    if f"RFR {run.name} M {msha[:8]}" not in events:
        sys.exit("record has no manifest marker for this manifest")
    if f"RFR {run.name} S {seal_id[:8]}" not in events:
        sys.exit("record has no seal marker for this draw")
    expectations = read_json(sealed / "expectations.json", "sealed expectations")
    drawn = faults["faults"]
    rows, triggers = [], []

    def add(test, exp, r, u, link, o):
        rows.append(f"| ☐ | {test} | {exp} | {r} | {u} | {link} | {o} |")
        if not o.startswith(("nominal", "planted")) or "UNEXPLAINED" in o:
            triggers.append(f"- {test}: {o}")

    for test in m["plan"]["tests"]:
        exp = expectations.get(test, "PASS")
        rv, uv = runner["tests"].get(test, {}), auditor["tests"].get(test, {})
        r, u = rv.get("verdict", "MISSING"), uv.get("verdict", "MISSING")
        o = outcome(exp, r, u)
        allowed = allowed_failures(test, exp, drawn, by_name)
        if allowed is not None and o.endswith("detected"):
            failed = set(rv.get("failed", [])) | set(uv.get("failed", []))
            extra = sorted(failed - allowed)
            if extra:
                o += f" + UNEXPLAINED FAIL ({'; '.join(extra)}): human triage"
        ql = run / "auditor" / "quicklooks" / f"{test}.md"
        link = (
            f"[{test}.md](auditor/quicklooks/{test}.md)" if ql.is_file() else "MISSING"
        )
        if link == "MISSING" and not o.startswith("MISSING"):
            o += "; quick-look missing"
        add(test, exp, r, u, link, o)
    for (test, exp, verdict), o in final_state_rows(run, drawn, by_name):
        link = (
            "[final-state.md](auditor/final-state.md)"
            if verdict != "MISSING"
            else "MISSING"
        )
        add(f"{test}/dump", exp, "—", verdict, link, o)
    header = [
        f"Manifest `{msha[:8]}` · seal-id `{seal_id[:8]}` (compare with the value "
        f"printed by `draw`) · record `{rsha[:8]}` · run `{run.name}`"
    ]
    if m.get("no_version_check"):
        header.append(f"**Image version check skipped:** {m['no_version_check']}")
        triggers.insert(0, "- image provenance check was skipped at manifest time")
    text = [
        "# Unseal",
        "",
        *header,
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
        for name in ("uut", "harness", "proj"):
            _git_repo(d / name, name)
        tests = ["L0-1", "L2-1", "L2-2", "L2-3", "L2-4", "L7-1"]
        (d / "plan.md").write_text(
            "".join(f"ID:        {t}\nTitle: t\n" for t in tests)
        )
        for name in ("a.py", "b.py", "bench.md"):
            (d / name).write_text(name)
        (d / "image.bin").write_bytes(
            b"\0LIB "
            + describe(d / "uut").encode()
            + b"\0PROJ "
            + describe(d / "proj").encode()
        )
        menu = {
            "max_count": 2,
            "knobs": [
                {
                    "name": "k1",
                    "class": "harness-expectation",
                    "applies_to": ["L2-1", "L2-2"],
                    "fails": ["p1"],
                },
                {
                    "name": "k2",
                    "class": "uut-state",
                    "applies_to": ["L2-3", "L7-1"],
                    "ripples_to": {"L2-4": ["p2"]},
                    "fails": ["p2"],
                },
            ],
        }
        (d / "menu.json").write_text(json.dumps(menu))
        seed = d / "seed"
        seed.write_text("captain-picard-1701\n")
        seed.chmod(0o600)
        mf = str(d / "rfr" / "manifest.json")
        base = ["manifest", "--uut", str(d / "uut"), "--harness", str(d / "harness")]
        base += ["--repo", f"project={d / 'proj'}", "--plan", str(d / "plan.md")]
        base += [
            "--checker",
            f"builder={d / 'a.py'}",
            "--checker",
            f"reviewer={d / 'b.py'}",
        ]
        base += ["--menu", str(d / "menu.json"), "--bench", str(d / "bench.md")]
        main(base + ["--image", f"Ref={d / 'image.bin'}:make", "-o", mf])
        main(["verify", mf])
        by = {
            "k1": {"class": "harness-expectation", "ripples_to": {}},
            "k2": {"class": "uut-state", "ripples_to": {"L2-4": None}},
        }
        assert dump_expectation("L7-1", {"L7-1": "k1"}, by) == "PASS"
        assert dump_expectation("L7-1", {"L7-1": "k2"}, by) == "FAIL"
        assert dump_expectation("L2-4", {"L7-1": "k2"}, by) == "FAIL-RIPPLE"
        run = d / "rfr" / "run1"
        main(
            ["draw", "--manifest", mf, "--seed-file", str(seed), "--run-dir", str(run)]
        )
        assert not seed.exists(), "seed file must be deleted after the draw"
        exp = json.loads((run / "sealed" / "expectations.json").read_text())
        faults = json.loads((run / "sealed" / "faults.json").read_text())["faults"]
        assert 1 <= len(faults) <= 2 and set(exp) == set(tests), (faults, exp)
        if "k2" in faults.values():
            assert exp["L2-4"] in ("FAIL-RIPPLE", "FAIL"), exp
        seed.write_text("captain-picard-1701\n")
        seed.chmod(0o600)
        run2 = d / "rfr" / "run2"
        main(
            ["draw", "--manifest", mf, "--seed-file", str(seed), "--run-dir", str(run2)]
        )
        assert (run / "sealed" / "faults.json").read_text() == (
            run2 / "sealed" / "faults.json"
        ).read_text(), "draw must be reproducible"
        seal_id = seal_id_of(run)
        rec = run / "record"
        rec.mkdir()
        msha = sha256_file(mf)
        markers = (f"RFR run1 M {msha[:8]}", f"RFR run1 S {seal_id[:8]}")
        ev = [
            json.dumps({"name": "NoOpStringReceived", "args": {"message": s}})
            for s in markers
        ]
        (rec / "events.jsonl").write_text("\n".join(ev) + "\n")
        (rec / "channels.jsonl").write_text("{}\n")
        (rec / "commands.jsonl").write_text("{}\n{}\n")
        counts = {"events.jsonl": 2, "channels.jsonl": 1, "commands.jsonl": 2}
        (rec / "export-counts.json").write_text(json.dumps(counts))
        main(["hash-record", "--run-dir", str(run)])
        main(["verify-record", "--run-dir", str(run)])
        rsha = sha256_file(run / "record.sha256")
        knobs = {k["name"]: k for k in menu["knobs"]}

        def verd(t):
            v = {"verdict": exp[t].replace("-RIPPLE", "")}
            if t in faults:
                v["failed"] = knobs[faults[t]]["fails"]
            elif exp[t] == "FAIL-RIPPLE":
                v["failed"] = ["p2"]
            return v

        rv = {t: verd(t) for t in tests}
        uv = {t: dict(verd(t)) for t in tests}
        uv["L0-1"] = {"verdict": "FAIL", "failed": ["version"]}
        planted = next(iter(faults))
        uv[planted] = {
            "verdict": "FAIL",
            "failed": uv[planted]["failed"] + ["SpiError == 0"],
        }
        for who, v in (("runner", rv), ("auditor", uv)):
            (run / who).mkdir()
            (run / who / "verdicts.json").write_text(
                json.dumps({"run": "run1", "record_sha256": rsha, "tests": v})
            )
        dump_v = "FAIL" if dump_expectation("L7-1", faults, knobs) != "PASS" else "PASS"
        (run / "auditor" / "final-state.json").write_text(
            json.dumps(
                [
                    {
                        "test": "L7-1",
                        "verdict": dump_v,
                        "expected_sha256": "x",
                        "observed_sha256": "x" if dump_v == "PASS" else "y",
                    }
                ]
            )
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
        assert "DIVERGENCE: human reads both" in text and "RFR VOID" not in text, text
        assert "UNEXPLAINED FAIL (SpiError == 0)" in text, text
        assert "L7-1/dump" in text and "nominal: spot-check" in text, text
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
        "manifest", help="freeze UUT, harness, repos, plan, checkers, images"
    )
    s.add_argument("--uut", required=True)
    s.add_argument("--harness", required=True)
    s.add_argument(
        "--repo",
        action="append",
        default=[],
        metavar="NAME=PATH",
        help="other repository the image is built from (project, fprime, library)",
    )
    s.add_argument("--plan", required=True)
    s.add_argument("--plan-revision", default="")
    s.add_argument("--checker", action="append", default=[], metavar="OWNER=PATH")
    s.add_argument(
        "--image", action="append", default=[], metavar="DEPLOYMENT=PATH.bin[:BUILD]"
    )
    s.add_argument(
        "--no-version-check",
        metavar="REASON",
        help="skip the describe-string-in-image check (recorded, shown at unseal)",
    )
    s.add_argument("--menu", required=True)
    s.add_argument("--bench", required=True)
    s.add_argument("-o", "--output", help="default rfr/manifest-<uut sha8>.json")
    s.set_defaults(fn=cmd_manifest)
    s = sub.add_parser("verify", help="refuse if anything on the manifest moved")
    s.add_argument("manifest")
    s.set_defaults(fn=cmd_verify)
    s = sub.add_parser(
        "draw", help="human-seeded draw of planted faults (count from menu)"
    )
    s.add_argument("--manifest", required=True)
    s.add_argument(
        "--seed-file",
        help="chmod-600 seed file, deleted after use; omitted = un-echoed prompt",
    )
    s.add_argument("--run-dir", required=True, help="rfr/<run-id>, run id <= 8 chars")
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
