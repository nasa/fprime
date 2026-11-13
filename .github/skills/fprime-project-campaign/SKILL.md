---
name: fprime-project-campaign
description: >-
  Orchestrate a whole F Prime project or deployment from ask to a release
  description document (RDD): system requirements, decomposition and
  allocation into inherited and new components (SSDD), human system-design
  gate, base deployment from the inherited components, one development
  campaign per new component against that deployment, system-level tests,
  a final run-for-record of every component and of the system, and the RDD
  the human reads at the system test close-out gate. Keywords: F Prime,
  project campaign, deployment, SSDD, system requirements, inheritance,
  RDD, release description, system test, run for record, close-out.
---

# Skill: F Prime project campaign

The development campaign, one level up. Six lanes, two human reviews
(**Approve SSDD**, **Read RDD**) and two human inputs (**Ask + goals**,
**Master seed**). Thin wrapper: the lane skills are the how; the contract
(`.github/agents/_shared/test-process-contract.md`, §9 for this level) is
the source of truth. Read both first, every time.

```
SYSTEM DESIGN → Approve SSDD → BASE DEPLOYMENT → COMPONENT CAMPAIGNS
→ SYSTEM TESTS → Master seed → FINAL RUN-FOR-RECORD → RDD → Read RDD
```

## 0. Inputs

| Input | Required | Content |
|---|---|---|
| Ask + goals | yes | What the deployment must do, quoted verbatim into the SSDD Introduction and every child campaign. Default goal: *a functional deployment with respect to the ask, every system requirement proven in system tests on the complete testbed*. |
| Project repository | yes | Where the deployment, SSDD, system tests and ledger live; its branch workflow. |
| Catalogs | yes | The repositories the inheritor may draw from, each at a ref: `nasa/fprime`, the `fprime-community` repositories allowed, project libraries. |
| Bench facts | yes | As `fprime-development-campaign` §0, for the whole testbed. |
| Component review | no | Names of new components the human will gate themselves (default: none — the project session is the approver, contract §9.2). |
| Review bound | no | `review rounds: N` for every design loop. |
| Skills source | no | Repository and ref the contract, agents and skills are read from. Default `nasa/fprime` `devel`; a campaign trialling unmerged skills names the branch or PR, and passes it to every child. |
| Master seed | later | Requested only when the FINAL RUN-FOR-RECORD lane is about to start (contract §9.3). |

## 1. Setup (anchor session)

1. Read fresh from the skills source: the contract, `phase-review-loop.md`,
   `fprime-development.agent.md`, and every skill named below. Clone the
   project repository and the catalogs at their refs.
2. Open the **project ledger**; every lane and every child campaign's
   ledger identity is appended to it. It is the only LLM-written deliverable
   besides the SSDD.
3. Confirm the bench facts read-only; send the human the needs checklist.

## 2. SYSTEM DESIGN lane — ends at gate 1, **Approve SSDD**

4. **System requirements** — `fprime-component-requirements` Step 2 form at
   system level (`REQ-<System>-NNN`), reviewed by `requirements-review`
   under `phase-review-loop.md`. Fault scenarios (RS-2) include loss of each
   link and device and of the ground system.
5. **Decomposition** — `fprime-system-decomposition`, reviewed by
   `system-decomposition-review`: SSDD `## Architecture` + `## Allocation`.
6. **Allocation** — `fprime-system-allocation`, reviewed by
   `system-allocation-review`: SSDD `## Inventory` + `### Red flags`.
7. **System test plan** — `fprime-system-test-design` with the SSDD as the
   UUT document (its `## Requirements` and `## Tests` tables are what `fth`
   reads, contract §3): every `subtopology`, `set` and `system` allocation
   row has at least one test; `component` rows are covered by their child
   campaigns and listed as such in the not-covered list's `inherited`
   subsection. Reviewed by `system-test-review` Mode A. Deployment = the
   base deployment to be built in lane 3; L0 is `desk-checked`.
8. Sub-sessions: one reviewer child per reviewer agent, reused across
   rounds, LOCAL ONLY. Elicitation findings batched once per round.
9. **Gate 1 — Approve SSDD (block).** The human reads one document,
   `docs/ssdd.md`; the gate message is ≤ 10 lines: `### Red flags` first
   (each `INHERITED-MODIFIED` row approved by name or struck), routed
   adjudication items, residual counts, bound. Plan, fault menu, review
   records, catalog notes are named attachments. Rulings go in the ledger.
   A struck red flag returns to step 6.

## 3. BASE DEPLOYMENT lane — no gate

10. `fprime-topology-development` builds the deployment from the
    `INHERITED` rows at their pinned refs: instances, base ids, rate groups,
    connections, phased init; `fprime-cmake-build-system` for the build.
    Include the coverage-dump component the tooling requires for
    `fth dev-campaign` (`fprime-system-test-harness` §3a). New components
    are not wired yet; where the topology needs a placeholder, use the
    tester component of `fprime-system-test-harness`.
11. `INHERITED-MODIFIED` changes (approved at gate 1 only): `fprime-maintenance`,
    upstream issue filed, standalone stage-2 plan (contract §1 row 2) run
    before the component is treated as inherited.
12. Verify on the bench: image builds for the target, boots, the L0 marker
    round-trips (`fprime-system-test-design` §1); the system plan's first line
    moves to `L0: bench-confirmed`. Record `repo@sha` of the base deployment
    in the ledger — the **test configuration** every child receives.

## 4. COMPONENT CAMPAIGNS lane — no human gate by default

13. One `fprime-development-campaign` child per `NEW` inventory row, in the
    order the allocation's dependencies dictate (a component another one's
    tests need goes first). Kickoff: the ask (verbatim), the component's
    allocated REQ ids and the SSDD, the test configuration (step 12),
    `approver: project-campaign` unless the human named the component,
    `campaign-id: <inventory id>`, the skills source, bench and identity
    rules, LOCAL ONLY.
14. Each child's two gates are performed by this session per
    `fprime-development-campaign` §6; every ruling is a ledger line that
    names the artifact and the check. Children run sequentially on one bench;
    a child's harness lands in the project repository on a per-component
    branch merged into the base deployment branch when the child exits.
15. Exit: every child's QL package verified and its UUT SHA, package identity
    and ledger recorded in the project ledger; all new components wired into
    the deployment (`fprime-topology-development` §6).

## 5. SYSTEM TESTS lane — no gate

16. `fprime-system-test-harness` on the system plan: UUT = the project
    deployment repository at its frozen SHA, harness = the same repository's
    system `test/int` (contract §3 manifest `uut`/`harness`), reviewed by
    `system-test-review` Mode B; vectors and rehearsal per
    `fprime-system-test-design` §6. Triage per the contract: harness →
    here; a component defect → that component's campaign reopens at its
    step 8 (and its final RfR below is what re-qualifies it); baseline →
    plan-change diff to the human.
17. `fth deliver` for the system package; seed-free rehearsal
    (`fprime-run-for-record` §3). Freeze the deployment SHA.

## 6. FINAL RUN-FOR-RECORD lane — human input **Master seed**, then **Read RDD**

18. Request the master seed (contract §9.3) only now. For every `NEW`
    component, re-run its RELEASE lane against the frozen deployment (new
    manifest: `fth deliver`, rehearsal) and its RfR with the derived seed
    `campaign-id = <id>:final`; then the system RfR with `campaign-id =
    system`. Each RfR in a clean runner child, one bench actor at a time,
    `fprime-run-for-record` §§2–7, no retries.
19. **Staleness** (contract §9.4): a component QL whose manifest `repos[]`
    SHAs differ from the system manifest's for any shared repository, or
    that was not re-run in step 18, is `STALE` in the RDD. The RDD is not
    handed over with a `STALE` row unless the human waived it by name in
    the ledger.
20. **RDD assembly** by the RDD tool (independent of `fth`; contract §9.5
    names its inputs and refusals): the SSDD, the inventory, every
    component package (`ql.zip` + sha256, `docs/checklist.md` after
    `fprime-checklist-closeout`), the system `ql.zip`, the ledgers. The tool verifies every hash and refuses a missing input;
    nothing is hand-assembled. Until the tool exists, the campaign stops
    here and reports the input set — it does not write the RDD by hand.
21. **Gate 2 — Read RDD (block).** Deliver the tool-built RDD and its
    checksum sidecar, the project ledger and the RfR logs as produced.
    Independently verify the hashes and each QL package's acceptance
    checklist (`fprime-run-for-record` §5a); report `[V]`/`[I]` facts only.
    Close out the bench and say so. Campaign close per
    `fprime-run-for-record` §7 for every run.

## Specifications

- Lanes run in order; the deployment SHA does not change after lane 5
  exits; no two RfRs run concurrently on the bench.
- Exactly two human reviews (SSDD, RDD) and two inputs (ask, master
  seed) unless the human named components to gate themselves (then two
  more per named component). Any other return to the human is a red flag,
  a plan-change diff, a tooling refusal, an unseal row needing triage, or a
  bench fact.
- Everything is local; nothing is posted to GitHub by this session or any
  child. Repository PRs follow the repositories' workflows.
- The RDD and every QL are tool-built; the SSDD and the ledgers are the
  LLM-written deliverables.

## Forbidden

- Modifying an inherited component without a gate-1-approved red flag.
- Starting a component campaign before gate 1, or without the test
  configuration SHA.
- Approving a child gate without the ledger line naming the evidence;
  gating a component the human asked to gate themselves.
- Deriving, holding or forwarding any seed in this session or a child
  other than the runner; asking for the master seed before every package
  has passed its seed-free rehearsal.
- Handing over an RDD with an unwaived `STALE` row, or one not built by the
  RDD tool.
- Everything `fprime-development-campaign` and contract §7 forbid.
