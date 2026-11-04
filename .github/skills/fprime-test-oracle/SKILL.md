---
name: fprime-test-oracle
description: >-
  Use when writing or implementing the success condition of a system
  test as code over a recorded run: the pass-rule format, the exported
  record it reads, the completeness-versus-UUT split, the
  two-independent-checkers rule, and the verdict and divergence formats
  shared by the test designer, reviewer, builder, runner, and auditor;
  the `@planted_fault` fault-injection declarations and the sealed faults file.
  Keywords: F Prime, test oracle, pass rule, checker, verdict, record,
  planted fault, faults.json, fth, divergence.
---

# Skill: F Prime test oracle — pass rules and checkers

A **test oracle** is whatever decides that an observed result is
correct. Here it has two halves with separate names: the **pass
rule** (the condition, public, part of the approved plan) and the
**checker** (a program that evaluates pass rules over the record).
Every record is judged twice by the same named checks: live, by the
test function under `fth run` (pytest's junit is the runner's verdict),
and by the auditor's **replay** of that same test function over the
sealed record (`fth audit --tests`, tooling pinned in
`test-process-contract.md` §8) — so that a disagreement (`DIVERGENCE`)
is one program disagreeing with itself about one record, which only a
non-deterministic test or a doctored record can produce. The engine
(`fth evaluate`) and a reviewer's independent checker remain for legacy
plans whose pass rules still live in `plan.json`.

---

## 1. Pass-rule format

Since phase 1 the named checks are declared **in the integration test**
(`fprime-system-test-harness` §3), in the grammar of the tooling's
`docs/plan-format.md`; `plan.json` keeps windows and `metrics`. Checks
refer only to things that exist in the record and are judged per window
over its closed history:

```python
with window(60, 61):
    require("STD")                                     # std_complete: markers, dispatcher, drops, packet samples
    send("framTester.SWEEP", [0x5005, 4096, 0x5005]); bench.wait_event("framTester.SweepDone", min_new=2)
    check("ABS")                                       # absence group from plan.abs
    check("event framTester.SweepDone(kind=WRITE, chunks=4096, errors=0) count == 1")
    check("event framTester.SweepDone(kind=WRITE) precedes event framTester.SweepDone(kind=VERIFY)")
    check("delta(framDriver.SpiErrors) == 0")
    check("dump image == xorshift32(seed)", dump_image_matches_xorshift32(0x5A, ...))   # check function
# plan.json: "metrics": ["delta(framTester.BytesVerified) == 262144",
#                        "t(event framTester.SweepDone(kind=VERIFY)) - t(framTester.SWEEP sent) <= 1200 s"]
```

Rules:

- `require` may reference only markers and infrastructure counters
  (`both markers present`, `commands_sent == count(event
  cmdDisp.OpCodeDispatched)`, dispatcher error and drop counters,
  `<packet> samples >= n`). UUT events belong in `check`. A precondition
  that does not hold is `UNVERIFIED` (`Incomplete`); a failed `check` or
  `metrics` predicate in a judgeable window is `FAIL`. A missing UUT
  completion event is therefore `FAIL`, never `UNVERIFIED` (Captain's
  rule: unverified only when the absence itself is not indicative of a
  UUT failure assuming a working system). Legacy plans that still carry
  `complete`/`all_of` are judged by `fth.engine` with the same rule.
- Event arguments are matched from the event **text** with the plan's
  `event_patterns` regexes (named groups), never from the exporter's
  numeric scrape (garbage for hex and enums).
- `commands_sent` is a record quantity: `commands.jsonl` rows from the
  START marker sending through the END marker sending, markers
  included.
- Absence predicates (`count == 0`) are valid only on a complete
  window; `delta` needs ≥ 2 samples and `first/last` ≥ 1, else
  `UNVERIFIED`.
- Thresholds carry units and come from the plan's metrics section.
  `t(…)` is FSW time; `CMD sent` is the command's `OpCodeDispatched`
  row; a window spanning a reboot (`reset: true`) has no `duration`.
- A pass rule never references the runner's, harness's, or pytest's
  own printed verdict.

## 2. Record export

Checkers read a flat export, not the live ground system, so both can
be re-run on the sealed record. `fth export` (inside `fth run`, and
again from a copy inside `fth audit`) writes it:

```
record/
  events.jsonl       one JSON object per event row (FSW time, boot epoch, name, severity, text, args)
  channels.jsonl     one per telemetry sample
  commands.jsonl     one per command sending (ground wall-clock, name, args, completion) — from raw/cmd/ only
  export-counts.json rows per file plus torn packets, as the exporter saw them
  raw/               ground-system archive as stored: GDS logs, recorder logs, cmd/ sender logs, flash.log
record.sha256        one line per file, produced by fth hash-record
```

The exporter dedupes exact duplicate recorder lines per file per boot
segment (the recorder writes every row twice), takes commands only from
`record/raw/cmd/`, and increments the boot `epoch` when the FSW clock
falls back by more than the plan's `reboot_min_backstep_s`; `t = epoch
× 1e6 + fsw` orders rows, archive index breaks ties, file order is
authoritative. `fth verify-record` fails when any export file is absent
or its row count differs from `export-counts.json`; the auditor's
re-export from a copy of `raw/` is the independent count. Checkers read
the export through the replay API (`fth.replay`: `load_record`,
`Record.window(start_id, end_id)`, `Window.events/channel/commands_sent/
segment(epoch)`), the only read surface the tooling's
`docs/replay-api.md` promises to keep.

## 3. Checker contract

**Runner and replay** (phase 1/2): the test function's `check(...)`
names are judged at window close; `fth run` derives
`runner/verdicts.json` from pytest's junit only (pass → PASS, failed
check → FAIL + the failed names, error/`Incomplete` → UNVERIFIED), and
`fth audit --tests <test/int>` re-executes the same functions over the
record (`--fth-replay-record`: `send()` must meet the recorded commands
in order, waits return at once) → `auditor/junit.xml` →
`auditor/verdicts.json` (`checker: fth.replay`), diffed per test with the
runner's — verdict **and** `failed` — any difference is `DIVERGENCE`.
Record integrity findings come first (one START..END per window, no
command outside a window, monotonic time, export counts, junit hashed,
no duplicate test). The former engine/independent-checker KATs are unit
tests of the check grammar and the check functions (`tests/test_checks.py`,
`tests/test_replay_match.py`, `tests/test_integrity.py`).

**Engine** (legacy plans with `complete`/`all_of`): `fth evaluate
plan.json <record-dir> -o verdicts.json` judges every plan predicate and
registered `@helper`. Output:

```
verdicts.json
  { "run": "<run id>", "record_sha256": "<sha256 of record.sha256>",
    "tests": { "L2-6": { "verdict": "PASS|FAIL|UNVERIFIED",
                         "failed": ["<predicate text>"],
                         "evidence": ["events.jsonl:1041-1049"] } } }
```

**Independent checker** (legacy plans only; optional since phase 2 —
no longer part of the contract) — interface frozen 2026-11-02:

```
python checks/<name>.py <record-dir> <plan.json> <out.json>
  -> {"tests": {ID: {"verdict": "PASS|FAIL|UNVERIFIED", "failed": ["<predicate text>"]}}}
```

Positional arguments in that order; `<record-dir>` holds the three
jsonl files; exit 0 after writing `<out.json>` (non-zero is
"independent checker failed", reported by the audit, not a verdict);
extra keys are ignored. `fth audit --independent` diffs verdict **and**
`failed` per test byte-for-byte.

- Every test on the manifest gets an entry; a test with no window in
  the record is `UNVERIFIED` with `failed: ["window missing"]`.
- `failed` holds the plan's predicate strings **verbatim** — the
  string is the key `fth unseal` matches against a planted fault's `fails`; a
  paraphrase turns every planted test into an unexplained failure.
  Transcribe whole sentences including any suffix the plan carries.
- `evidence` points into the export so a quick-look can be built from
  it without re-deriving anything.
- Both checkers are deterministic and have no bench or network access.
- The independent checker is written by an adversarial reviewer from
  the plan text alone, without reading the engine, the helpers module
  or any other checker; the two are compared only by their outputs on
  the same record (the mutation vectors, then the audit).

## 4. Divergence report

```
divergence.md                      (auditor/diff.md in stage 3)
| Test | Engine | Independent | Predicate(s) in dispute | Record lines |
```

Any row here blocks stage-2 closure (vectors) and is read first by the
human at unseal (stage 3).

## 5. Planted faults and the sealed faults file

A planted fault is a **command rewrite** declared next to the helpers and
applied by the `faults` fixture of `fth.pytest_plugin` to the **drawn
test only** — never by name to every test in `applies_to`:

```python
@planted_fault("verify_wrong_seed", applies_to=["L2-1", "L2-5"], cls="harness-expectation",
      fails={"L2-1": ["event framTester.VerifyDone(client=0, offset=0, length=64, status=OK, mismatches=0) count == 1"],
             "L2-5": ["event framTester.SweepDone(kind=VERIFY, chunks=4096, errors=0) count == 1"]},
      mechanism="VERIFY sent with seed+1", kind="rewrite")          # kind: rewrite (default) | insert
def verify_wrong_seed(test_id, command_name, args):
    if command_name == "framTester.VERIFY":
        return [args[0], args[1], args[2], args[3] + 1]              # rewrite: new args
    return None                                                      # untouched; insert planted faults return [(name, args), …]
```

- `fails` is a per-test dict of plan predicate text, verbatim (a legacy
  list expands to every `applies_to` test); `cls` is
  `harness-expectation | uut-state | stimulus`; `kind` is enforced at
  apply time (a `rewrite` that inserts is a `PlanError`). `fth
  planted-faults-menu plan.json` generates the fault menu from these
  declarations; it is never hand-written, and `fth verify-plan` rejects
  a planted fault on an unknown test.
- The sealed draw is exactly
  `{"manifest_sha256": "<sha of the manifest file>", "faults": {"<test>": "<planted_fault>"}}`.
  Anything else — malformed JSON, extra keys, unknown test, unknown or
  inapplicable planted fault, a stale `manifest_sha256` — stops pytest with exit
  code 2 **before any command is sent**, with a message that names no
  planted fault.
- Nothing names an active planted fault outside `sealed/`: not the run id (it is
  invertible through `MARK(crc32(run id))`), not stdout, not a file
  under `runner/` or `record/`.

## 6. Lessons that shaped these rules (fram-r1, fram-r2)

- Planted fault applied by **name** to every `applies_to` test → two undrawn
  tests failed and unseal reported REAL FAILs that were harness scope
  errors. Fixed by construction: the fixture consults only
  `faults[test_id]`.
- A draw could remove the only unaltered test covering a requirement →
  `fth draw` refuses such a draw; plans carry planted-fault-disjoint redundancy.
- A hand-written dry-run fault file shape voided fram-r1 at run start
  → the stage-3 rehearsal reads a real `fth draw` output.
- Abbreviated `failed` keys broke unseal matching → predicate text
  verbatim everywhere, diffed byte-for-byte.
- Quick-look pages written by an agent could not be trusted cold →
  pages, index and VnV matrix are program-generated only.
- Rebuild could only report `DIFFERS (toolchain unpinned)` → the
  toolchain is pinned in the manifest (`--toolchain-json`).
- The recorder writes every row twice and the sender writes its own
  stray logs → dedup is the exporter's job, per file per boot segment,
  exact raw line; commands come only from `raw/cmd/`.
- `nums` scraped from event text were garbage for hex and enums → args
  are parsed with the plan's `event_patterns`.
- Run ids named after planted faults leaked the draw into the record → neutral
  run ids; grep every planted fault name over `record/`, `runner/` and the
  verdicts before shipping.

## Anti-patterns

- Pass rules in prose ("sweep completes successfully").
- Checkers that read the harness's own log of what it thinks happened.
- UUT completion events inside `complete` — a hung UUT then reads as a
  bench problem instead of a failure.
- Relaxing a predicate because the dry run failed it; that is a plan
  change (contract §2).
- A planted fault switched on by name, a hand-written fault menu, or a faults
  file the harness "tolerates".
