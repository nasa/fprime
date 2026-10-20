---
name: fprime-system-test-design
description: >-
  Use when designing a system-level or hardware-in-the-loop test plan
  for an F Prime component, driver, or subsystem that will be run and
  audited by agents (stage 2 of the system test process). Produces a
  requirements-traced plan with a machine-evaluable pass rule per test,
  numeric metrics, a fault-injection menu, and an honest not-covered
  list. Keywords: F Prime, system test, HIL, test plan, traceability,
  pass rule, oracle, fault injection, qualification.
---

# Skill: F Prime system test design

Write the plan a human approves before any test code is built, and
that the runner, auditor, and human all read later. Design from the
requirements and SDD; open the code last. Governing rules are in
`.github/agents/_shared/test-process-contract.md`.

---

## 1. Inputs

- Human goals for the qualification (one paragraph, quoted verbatim).
- Requirements list (REQ-IDs) and `docs/sdd.md` of the UUT.
- The deployment the tests run in. **Default: the project's full
  system deployment on the complete testbed.** Propose a reduced or
  sibling deployment only when a testbed constraint forces it, state
  the constraint, and wait for the human's decision.
- UUT code, read after the test list exists, to locate fault-injection
  points and observable counters.

## 2. Plan structure

```
0  Scope        UUT, deployment, bench, ground system, out of scope
1  Entry gates  what must be true before a run (SHAs, downlink health, bench)
2  Test levels  L0 provenance · L1 unit/int tests run and pass · L2 functional
                L3 persistence/reset · L4 endurance/performance · L5 fault paths
                L6 negative controls · L7 final state for independent verification
3  Test entries one block per test (§3)
4  Traceability REQ-ID → test IDs; every REQ has ≥ 1 test or a stated waiver
5  Metrics      numeric thresholds with units, decided here, never relaxed later
6  Fault menu   knobs the harness must support to plant failures (§4)
7  Not covered  what this plan cannot prove on this bench, and why
8  Decision log numbered rulings with who decided and when
```

## 3. One test entry

```
ID:        L2-6
Title:     Full-device write/verify sweep
Covers:    REQ-FRAM-003, REQ-FRAM-007
Setup:     <state the UUT must be in>
Procedure: <commands sent, in order>
Window:    NO_OP_STRING "QUAL <run> L2-6 START" … "QUAL <run> L2-6 END"
Pass rule: <machine-evaluable, per fprime-test-oracle>
Evidence:  <which EVR sequence or telemetry strip a human should see>
Fault knobs: <menu entries that may be applied to this test>
```

Rules:

- One pass rule per test; no "verify visually", no "as expected".
- Prefer EVR sequences a human can check off (start → progress →
  complete with counts); telemetry strips when counters are the
  evidence; never prose from the runner.
- Negative controls are real tests: a request that must be rejected
  has a pass rule that the rejection event appears **and** nothing
  else happened (counters unchanged, no bus traffic).
- Include at least one test whose evidence lies outside the UUT's own
  reporting path (independent dump, external instrument, datasheet
  constant) so a UUT that lies about itself is caught.
- Every expected constant cites its source (datasheet section, ICD).
- A final test leaves the UUT in a state the auditor can verify
  independently (e.g. a known pseudo-random image whose generator and
  seed are handed over).

## 4. Fault menu

The menu lists knobs the harness applies **through its own
configuration**, so a planted fault is indistinguishable from a real
one in the record:

```
{ "knobs": [
  { "name": "verify_wrong_seed", "applies_to": ["L2-6", "L7-1"],
    "expected": "FAIL", "mechanism": "harness verifies against seed+1" },
  { "name": "expected_id_wrong", "applies_to": ["L0-6"],
    "expected": "FAIL", "mechanism": "harness loads an off-by-one device ID" }
] }
```

Each knob names the tests it may be applied to and why the test must
then fail. Knobs that need a different build, a different test ID, or
a visible flag are not acceptable. Three to five knobs covering
different failure classes is usually enough; the draw (stage 3) picks
a subset.

## 5. Hand-off

Deliver the plan as a single markdown file plus the fault menu as
JSON. The reviewer's must-fix / could-fix findings and the
disagreement log are appended to §8 of the plan, not kept elsewhere.
The plan goes to the human only when the reviewer has no open
must-fix or the round limit (contract §2) is reached.

## Anti-patterns

- Designing from the code: tests that restate what the implementation
  does rather than what the requirement demands.
- Metrics without thresholds, or thresholds chosen after seeing data.
- "Adversary re-runs the tests" — the auditor reads the record; it
  does not generate a second one.
- Reduced deployments chosen for convenience rather than by the human.
