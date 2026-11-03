---
name: fprime-system-test-design
description: >-
  Use when designing a system-level or hardware-in-the-loop test plan
  for an F Prime component, driver, or subsystem that will be run and
  audited by agents (stage 2 of the system test process, or the
  test-design phase of component development). Produces a
  requirements-traced plan (`plan.json`, rendered to `plan.md` by
  `fth render-plan`) with a one-sentence purpose and a structured,
  machine-evaluable pass rule per test, knob-disjoint redundancy per
  requirement, numeric metrics, a fault-injection menu, and an honest
  not-covered list. Keywords: F Prime, system test, HIL, test plan,
  traceability, pass rule, oracle, fault injection, qualification,
  fth, plan.json.
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
  L0 feasibility check first — the plan's marker (`NO_OP_STRING`, or a
  tester `MARK(U32)` where the deployment has no string command) must
  round-trip byte-exact through the archive and the plan's event rate must fit
  the downlink; if either fails on the system deployment, present a
  one-line test-deployment decision with that constraint, and wait
  for the human. When the plan is written before the code exists
  (test-design phase), do the check from the deployment's
  configuration and ground-system documentation and write the plan's
  first line as `L0: desk-checked; bench confirmation pending` — the
  stage-2 refresh (§7) confirms it on the bench. A project with no
  deployment is the same human call (contract §2): ask first; without
  one, test in a temporary uncommitted deployment.
- The FPP model: ports, commands, events, telemetry and parameters
  are the observables and the fault-injection surface (knobs are
  harness configuration, §4).
- UUT code, when it exists, read after the plan's test entries (§3)
  exist, to locate observable counters. In the development lifecycle the
  plan is written before implementation (`phase-review-loop.md`,
  test-design phase) and this input is absent.
- The device datasheet named in the requirements Introduction, with
  the bench identification reply beside it. If the bench device is not
  the datasheet's device, the plan is not written: the designer sends
  the mismatch back as a requirements reopen.

## 2. Plan structure

The plan is `plan.json`; `plan.md` is rendered from it (`fth
render-plan`, byte-stable) and never edited by hand. Keys, predicate
grammar and the test entry are defined in the tooling's
`docs/plan-format.md` (pinned in `test-process-contract.md` §8);
`fth verify-plan` must pass before a plan is handed to anyone — an
unknown helper, knob or requirement or an unparsable predicate is a
load error, never a verdict.

```
0  Scope        UUT, deployment, bench, ground system, out of scope
1  Entry gates  what must be true before a run (SHAs, marker round-trip, downlink
                budget, bench recipe); stage-1 close (unit and component integration
                tests pass) is an entry gate, not an RfR test
2  Test levels  L0 provenance and feasibility · L2 functional · L3 persistence/reset
                L4 endurance/performance · L5 fault paths · L6 negative controls
                L7 final state for independent verification
3  Test entries one block per test (§3)
4  Traceability `requirements`: REQ-ID → {tests, verification, note}; redundancy rule below, or one row in the SDD's "Out-of-scope failure modes" table (no Waivers section)
   Helpers      `helpers`: [{instance, component, src, repo}] — every harness-only component of the
                test deployment (e.g. FramTester); the package renders a succinct SDD page per helper
5  Metrics      numeric thresholds with units, decided here, never relaxed later
6  Fault menu   knobs the harness must support to plant failures (§4)
7  Not covered  what this plan cannot prove on this bench, and why
8  Decision log numbered rulings with who decided and when
```

The SDD's `## Tests` table is DERIVED, never hand-edited: `fth render-sdd-tests plan.json
docs/sdd.md` rewrites it (and every requirement's Verification cell that names a system
test) between `<!-- fth:tests:begin -->` / `<!-- fth:tests:end -->` markers from the plan's
`purpose` and `covers`, in natural test-id order; commit the result on the UUT branch with
the plan. The package renderer (`render-ql --sdd`) fails when the SDD's test set differs
from the plan's, so the SDD and the VnV matrix cannot drift. What the quick-look package
must contain is the "QL package contract" in `fprime-run-for-record` §5a — design to it.

Where it lives: `plan.json`, `plan_helpers.py` and the stimulus tests are
committed in the deployment branch beside the test-helper component
(`fprime-system-test-harness` "Layout rule") — never in the tooling repo.

## 3. One test entry

`plan.json` `tests[]` entry (rendered form in parentheses):

```
id         L2-6                 # unique, ≤ 8 chars; the manifest's test list
title      Full-device write/verify sweep
purpose    One sentence: what a PASS proves, in the requirement's words.
covers     ["REQ-FRAM-003", "REQ-FRAM-007"]
setup      <state the UUT must be in>
procedure  ["MARK(60)", "bracket (2 samples)", "SWEEP(seed=0x5005, 4096, 0x5005)",
            "action: OpenOCD reset run", "MARK(61)"]      # NAME(args), bracket, wait, action:
windows    [{"name": "W", "start_id": 60, "end_id": 61, "reset": false}]
complete   ["STD"]                                        # or per window {"W1": ["STD"], "R": [...]}
all_of     ["ABS", "<predicate>", {"helper": name, "text": ..., "args": {...}}]
metrics    ["<predicate with units>"]
rationale  [{"pred": "<predicate>", "text": "why this value is ignored / masked"}]
knobs      ["verify_wrong_seed"]
evidence   <which EVR sequence or telemetry strip a human should see>
```

Rules:

- `purpose` is one sentence per test; the QL page and the SDD test
  table print it, so it names the observable outcome, not the
  procedure.
- One pass rule per test; no "verify visually", no "as expected".
  Completeness facts and UUT facts are separate blocks
  (`fprime-test-oracle` §1): a failed `complete` predicate makes the
  window `UNVERIFIED`; a failed `all_of` or `metrics` predicate in a
  complete window is `FAIL`. Never put a UUT fact in `complete` (a hung
  UUT would read as a bench problem).
- Predicates are **structured text from the grammar** of
  `docs/plan-format.md` (`event X(args) count == n`, `event A precedes
  event B`, `delta/first/last(ch)`, `last(ch) & mask == v`,
  `commands_sent == n`, `t(B) - t(CMD sent) <= N s`, the completeness
  forms, `<packet> samples >= n`) or a reference to a registered
  `@helper` (`{"helper", "text", "args"}`). Nothing else: prose, regexes
  and inline code are load errors. The predicate text is the key the
  verdict files, the knob `fails` lists and `fth unseal` match
  byte-for-byte.
- Every value a rule deliberately ignores or masks (a status bit, a
  boot-cumulative counter judged only by Δ, a tolerance) has a
  `rationale` line naming the predicate and the source of the value
  (datasheet section, bench reading confirmed at stage 2).
- A window that spans a reboot carries `"reset": true`; the plan's
  top-level `reboot_min_backstep_s` is the FSW-clock fall-back the
  exporter counts as a new boot epoch (default 1 s), and a reset
  window must hold exactly one epoch boundary. Judge the two boots with
  `segment`-aware helpers or per-boot predicates; `duration` is
  undefined across a reboot.
- **Redundancy.** Every requirement with system-test coverage is
  covered by ≥ 2 tests whose knob sets are disjoint, so no drawn knob
  can remove the prime-path verification of a requirement (`fth draw`
  refuses a draw that would). A requirement that cannot get a second
  knob-disjoint test carries an explicit waiver in its
  `requirements` entry: `verification` text naming no test (e.g. `UT
  only`, `Inspection`) and a `note` saying why.
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

The menu is **generated** from the `@knob` declarations of the plan's
helpers module (`fth knobs-menu plan.json`, `fprime-test-oracle` §5);
the designer writes the declarations, never the JSON. Knobs are
command rewrites or insertions applied by the harness to the drawn test
only, so a planted fault is invisible to the checkers and absent from
harness output. A `uut-state` knob's disturbing command does appear in
`commands.jsonl`; the auditor is forbidden to reason from it (contract
§5). Generated shape:

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
- Together with §3's redundancy rule: a knob may apply to several
  tests, but for every requirement at least one covering test must
  carry no knob that any other covering test also carries.

## 5. Hand-off

Two audiences, two artifacts:

- **The human reads the SDD only** — one document, never a separate
  pack or zip. Add a **Tests** section to
  `docs/sdd.md` that is one table and nothing else — no preamble, no
  paragraphs — with three columns: `Test` (name or ID), `Description`
  (one sentence, ≤ 15 words, on what it proves), `Requirements`
  (REQ-IDs verified). It covers the system tests of this plan and the
  unit tests (`fprime-component-unit-test`), one row each; no
  procedure, stimulus, pass-rule, metric, menu or decision-log
  columns. A REQ-ID with no test gets one row whose Description names
  the reason. That table is the test-design gate's quick-look.
- **The builder and the reviewer read the plan.** Deliver `plan.json`
  (§2 structure, one entry per test; `fth verify-plan` passes), its
  rendered `plan.md`, the helpers module with the `@knob` declarations
  and the generated fault menu, outside the component tree; it is the
  baseline the harness implements and both checkers evaluate, and it is
  not pasted into the SDD or into chat. Reviewer findings are recorded per the loop's record rule
(`phase-review-loop.md`, "Record and gate"); the dispositions and the
disagreement log are appended to §8 of the plan. The plan goes to the
human only when the loop's stop rule is met (`phase-review-loop.md`,
Bound and Rounds).

## 6. Stage-2 exit gate

Stage 2 closes only when both hold, on the bench, with the frozen plan:

1. **Mutation vectors** (`fth vectors plan.json vectors/`): one clean
   full-plan record that PASSes every test, and one single-knob record
   per knob that FAILs exactly that knob's `fails` set on its target
   (and `ripples_to`, FAIL not UNVERIFIED) while every other test
   passes. Records live under `vectors/<name>/` with `vector.json`
   (`kind: clean | single-knob | regression`) and run in CI; a vector
   that no longer judges as declared blocks the manifest.
2. **Stage-3 rehearsal**, end to end with a throwaway seed on a
   scratch manifest: `fth draw` → `fth run` → `fth audit` → `fth
   unseal` without a tooling error, the harness reading that real
   `sealed/faults.json` (fram-r1 was voided at run start because the
   harness had only ever read a hand-written dry-run fault file).

## 7. Refresh at stage 2

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
