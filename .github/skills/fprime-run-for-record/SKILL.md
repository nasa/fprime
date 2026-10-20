---
name: fprime-run-for-record
description: >-
  Use when executing or auditing a run-for-record (stage 3 of the
  system test process): freezing the manifest, drawing the human-seeded
  planted faults, running the approved plan once, hashing the record,
  auditing it blind into quick-looks, and producing the unseal table
  the human reviews. Ships seal.py (manifest, verify, draw, seal-id,
  hash-record, verify-record, unseal). Keywords: F Prime, run for
  record, RfR, seal, manifest, fault injection, quick-look, audit,
  unseal.
---

# Skill: F Prime run-for-record

Stage 3 of `.github/agents/_shared/test-process-contract.md`. Nothing
here designs or fixes; it executes an approved plan against a frozen
UUT and harness and makes the result inspectable. `seal.py` beside
this file implements the mechanics (Python 3 stdlib only; run
`python3 seal.py selftest`). Every artifact lives in the run directory
`rfr/<run-id>/` outside both repositories; `<run-id>` ≤ 8 characters.

---

## 1. Manifest (closes stage 2)

```bash
python3 seal.py manifest --uut <path> --harness <path> --plan <plan.md> \
    --checker builder=<path> --checker reviewer=<path> \
    --image <Deployment>=<built image>:"<build command>" \
    --menu <faults-menu.json> --bench <bench.md> -o manifest.json
```

Records repo SHAs (refuses dirty trees), file hashes, the plan's test
list (parsed from its `ID:` lines, each ≤ 8 characters), and the exact
image files that will be flashed. `python3 seal.py verify manifest.json`
must pass before every later step.

## 2. Draw (human action)

The human runs this in a shell of their own. The seed comes from a
file or an un-echoed prompt — never a chat message or a command-line
argument — and is kept by the human.

```bash
python3 seal.py draw --manifest manifest.json --max-count 3 --run-dir rfr/<run-id>
```

Writes `rfr/<run-id>/sealed/{faults.json, expectations.json,
sealed.sha256}` and `rfr/<run-id>/seal-id.txt`. The number of planted
tests is itself drawn in `[1, max-count]`; the menu must cover at
least `2 × max-count` tests. Only `seal-id.txt` (and the sha8 the
command prints) is readable by runner and auditor.

## 3. Runner

1. `seal.py verify manifest.json`. Flash exactly the manifest image
   files; store `sha256sum` of each file and of the device read-back
   in `record/raw/flash.log`.
2. Start the ground system with a fresh archive for this run ID.
3. Run the plan in order, once. At start the harness emits
   `NO_OP_STRING "RFR <run> M <manifest sha8>"` and
   `"RFR <run> S <seal-id sha8>"`; every test is bracketed per
   `fprime-system-test-harness` §3. Do not read `sealed/`. Do not fix,
   tune, or retry; log every anomaly, operator action, and timing in
   `runner/report.md`. Harness stdout and pytest output go to
   `runner/`, never to `record/`.
4. Export the record (`fprime-test-oracle` §2) into `record/`, then
   `python3 seal.py hash-record --run-dir rfr/<run-id>`.
5. Run the **builder's** checker → `runner/verdicts.json`. Report per
   test PASS/FAIL/UNVERIFIED plus anomalies. Hand over the run
   directory by path; the auditor reads `record/`, `record.sha256`,
   `seal-id.txt` and nothing under `runner/` or `sealed/`.

## 4. Auditor

1. `seal.py verify manifest.json` and
   `seal.py verify-record --run-dir rfr/<run-id>`; confirm
   `record/events.jsonl` carries `RFR <run> M <sha8>` for this manifest
   and `RFR <run> S <sha8>` matching `seal-id.txt`. Any failure ends
   the audit with `RECORD INVALID`.
2. Image identity: device read-back hash in `record/raw/flash.log`
   must equal the manifest image hash (hard check). Rebuild from the
   manifest SHAs with the recorded `build_cmd` and report
   `REBUILD MATCH` or `REBUILD DIFFERS (<reason>)` — cross-compiled
   images often embed paths; a differing rebuild is reported, not
   `RECORD INVALID`.
3. Run the **reviewer's** checker → `auditor/verdicts.json`.
4. Build one quick-look per test (§5) from the record only. Replay
   the archive from a **copy** of `record/raw/`; after the last
   quick-look run `seal.py verify-record` again and print the result in
   `auditor/report.md`.
5. Perform the plan's independent final-state check after the runner
   has released the bench and before any reflash: UUT powered down or
   halted, a non-F´ tool, with tool, version, command line and result
   in `auditor/final-state.md`.
6. `auditor/report.md`: every `UNVERIFIED` window, every divergence
   from the plan's expectations, the final-state result, the rebuild
   result, and the blindness statement (contract §4). Do not open
   `runner/` or `sealed/`; do not guess which failures were planted.

## 5. Quick-look format

`auditor/quicklooks/<id>.md` plus `<id>.png` when an image is used:

```
# L2-6 — Full-device write/verify sweep
Verdict (reviewer checker): PASS
Covers: REQ-FRAM-003, REQ-FRAM-007
Pass rule: <copied from plan>
Replay: run=<run>, window=<START recv time>–<END recv time>
Evidence: ![L2-6](L2-6.png)   # EVR sequence or telemetry strip for this
                                # window only; both marker rows visible
          or a ≤ 12-line snippet from events.jsonl
Record lines: events.jsonl:1041-1049
Completeness: 14 commands sent, 14 dispatched, 0 errors, 0 events dropped
Notes: <anything a human should know; empty is fine>
```

The pass rule is on the page so the human checks the picture against
the rule, not against the auditor's word.

## 6. Unseal (human action)

```bash
python3 seal.py unseal --run-dir rfr/<run-id> --manifest manifest.json -o rfr/<run-id>/unseal.md
```

Verifies the integrity chain (contract §6), then writes the check-off
table `☐ | test | expected | runner | auditor | quick-look | outcome`
for every test on the manifest, with the human triggers listed first
and the seal id to compare with the value printed by `draw`. The human
reads trigger rows, spot-checks two or three nominal rows against the
raw record, ticks the boxes, and records the decision in the plan's
decision log.

## Anti-patterns

- A second run "to confirm" without a new run ID and a stated reason.
- Quick-looks built from the runner's report or from harness output.
- An auditor that announces which failures it believes were planted.
- A seed typed into a chat, a prompt, or a command line.
- Replaying the archive in place.
