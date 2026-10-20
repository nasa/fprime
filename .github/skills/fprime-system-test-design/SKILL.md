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
  system deployment on the complete testbed** (contract §0). Run the
  L0 feasibility check first — a `NO_OP_STRING` marker must round-trip
  byte-exact through the archive and the plan's event rate must fit
  the downlink; if either fails on the system deployment, present a
  one-line test-deployment decision with that constraint, and wait
  for the human.
- UUT code, read after the test list exists, to locate fault-injection
  points and observable counters.

## 2. Plan structure

```
0  Scope        UUT, deployment, bench, ground system, out of scope
1  Entry gates  what must be true before a run (SHAs, marker round-trip, downlink
                budget, bench recipe); stage-1 close (unit and component integration
                tests pass) is an entry gate, not an RfR test
2  Test levels  L0 provenance and feasibility · L2 functional · L3 persistence/reset
                L4 endurance/performance · L5 fault paths · L6 negative controls
                L7 final state for independent verification
3  Test entries one block per test (§3)
4  Traceability REQ-ID → test IDs; every REQ has ≥ 1 test or a stated waiver
5  Metrics      numeric thresholds with units, decided here, never relaxed later
6  Fault menu   knobs the harness must support to plant failures (§4)
7  Not covered  what this plan cannot prove on this bench, and why
8  Decision log numbered rulings with who decided and when
```

## 3. One test entry

```
ID:        L2-6                      # at column 0, one per test, unique, ≤ 8 chars; parsed into the manifest
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
  Completeness facts and UUT facts are separate blocks
  (`fprime-test-oracle` §1).
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
- L0 includes an image-identity test: its procedure sends
  `version.VERSION ALL`; its pass rule is that the
  `Svc.Version` `ProjectVersion`, `FrameworkVersion` and
  `LibraryVersions` payloads in the window equal the manifest's
  `repos[].describe` strings (F Prime bakes
  `git describe --tags --always --dirty --broken` of the project,
  framework and each library into the image; a tagged commit
  describes as the bare tag). A deployment without `Svc.Version` is a
  stage-2 finding.
- A final test leaves the UUT in a state the auditor can verify
  independently (e.g. a known pseudo-random image whose generator and
  seed are handed over).

## 4. Fault menu

The menu lists knobs the harness applies **through its own
configuration**, so a planted fault is invisible to the checkers and
absent from harness output. A `uut-state` knob's disturbing command
does appear in `commands.jsonl`; the auditor is forbidden to reason
from it (contract §5):

```
{ "max_count": 2,
  "knobs": [
  { "name": "verify_wrong_seed", "class": "harness-expectation",
    "applies_to": ["L2-6", "L7-1"],
    "fails": ["event SweepComplete(kind=VERIFY, pages=2048, errors=0) count == 1"],
    "mechanism": "harness verifies against seed+1" },
  { "name": "corrupt_page", "class": "uut-state",
    "applies_to": ["L2-6", "L3-1"], "ripples_to": ["L7-1"],
    "fails": ["event SweepComplete(kind=VERIFY, pages=2048, errors=0) count == 1"],
    "mechanism": "harness overwrites one page between WRITE and VERIFY" },
  { "name": "short_write", "class": "stimulus",
    "applies_to": ["L2-2"],
    "fails": ["delta(framTester.BytesVerified) == 4096"],
    "mechanism": "harness sends length-1 and still expects the full length" }
] }
```

- `max_count` is the most tests the stage-3 draw may plant; the menu
  must cover at least `2 × max_count` tests, and the draw picks a
  hidden number in `[1, max_count]`.
- `class`: `harness-expectation` (the checker's expectation is wrong),
  `uut-state` (device content or state is disturbed), `stimulus` (the
  drive differs from the plan). At least one knob is `uut-state`:
  expectation-only knobs prove the checker reads the harness's error
  event, not that the test catches a defect.
- `fails`: the pass-rule predicates, verbatim, that the knob breaks.
  At unseal any other failed predicate on a planted test is an
  unexplained failure and a human trigger.
- `ripples_to`: tests that legitimately fail when this knob is
  planted (a disturbed device stays disturbed), as a list or as a map
  `{"L7-1": ["<predicate>"]}` naming the predicates that fail there
  (map form enables the unexplained-failure check on ripples).
  Verified in the dry run; the draw expects `FAIL-RIPPLE` for them.
- Only a `uut-state` knob changes what the independent final-state
  dump should find; a `harness-expectation` or `stimulus` knob drawn
  on the final test leaves the device correct and the dump is
  expected to PASS.
- Knobs that need a different build, a different test ID, or a
  visible flag are not acceptable.

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
