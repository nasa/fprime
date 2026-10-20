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
| Plan | The approved system test plan (`fprime-system-test-design`). |
| Pass rule | The success condition of one test, written so a program can evaluate it over the record (`fprime-test-oracle`). |
| Checker | A program that evaluates pass rules over a record and emits verdicts. Two independent checkers exist: the builder's and the reviewer's. |
| Harness | Test-only code: tester components, integration tests, checkers, bench scripts. Never part of the UUT. |
| Record | The run-for-record archive (fprime-yamcs database, GDS logs, pytest output) plus its hash. |
| Fault menu | The approved list of fault-injection knobs the harness can apply to make a test fail on purpose. |
| Seal | The human-seeded draw of which tests get which fault, hidden from runner and auditor until unseal. |
| Quick-look | One auditor-produced page per test: verdict, evidence image or snippet from the record, pass rule, notes. |

## 1. Three stages, three human gates

| Stage | Who iterates | Human gate | Close condition | Hand-off |
|---|---|---|---|---|
| 1. Base development | `fprime-development.agent` with review and unit-test skills (`fprime-iterative-development` when approved) | Requirements + FPP design approval **before** iteration | No must-fix findings, unit and component integration tests pass | UUT SHA |
| 2. System-test development | `system-test-design` ↔ `system-test-review` on the plan; then `system-test-build` ↔ `system-test-review` on the harness, with a dry run | Test-plan approval **before** building; harness PR review **before** the seal | Plan has no must-fix, both checkers agree on an unsealed dry run of the full plan (every fault knob exercised) | Manifest (§3) |
| 3. Run-for-record (RfR) | `test-runner` then `test-auditor`, both blind | Human seeds the seal at start; human reviews the unseal table and quick-looks at the end | Unseal table produced | Record + runner report + auditor quick-looks |

Each stage is a separate human-driven prompt. Stages never run
concurrently on the same UUT SHA.

## 2. Standing rules

- **Requirements first.** Tests derive from requirements and the SDD;
  the UUT code is read afterwards, to find fault-injection points, not
  to decide what to test.
- **Default to the system deployment.** Tests run against the
  project's full deployment on a complete testbed. A reduced or
  sibling test deployment is a **human call** — propose it with the
  testbed constraint that forces it and wait for the decision.
- **Pass rules are code, not judgment.** Every test in the plan has a
  machine-evaluable pass rule before anything is built. "The tester
  verified it" is never a verdict.
- **Two independent checkers.** The builder implements the pass rules;
  the reviewer implements them again without reading the builder's
  code. A verdict is only reportable when both exist.
- **The record is the evidence.** Screenshots, snippets and tables are
  derived from the hashed record by the auditor, never supplied by the
  runner.
- **Stage 2 never edits the UUT.** A UUT defect found while building
  or dry-running tests is reported and goes back through stage 1 as a
  minimal iteration on the fix; the harness stage waits.
- **Bounded loops.** Plan review: at most three designer/reviewer
  rounds, then escalate with the disagreement log. Harness review:
  five rounds, then escalate (as `fprime-iterative-development` §3).
  Agreement reached by softening a must-fix is recorded as a
  disagreement, not a resolution.
- **Never weaken a test to make it pass.** Changing a pass rule,
  threshold, or metric after plan approval is a plan change and
  returns to the human gate with a diff.

## 3. Hand-off artifacts

Stage 2 → stage 3 delivers exactly one **manifest** (`seal.py manifest`,
see `fprime-run-for-record`):

```
manifest.json
  uut:      { repo, sha, dirty: false }
  harness:  { repo, sha, dirty: false }
  plan:     { path, sha256, revision }
  checkers: [ { owner: builder|reviewer, path, sha256 } ]
  menu:     { path, sha256 }
  bench:    { path, sha256 }        # bench recipe: wiring, ports, power control, ground system
```

RfR refuses to start if any entry fails `seal.py verify`. Anything
not on the manifest is not under test.

Stage 3 delivers to the human: `record/` + `record.sha256`,
`runner/report.md` + `runner/verdicts.json`, `auditor/quicklooks/` +
`auditor/verdicts.json`, and `unseal.md`.

## 4. Blindness

| Party | Must not see | Enforcement |
|---|---|---|
| Runner | `sealed/` contents (the draw and expectations) | Procedural: the harness reads `sealed/faults.json` itself; the runner never opens, prints, or diffs that directory. The harness writes the sealed hash into the record at run start so the auditor can prove which draw ran. |
| Auditor | `sealed/`, `runner/verdicts.json`, `runner/report.md` | Procedural: the auditor's inputs are the manifest, the plan, the record, and the reviewer's checker. END markers in the record carry **no verdict**. |
| Designer, reviewer, builder | The draw | The draw happens after their work is frozen, from a seed only the human knows. |

Blindness here is procedural, not cryptographic: every agent shares a
model lineage and a machine. The seal is tamper-evident (hashes in
the record and manifest), not tamper-proof. State this in every RfR
report.

## 5. Role conduct

- **Designer** writes the plan per `fprime-system-test-design`; owns
  traceability and the fault menu.
- **Reviewer** is adversarial: finds what the plan would miss and what
  the harness would pass by construction; writes the independent
  checker; keeps the disagreement log.
- **Builder** keeps the harness small enough to be read in one sitting
  and reuses `test/int/` reusable integration tests and standard
  components before writing new code.
- **Runner** verifies the manifest, runs the plan exactly once per run
  ID, fixes nothing, retries nothing silently, hashes the record, and
  reports per-test PASS/FAIL from the builder's checker plus every
  anomaly it noticed.
- **Auditor** verifies hashes first, runs the reviewer's checker,
  builds one quick-look per test from the record, marks tests whose
  window is incomplete `UNVERIFIED`, and reports divergences from the
  plan's expectations without knowing the runner's verdicts.

## 6. Unseal and human triggers

`seal.py unseal` joins sealed expectation | runner verdict | auditor
verdict per test. The human reads disagreeing rows first, then
spot-checks two or three agreeing rows against the raw record.

| Outcome | Meaning | Action |
|---|---|---|
| Expected PASS, both PASS | Nominal | Spot-check |
| Expected FAIL (planted), both FAIL | Verdict machinery works | None |
| Expected FAIL, any PASS | Harness or checker cannot detect that fault | RfR void; back to stage 2 |
| Expected PASS, any FAIL | Real UUT or bench defect, or checker bug | Human triage; UUT fixes go to stage 1 |
| Runner ≠ auditor | Checkers disagree | Human reads both quick-look and runner evidence |

## 7. Forbidden actions

- Posting any stage-2 or stage-3 output to GitHub as reviews, comments
  or statuses. Plans, findings, reports and quick-looks are files
  delivered to the human (a PR for harness code is a human decision).
- Running RfR on a UUT, harness, or plan whose SHA is not on the
  manifest or whose tree is dirty.
- Rerunning a failed RfR test "to see if it passes". A rerun is a new
  run ID with its own record and a stated reason.
- Fixing anything during stage 3.
- Modifying the record, the manifest, or `sealed/` after the seal.
- Letting a runner or auditor context contain the draw.
