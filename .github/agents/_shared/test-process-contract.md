# F Prime System Test Process — Shared Contract

This file is the **single source of truth** for how the system-test
agents behave and hand work to each other. Every agent in the flow —
`system-test-design`, `system-test-review`, `system-test-build`,
`test-runner`, `test-auditor` — opens its body with "Apply the test
process contract in `_shared/test-process-contract.md`." and follows
the rules below. If a per-agent file disagrees with this contract, the
contract wins.

The flow exists to make AI-run testing **inspectable by a human who
did not run it**: the human reviews a plan and a small harness up
front, then reads a verdict table and quick-looks at the end. Scale
comes from the agents; trust comes from the hand-offs below.

---

## 0. Terms

| Term | Meaning |
|---|---|
| UUT | Unit under test: the component, driver, or subsystem being qualified, identified by a git SHA. |
| Plan | The approved system test plan (`fprime-system-test-design`). Each test has an `ID:` of ≤ 8 characters. |
| Pass rule | The success condition of one test, written so a program can evaluate it over the record (`fprime-test-oracle`). |
| Checker | A program that evaluates pass rules over the record and emits verdicts. Two exist, written independently: the builder's and the reviewer's. |
| Harness | Test-only code: tester components, test procedures, checkers, bench scripts. Never part of the UUT. |
| Complete testbed | The bench recipe names every device, link, power control and ground system of the mission configuration; any build whose topology differs from the mission's is a *test deployment*. |
| Record | `record/` = ground-system archive (`raw/`), its flat export, and the flash log — plus `record.sha256`. Harness stdout and pytest output are **not** record; they live in `runner/`. |
| Fault menu | The approved list of fault-injection knobs the harness can apply to make a test fail on purpose, with its `max_count`, each knob's `class`, the predicates it `fails`, and the tests it `ripples_to`. |
| Draw / seal | The human-seeded choice of which tests get which knob, stored under `sealed/`. The seal is **procedural and tamper-evident**, not cryptographic (§4). |
| Seal id | `sha256` of `sealed/sealed.sha256`, written to `seal-id.txt` beside `sealed/`. The only seal artifact runner and auditor may read. |
| Run directory | `rfr/<run-id>/` outside both repositories, `<run-id>` ≤ 8 characters; holds `sealed/`, `seal-id.txt`, `record/`, `record.sha256`, `runner/`, `auditor/`, `unseal.md`. The manifest lives beside the runs as `rfr/manifest-<uut sha8>.json`. |
| Quick-look | One auditor-produced page per test: reviewer-checker verdict, evidence image or snippet from the record, pass rule, provenance. |

## 1. Three stages, three human gates

| Stage | Who iterates | Human gate | Close condition | Hand-off |
|---|---|---|---|---|
| 1. Base development | `fprime-development.agent` with review and unit-test skills (`fprime-iterative-development` when approved); requirements, FPP design and test design (system test plan, fault menu, unit-test list) each pass the loop of `phase-review-loop.md` | Requirements, FPP design and test-design approval **before** iteration | No must-fix findings; unit and component integration tests pass | UUT SHA |
| 2. System-test development | `system-test-design` ↔ `system-test-review` on the plan (standalone, when stage 1 produced no plan); then `system-test-build` ↔ `system-test-review` on the harness, closing with an unsealed dry run | Test-plan approval **before** building; harness review (local files; a PR only if the human asks) **before** the manifest is frozen | Plan has no open must-fix; dry run of the full plan on the real bench with every knob exercised once; both checkers agree on every verdict | Manifest (§3) |
| 3. Run-for-record (RfR) | `test-runner` then `test-auditor`, both blind | Human seeds the draw at start; human reviews `unseal.md` and the quick-looks at the end | `unseal.md` produced | Run directory |

Each stage is a separate human-driven prompt. Stages never run
concurrently on the same UUT SHA.

## 2. Standing rules

- **Requirements first.** Tests derive from requirements and the SDD;
  the UUT code is read afterwards, to find fault-injection points and
  observable counters, not to decide what to test.
- **Default to the system deployment** on the complete testbed. A
  test deployment is a **human call**: the designer runs the L0
  feasibility check (markers round-trip byte-exact, event budget fits
  the downlink) and presents a one-line decision with the constraint
  that forces it.
- **Pass rules are code, not judgment.** Every test has a
  machine-evaluable pass rule before anything is built. "The tester
  verified it" is never a verdict. Completeness facts (markers,
  dispatcher and event-drop counters) and UUT facts (completion
  events) are separate: a missing UUT event in a complete window is
  `FAIL`, never `UNVERIFIED`.
- **Two independent checkers.** The builder implements the pass rules;
  the reviewer implements them again without reading the builder's
  code. A verdict is reportable only when both exist.
- **The record is the evidence.** Quick-looks are derived from the
  hashed record by the auditor, never supplied by the runner. The
  procedure drives and records; it never decides.
- **Stage 2 never edits the UUT.** A UUT defect found while building
  or dry-running is reported with the record lines that show it and
  goes back through stage 1 as a minimal iteration; harness work waits.
- **Bounded loops.** Plan review: three designer/reviewer rounds by
  default, `review rounds: N` at kickoff overrides
  (`phase-review-loop.md`), then escalate with the disagreement log. Harness review:
  five rounds, then escalate. Agreement reached by softening a
  must-fix is recorded as a disagreement, not a resolution.
- **Never weaken a test to make it pass.** Changing a pass rule,
  threshold, metric, or the test list after plan approval is a plan
  change and returns to the human gate with a diff.

## 3. Hand-off artifacts

Stage 2 → stage 3 delivers exactly one **manifest** (`seal.py manifest`,
`fprime-run-for-record` §1):

```
manifest.json
  uut:      { path, sha, describe, dirty: false }
  harness:  { path, sha, describe, dirty: false }
  repos:    [ { name, path, sha, describe } ]     # uut + every repository the image is built from
                                                  # (project, fprime, libraries); clean trees only
  plan:     { path, sha256, revision, tests: [ "L0-1", … ] }   # parsed from the plan's `ID:` lines (column 0, unique)
  checkers: [ { owner: builder|reviewer, path, sha256 } ]
  images:   [ { deployment, path, sha256, build_cmd, version_check } ]
            # raw .bin written to flash; each must contain every repos[].describe string
            # (what Svc.Version reports as PROJECT / FRAMEWORK / LIBRARY versions)
  no_version_check: ""                            # reason, if the image check was skipped; shown at unseal
  menu:     { path, sha256 }
  bench:    { path, sha256 }        # bench recipe: wiring, ports, power control, ground system
```

RfR refuses to start if `seal.py verify` fails. Anything not on the
manifest is not under test. All RfR artifacts live in the run
directory, never inside either repository (the dirty-tree check would
otherwise trip on them).

Stage 3 delivers the run directory to the human: `record/` +
`record.sha256`, `runner/report.md` + `runner/verdicts.json`,
`auditor/quicklooks/` + `auditor/verdicts.json` +
`auditor/final-state.{json,md}` + `auditor/report.md`, and
`unseal.md`.

## 4. Blindness

| Party | Must not see | Enforcement |
|---|---|---|
| Every agent, including any orchestrator | The seed | The human runs `seal.py draw` in a shell of their own; the seed comes from a file or an un-echoed prompt, never a chat message or command line. The draw is reproducible from seed + manifest, so a leaked seed is a leaked draw. |
| Runner | `sealed/` contents | The harness reads `sealed/faults.json` itself and never logs, prints, asserts on, or writes which knobs are active, to any file or stream; the runner never opens, prints, or diffs that directory. The harness writes the manifest sha8 and the seal id sha8 into the record at run start (`RFR <run> M <sha8>`, `RFR <run> S <sha8>`) — the only trace of the draw outside `sealed/`. |
| Auditor | `sealed/`, `runner/` | The auditor's inputs are the manifest, the plan, `record/` + `record.sha256`, `seal-id.txt`, and the reviewer's checker. END markers carry **no verdict**. Whoever hands over the run directory passes paths, never runner text. |
| Designer, reviewer, builder | The draw | The draw happens after the manifest is frozen. |

Runner, auditor, and the shell that performed the draw are **three
distinct contexts**; one session never plays two of these roles for
the same run. Blindness is procedural: every agent shares a model
lineage and `faults.json` is plain text on the bench host. The seal is
tamper-evident (hashes in the record, the manifest, and `seal-id.txt`),
not tamper-proof. Every RfR report says so.

## 5. Role conduct

- **Designer** writes the plan per `fprime-system-test-design`; owns
  traceability, the L0 feasibility check, and the fault menu.
- **Reviewer** is adversarial: finds what the plan would miss and what
  the harness would pass by construction; writes the independent
  checker; keeps the disagreement log.
- **Builder** keeps the harness small enough to be read in one sitting
  and reuses `test/int/` reusable integration tests and standard
  components before writing new code.
- **Runner** verifies the manifest, runs the plan exactly once per run
  ID, fixes nothing, retries nothing silently, hashes the record, and
  reports per-test PASS/FAIL/UNVERIFIED from the builder's checker
  plus every anomaly it noticed.
- **Auditor** verifies hashes first, runs the reviewer's checker,
  builds one quick-look per test from the record, performs the plan's
  independent final-state check, and reports divergences from the
  plan's expectations without knowing the runner's verdicts.

## 6. Unseal and human triggers

`seal.py unseal` checks the integrity chain (seal id ↔ `sealed/`,
draw ↔ manifest, record ↔ `record.sha256`, run id ↔ both verdict
files, markers in the record ↔ both hashes) and then joins, for
**every test on the manifest** plus the independent final-state row
(`auditor/final-state.json`), sealed expectation | runner verdict |
auditor verdict | quick-look. Planted expectations are `FAIL` for the
drawn test and `FAIL-RIPPLE` for the tests its knob `ripples_to`.
Precedence, first match wins:

| Outcome | Meaning | Action |
|---|---|---|
| Expected FAIL / FAIL-RIPPLE, any PASS or MISSING | Harness or checker cannot detect that fault, or the knob did not disturb state | **RfR void**; back to stage 2 |
| `<test>/dump` row (independent final-state check) | Expectation comes from the knob **class**: FAIL only for a drawn `uut-state` knob on that test, FAIL-RIPPLE for its ripples, else PASS — a harness-expectation or stimulus knob leaves the device correct | Same precedence as above |
| Expected FAIL, both FAIL, failed predicates ⊄ knob `fails` | Planted fault **and** an unexplained failure | Human triage of the extra predicates |
| Expected FAIL / FAIL-RIPPLE, both FAIL | Verdict machinery works | None |
| Expected FAIL / FAIL-RIPPLE, both UNVERIFIED | Planted window incomplete | Harness/bench triage |
| Expected FAIL / FAIL-RIPPLE, FAIL vs UNVERIFIED | Checkers disagree on a planted fault | Human reads both |
| Any MISSING (including the final-state row) | A test on the manifest was not reported, or the independent check was not performed | Human triage |
| Runner ≠ auditor | Checkers disagree | Human reads both quick-look and runner evidence |
| Both PASS | Nominal | Spot-check two or three rows against the raw record |
| Both UNVERIFIED | Window incomplete (markers, drops) | Bench/harness triage; not a UUT verdict |
| Both FAIL | Real UUT or bench defect, or shared checker bug | Human triage; UUT fixes go to stage 1 |

A rerun before unseal may reuse `sealed/` only with fresh runner and
auditor contexts and a new run ID; after unseal a rerun needs a new
draw.

## 7. Forbidden actions

Agents cite this list; they do not restate it.

- Posting any stage-2 or stage-3 output to GitHub as reviews, comments
  or statuses. Plans, findings, reports and quick-looks are files
  delivered to the human (a PR for harness code is a human decision).
- Running RfR on a UUT, harness, plan, or image whose hash is not on
  the manifest, or from a dirty tree.
- Rerunning a failed RfR test "to see if it passes". A rerun is a new
  run ID with its own record and a stated reason (§6).
- Fixing anything during stage 3.
- Modifying the record, the manifest, `sealed/`, or `seal-id.txt`
  after the draw. Replay for quick-looks works on a copy of `raw/`.
- Logging, printing, or writing which knobs are active anywhere
  outside `sealed/`.
- Letting a runner or auditor context contain the seed, the draw, or
  the other party's verdicts.
- Pasting runner output, harness stdout, or pytest output into the
  record or to the auditor.
