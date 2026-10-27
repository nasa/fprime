---
name: fprime-system-test-design
description: >-
  Use when designing a system-level or hardware-in-the-loop test plan
  for an F Prime component, driver, or subsystem that will be run and
  audited by agents (stage 2 of the system test process, or the
  test-design phase of component development). Produces a
  requirements-traced plan with a machine-evaluable pass rule per test,
  numeric metrics, a fault-injection menu, and an honest not-covered
  list. Keywords: F Prime, system test, HIL, test plan, traceability,
  pass rule, oracle, fault injection, qualification.
---

# Skill: F Prime system test design

Write the plan a human approves before any test code is built, and
that the runner, auditor, and human all read later. Design from the
requirements, SDD and FPP model; open the code — if any exists — last.
Governing rules are in
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
  for the human. When the plan is written before the code exists
  (test-design phase), do the check from the deployment's
  configuration and ground-system documentation and write the plan's
  first line as `L0: desk-checked; bench confirmation pending` — the
  stage-2 refresh (§6) confirms it on the bench.
- The FPP model: ports, commands, events, telemetry and parameters
  are the observables and the fault-injection surface (knobs are
  harness configuration, §4).
- UUT code, when it exists, read after the plan's test entries (§3)
  exist, to locate observable counters. In the development lifecycle the
  plan is written before implementation (`phase-review-loop.md`,
  test-design phase) and this input is absent.

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
  `Svc.Version` payloads in the window match the manifest's
  `repos[].describe` strings: `ProjectVersion` and `FrameworkVersion`
  equal the project and framework describe; each `LibraryVersions`
  entry is `<library>@<describe>` (F Prime bakes
  `git describe --tags --always --dirty --broken` of each repository
  into the image; a tagged commit describes as the bare tag; payloads
  are 40-character strings, so the harness repo and every library
  must describe in ≤ 40 characters or the comparison is on the
  truncated prefix and the plan says so). A deployment without
  `Svc.Version` is a stage-2 finding.
- A final test leaves the UUT in a state the auditor can verify
  independently (e.g. a known pseudo-random image whose generator and
  seed are handed over).

Observability rules for pass-rule predicates (found by running the checker):

- `Svc.TlmChan` reports one value per channel per flush, so a
  false→true→false→true within one tick coalesces. Judge **transitions by
  events**, **channels by last value**; never predicate on a channel
  *sequence* across a tick.
- Count commands with `cmdDisp.OpCodeDispatched` events from the START
  marker row to the END event, not with `CommandsDispatched` deltas (the
  END-marker sample lands on either side of the window with flush phase).
- A write-on-change counter never emitted since boot has delta 0; a
  boot-zero counter first seen inside a window has baseline 0. Say which
  channels are which in the plan.
- Telemetry you predicate on must be *scheduled*: a `BufferManager`
  without `schedIn` wired emits nothing.
- A setup/re-apply that re-acknowledges settings doubles acknowledgement
  counts — bound counts "before t(SETUP.sent)" when the procedure ends
  with a restoring setup.

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

Two audiences, two artifacts:

- **The human reads the SDD only.** Add a **Tests** section to
  `docs/sdd.md` that is one table and nothing else — no preamble, no
  paragraphs — with three columns: `Test` (name or ID), `Description`
  (one sentence, ≤ 15 words, on what it proves), `Requirements`
  (REQ-IDs verified). It covers the system tests of this plan and the
  unit tests (`fprime-component-unit-test`), one row each; no
  procedure, stimulus, pass-rule, metric, menu or decision-log
  columns. A REQ-ID with no test gets one row whose Description names
  the reason. That table is the test-design gate's quick-look.
- **The builder and the reviewer read the plan.** Deliver the full plan
  (§2 structure, one entry per test) as a single markdown file plus
  the fault menu as JSON, outside the component tree; it is the
  baseline the harness implements and the checkers evaluate, and it
  is not pasted into the SDD or into chat. Reviewer findings are recorded per the loop's record rule
(`phase-review-loop.md`, "Record and gate"); the dispositions and the
disagreement log are appended to §8 of the plan. The plan goes to the
human only when the loop's stop rule is met (`phase-review-loop.md`,
Bound and Rounds).

## 6. Refresh at stage 2

A plan approved in stage 1 (before code) is refreshed when stage 2
starts on the UUT SHA: confirm L0 on the bench and replace the
desk-checked line; read the UUT code — last — for observable counters
and update §3 evidence paths where a counter is better than a report;
re-check every fault-menu knob is still applicable. The reviewer sees
the diff once (`system-test-review` Mode A on the changed entries, one
review; an empty diff needs none), then the human re-approves it
(contract §2, plan change); an empty diff is reported as such. No test
is added, removed or weakened here without the human's decision.

## Anti-patterns

- Designing from the code: tests that restate what the implementation
  does rather than what the requirement demands.
- Metrics without thresholds, or thresholds chosen after seeing data.
- "Adversary re-runs the tests" — the auditor reads the record; it
  does not generate a second one.
- Reduced deployments chosen for convenience rather than by the human.
