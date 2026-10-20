---
name: fprime-test-oracle
description: >-
  Use when writing or implementing the success condition of a system
  test as code over a recorded run: the pass-rule format, the exported
  record it reads, the completeness-versus-UUT split, the
  two-independent-checkers rule, and the verdict and divergence formats
  shared by the test designer, reviewer, builder, runner, and auditor.
  Keywords: F Prime, test oracle, pass rule, checker, verdict, record,
  fprime-yamcs, divergence.
---

# Skill: F Prime test oracle — pass rules and checkers

A **test oracle** is whatever decides that an observed result is
correct. Here it has two halves with separate names: the **pass
rule** (the condition, public, part of the approved plan) and the
**checker** (a program that evaluates pass rules over the record).
Two checkers are written independently — by the builder and by the
reviewer — so that a disagreement is two programs disagreeing about
one record, not two opinions.

---

## 1. Pass-rule format

One block per test in the plan. Predicates refer only to things that
exist in the record.

```
pass_rule:
  window: ["QUAL <run> L2-6 START", "QUAL <run> L2-6 END"]   # NO_OP_STRING markers
  complete:                        # harness and infrastructure facts only
    - both markers present
    - commands_sent == delta(cmdDisp.CommandsDispatched)      # markers included
    - delta(cmdDisp.CommandErrors) == 0                       # unless a rejection is expected
    - delta(cmdDisp.CommandsDropped) == 0
    - delta(eventLogger.EventsDropped) == 0
  all_of:                          # UUT facts
    - event SweepComplete(kind=WRITE,  pages=2048, errors=0) count == 1
    - event SweepComplete(kind=VERIFY, pages=2048, errors=0) count == 1
    - event SweepStarted(kind=WRITE) precedes event SweepStarted(kind=VERIFY)
    - event SpiError count == 0
  metrics:
    - delta(framTester.BytesVerified) == 262144
```

Rules:

- `complete` may reference only markers and infrastructure counters
  (dispatcher, event manager, downlink drop counters). UUT events
  belong in `all_of`. A missing UUT completion event in a complete
  window is `FAIL`; a failed `complete` is `UNVERIFIED`.
- Predicates are counts, sequences, equalities, and deltas over
  events, telemetry, and command completions inside the window.
- Absence predicates (`count == 0`) are valid only when `complete`
  holds.
- Thresholds carry units and come from the plan's metrics section.
- A pass rule never references the runner's, harness's, or pytest's
  own printed verdict.

## 2. Record export

Checkers read a flat export, not the live ground system, so both can
be re-run on the sealed record:

```
record/
  events.jsonl     one JSON object per line: {recv, gen, kind:"event", severity, name, args:{}}
  channels.jsonl   {recv, gen, kind:"channel", name, value}
  commands.jsonl   {sent, kind:"command", opcode, name, args:{}, completion}
  export-counts.json  {"events.jsonl": N, …} — totals the archive itself reports
  raw/             ground-system archive as stored, plus raw/flash.log
record.sha256      one line per file, produced by seal.py hash-record
```

The exporter pages through the archive completely (fprime-yamcs REST
pages at 1000 rows with a continuation token) and writes the
archive-reported totals to `export-counts.json`; `seal.py
verify-record` fails when a file's row count differs. `completion` is
the dispatcher's response (`OK`, `EXECUTION_ERROR`, …) or `NONE`.
Slice windows by `NoOpStringReceived` marker events ordered by
reception time; generation time may be uptime-based.

## 3. Checker contract

Any language; stdlib preferred. Input: `record/`, the plan's pass
rules. Output:

```
verdicts.json
  { "run": "<run id>", "record_sha256": "<sha256 of record.sha256>",
    "tests": { "L2-6": { "verdict": "PASS|FAIL|UNVERIFIED",
                         "failed": ["<predicate text>"],
                         "evidence": ["events.jsonl:1041-1049"] } } }
```

- Every test on the manifest gets an entry; a test with no window in
  the record is `UNVERIFIED` with `failed: ["window missing"]`.
- `evidence` points into the export so a quick-look can be built from
  it without re-deriving anything.
- The checker is deterministic and has no bench or network access.
- The reviewer's checker is written from the plan alone, without
  reading the builder's checker; the two are compared only by their
  `verdicts.json` on the same record.

## 4. Divergence report

```
divergence.md
| Test | Checker A | Checker B | Predicate(s) in dispute | Record lines |
```

Any row here blocks stage-2 closure (dry run) and is read first by
the human at unseal (stage 3).

## Anti-patterns

- Pass rules in prose ("sweep completes successfully").
- Checkers that read the harness's own log of what it thinks happened.
- UUT completion events inside `complete` — a hung UUT then reads as a
  bench problem instead of a failure.
- Relaxing a predicate because the dry run failed it; that is a plan
  change (contract §2).
