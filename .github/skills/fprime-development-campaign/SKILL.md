---
name: fprime-development-campaign
description: >-
  Orchestrate one F Prime component or feature from request to a tool-built
  run-for-record quick-look package in four lanes — DESIGN → DEVELOPMENT →
  RELEASE → RUN-FOR-RECORD — with two human reviews (Approve SDD, Read QL)
  and two human inputs (Request + goals, Seed), or with those gates
  delegated to a project campaign. Thin wrapper over the lane skills and
  the test process contract. Use for any component campaign, standalone or
  as a child of fprime-project-campaign. Keywords: F Prime, development
  campaign, lanes, SDD, run for record, quick-look, seed, ledger, fth.
---

# Skill: F Prime development campaign

Takes an F Prime component or feature from request to a tool-built
run-for-record (RfR) quick-look (QL) package in four lanes with exactly
two human reviews and two human inputs. This skill is a thin wrapper: it
names the lanes, the skills and agents that carry each lane, how the work
splits into sub-sessions, and the approver of each gate. **The skills are
the how**; no tool command, file layout or pass rule is defined here, and
if this skill and a lane skill disagree, the lane skill wins.

Process source of truth: `.github/agents/_shared/test-process-contract.md`
(§1 lanes and gates, §2 standing rules, §8 tooling pin, §9 project level).
Read it first, every time.

## 0. Inputs

| Input | Required | Content |
|---|---|---|
| Request + goals | yes | What to build or change; the UUT repository; the test-deployment repository when it is not the UUT repository. Default goal: *a functional component with respect to the request, proven in system tests — on the real hardware when the component is hardware-driven*; extra goals only for proofs beyond it. Carried into the plan verbatim. |
| Test configuration | yes | The deployment the system tests run in, as `repo@ref` (contract §0 "complete testbed"; in a project campaign, the base deployment). |
| Bench facts | yes | Target platform, UUT device path(s), the human's per-device power control, ground system, foreign devices not to be touched. Bench access and conduct come from the project's bench procedure; this line supplies only what it cannot know. |
| Approver | no | `human` (default) or `project-campaign` (contract §9.2). |
| Seed | later | Requested only when the RUN-FOR-RECORD lane is about to start; `human` or `derived` per contract §9.3. Never sent with the request. |
| Review bound | no | `review rounds: N` for the DESIGN loops (default per `_shared/phase-review-loop.md`). |

## 1. Setup (anchor session)

1. Read fresh from `nasa/fprime` `devel`: the contract,
   `_shared/phase-review-loop.md`, `fprime-development.agent.md`, and the
   lane skills below. Clone the UUT and deployment repositories.
2. Identities and bench access follow the repositories' workflows and the
   project's bench procedure; every child session that touches the bench is
   bound by them.
3. Confirm the bench facts read-only (device identity per the datasheet,
   device ownership) and send the human the contract's needs checklist
   marked known/unknown — do not wait for it.
4. Open the **campaign ledger** (decision log) that every lane appends to;
   it travels with the delivery and is the only LLM-written deliverable.

## 2. DESIGN lane — ends at gate 1, **Approve SDD**

5. `fprime-development.agent.md` §2 phases 1–3 with the review loop of
   `phase-review-loop.md`: requirements (`fprime-component-requirements`,
   reviewed by `requirements-review`); FPP + SDD design
   (`fprime-component-design-fpp`, `fprime-topology-development` when a
   topology changes; reviewed by `design-review`, `architecture-review`,
   `design-principles-review`); test design (`fprime-system-test-design`,
   unit-test list per `fprime-component-unit-test`; reviewed by
   `system-test-review` Mode A). Coverage targets and the image-version
   policy are set in the plan here.
6. Sub-sessions: one reviewer child per reviewer agent, reused across
   rounds, LOCAL ONLY. Elicitation findings are questions to the human,
   batched once per round.
7. **Gate 1 — Approve SDD (block).** Hand the approver `docs/sdd.md` as the
   single document (requirements table, design, Tests table); plan, fault
   menu and review records as named attachments. Record the ruling in the
   ledger. A rejection redoes the affected phase and returns here.

## 3. DEVELOPMENT lane — no gate

8. Code + unit tests: `fprime-iterative-development` Phase 2 entered at
   implementation (its §0 approval is the SDD approval), with
   `fprime-component-implementation`, `fprime-cpp-design`,
   `fprime-component-unit-test` / `fprime-unit-testing`; local review by
   the review agents named there, LOCAL ONLY. Delivered code is reviewed
   against the approved SDD under the pinned test configuration.
9. Harness + integration tests: `fprime-system-test-harness` with
   `system-test-build` and `system-test-review` Mode B; dry run per that
   skill on the bench. Triage per the contract: harness defect → fix here;
   UUT defect → step 8; baseline defect → plan-change diff to the approver
   (the only way back to the approver in this lane).
10. Sub-sessions: a development child for step 8, reviewer children for
    step 9, and a **runner child** for every bench dry run — one bench
    actor at a time. Exit: dry run clean per the skill, harness review zero
    must-fix, UUT and harness frozen at recorded SHAs.

## 4. RELEASE lane — ends at the input **Seed**

11. Coverage evidence per `fprime-system-test-harness` §3a and delivery per
    `fprime-run-for-record` §1 (`fth deliver`): one input package whose
    verification refuses anything missing or non-portable (a refusal returns
    to step 9 — nothing is hand-assembled). Record the package identity in
    the ledger.
12. Rehearse the RfR without a seed per `fprime-run-for-record` §3 **before**
    asking for the seed. Then obtain the seed: `human` — request it, it goes
    to the runner as the skill specifies, never into chat, logs or commits;
    `derived` — the human derives it per contract §9.3 in a shell of their
    own and the campaign id is recorded in the ledger.

## 5. RUN-FOR-RECORD lane — ends at gate 2, **Read QL**

13. A **clean runner sub-session** (or the human) with only the input
    package, the seed and `fprime-run-for-record` §§2–7: one command, rows
    in order, stop at the first refusal, no retries, no inputs assembled by
    any LLM. The developer session never runs it. Auditor and unseal are
    rows of the same procedure.
14. **Gate 2 — Read QL (block).** Deliver the tool-built QL package and its
    checksum sidecar, the ledger and the RfR log as produced by the tooling.
    Independently verify the hash and the package's acceptance checklist
    (`fprime-run-for-record` §5a); report `[V]`/`[I]` facts only. Close out
    the bench and say so. Campaign close per `fprime-run-for-record` §7.

## 6. Approver

- `human`: gates 1 and 2 block on the human; the human reads the SDD and
  the QL.
- `project-campaign` (contract §9.2): the project session performs both
  gates against the SSDD — gate 1 passes when the SDD `## Requirements`
  rows trace to every system requirement allocated to this component and
  the review loop closed per `phase-review-loop.md`; gate 2 passes when the
  QL package verifies (§5a) and `unseal.md` shows no row needing human
  triage (contract §6). Each ruling is one ledger line with the evidence
  read. Any row that needs human triage, any plan-change diff and any
  elicitation question still goes to the human. The human may name
  components they will approve themselves; those run as `human`.

## Specifications

- Lanes run in order, in one campaign prompt; the UUT SHA does not change
  after the DEVELOPMENT lane exits.
- Exactly two reviews (SDD, QL) and two inputs (goals, seed). Any other
  return to the approver is a plan-change diff, a tooling refusal, or a
  bench fact the skills cannot know.
- Every review is local; nothing is posted to GitHub by this session or
  any child. UUT and harness PRs follow the repositories' workflows.
- The QL package is tool-built; the ledger is the only LLM-written
  deliverable.
- Tooling only at the version contract §8 pins; a pin change after the
  package is built is a plan change.

## Advice

- When in doubt open the lane skill; hand a child the skill path and the
  contract, never a restatement.
- Child kickoffs name the skills source, the identity and bench rules,
  LOCAL ONLY, the approver, and the one thing the child owns; runner
  children never receive the developer's working tree.
- Record every decision (rulings, waivers, refusals and what they caught)
  in the ledger as it happens.
- Report `[V]` for what was read from an artifact or tool output, `[I]`
  for inference; quote the goals verbatim wherever they appear.

## Forbidden

- Writing code, unit tests or harness before gate 1, or designing tests
  from implementation code.
- A third review; asking for a seed before the seed-free rehearsal passed.
- Hand-assembling, editing or re-ordering any RfR input; running the RfR
  from the developer session; retrying a refused row.
- Weakening a pass rule, threshold, coverage target or test to make a run
  pass; lowering a tooling gate.
- Posting any review, finding, plan or run output to GitHub; exposing a
  seed, password or token in chat, logs, command history or commits.
- Touching the bench outside the project's bench procedure.
- Restating or overriding a lane skill in this skill or a child prompt.
