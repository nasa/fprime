---
name: fprime-run-for-record
description: >-
  Use when executing or auditing a run-for-record (stage 3 of the
  system test process): freezing the manifest, sealing the human-seeded
  fault draw, running the approved plan once, hashing the record,
  auditing it blind into quick-looks, and producing the unseal table
  the human reviews. Ships seal.py for manifest, draw, verify,
  hash-record, and unseal. Keywords: F Prime, run for record, RfR,
  seal, manifest, fault injection, quick-look, audit, unseal.
---

# Skill: F Prime run-for-record

Stage 3 of `.github/agents/_shared/test-process-contract.md`. Nothing
here designs or fixes; it executes an approved plan against a frozen
UUT and harness, and makes the result inspectable. `seal.py` beside
this file implements the mechanics (Python 3 stdlib only; run
`python3 seal.py selftest`).

---

## 1. Manifest (closes stage 2)

```bash
python3 seal.py manifest --uut <path> --harness <path> --plan <plan.md> \
    --checker builder=<path> --checker reviewer=<path> \
    --menu <faults-menu.json> --bench <bench.md> -o manifest.json
```

Records repo SHAs (refuses dirty trees), file hashes, and the plan
revision. `python3 seal.py verify manifest.json` must pass before every
later step.

## 2. Seal (human action)

The human supplies the seed; nobody else sees it.

```bash
python3 seal.py draw --manifest manifest.json --seed <human seed> --count 3 -o sealed/
```

Writes `sealed/faults.json` (test → knob, read by the harness),
`sealed/expectations.json` (test → PASS|FAIL), and
`sealed/sealed.sha256`. The human keeps the seed; the `sealed/`
directory travels with the run but is **not opened** by runner or
auditor (contract §4).

## 3. Runner

1. `seal.py verify manifest.json`; flash/deploy exactly the UUT and
   harness SHAs; confirm the flashed image hash.
2. Start the ground system with a fresh archive for this run ID.
3. Run the plan in order, once. Markers per `fprime-system-test-harness`
   §3. Do not read `sealed/`. Do not fix, tune, or retry; log every
   anomaly (bench glitch, operator action, timing) in `runner/report.md`.
4. Export the record (`fprime-test-oracle` §2) and
   `python3 seal.py hash-record record/`.
5. Run the **builder's** checker → `runner/verdicts.json`. Report per
   test PASS/FAIL/UNVERIFIED plus anomalies. Hand off `record/`,
   `record.sha256`, `runner/` to the auditor **without** `runner/`
   being readable to it (separate directory, stated in the hand-off).

## 4. Auditor

1. Verify `record.sha256` against `record/`; verify the manifest and
   that the record's `RFR … MANIFEST … SEALED …` marker matches both
   hashes. Any mismatch ends the audit with `RECORD INVALID`.
2. Rebuild the UUT and harness from the manifest SHAs and compare the
   image hash with the one in the runner's flash log and, where
   possible, with the device (`verify_image`).
3. Run the **reviewer's** checker → `auditor/verdicts.json`.
4. Build one quick-look per test (§5) from the record only.
5. Perform the plan's independent final-state check (e.g. dump the
   device and compare with the regenerated expected image).
6. Report divergences from the plan's expectations and every
   `UNVERIFIED` window. Do not open `runner/` or `sealed/`.

## 5. Quick-look format

One page per test, `auditor/quicklooks/<id>.md`:

```
# L2-6 — Full-device write/verify sweep        Verdict: PASS
Covers: REQ-FRAM-003, REQ-FRAM-007
Pass rule: <copied from plan>
Evidence: <image of the EVR sequence or telemetry strip, taken from
          the replayed archive, window L2-6 only; or a ≤ 12-line snippet>
Record lines: events.csv:1041-1049
Completeness: 14 commands sent, 14 completed
Notes: <anything a human should know; empty is fine>
```

Images come from the ground system replaying the sealed archive, not
from the live run. The pass rule is on the page so the human checks
the picture against the rule, not against the auditor's word.

## 6. Unseal (human action)

```bash
python3 seal.py unseal --sealed sealed/ --runner runner/verdicts.json \
    --auditor auditor/verdicts.json -o unseal.md
```

Produces the table `test | expected | runner | auditor | outcome` with
outcomes per contract §6 and the human triggers at the top. The human
reads disagreeing rows, spot-checks two or three agreeing rows in the
raw record, and records the decision in the plan's decision log.

## 7. Reporting rule

Every RfR report states: blindness is procedural, the seal is
tamper-evident not tamper-proof, and which checks were outside the
UUT's own reporting path.

## Anti-patterns

- A second run "to confirm" without a new run ID and a stated reason.
- Quick-looks built from the runner's report instead of the record.
- An auditor that announces which failures it believes were planted.
- Expectations or faults chosen by an agent instead of drawn from a
  human seed.
