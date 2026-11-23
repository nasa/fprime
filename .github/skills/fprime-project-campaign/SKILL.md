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

The development campaign, one level up. Five lanes, two human reviews
(**Approve SSDD**, **Read RDD**) and two human inputs (**Ask + goals**,
**Master seed**). Thin wrapper: the lane skills are the how; the contract
(`.github/agents/_shared/test-process-contract.md`, §9 for this level) is
the source of truth. Read both first, every time.

```
SYSTEM DESIGN (incl. component SDDs) → BASE DEPLOYMENT (prototype on the
bench) → Approve SSDD → COMPONENT CAMPAIGNS → SYSTEM TESTS → Master seed →
RUN-FOR-RECORD → RDD → Read RDD
```

**Who decides what.** The human concurs on the system design only, at
Approve SSDD. The agents of this process are responsible for every
component, one by one — its requirements, SDD, code, tests and package —
and the project session, not the human, passes each component's gates.
The human's input on a component enters at Approve SSDD, where the
component SDDs are linked: an SDD the human rejects there, or comments
on, returns to that component's campaign (step 12). After that gate the
human is asked only for the master seed, for what the contract always
routes to a human (an unseal row needing triage, a plan-change diff, a
red flag), and to read the RDD.

The human approves a design that has already run: every new component has
its SDD and the inherited components have booted as a prototype before
gate 1. The SSDD then carries the design and the test plan unchanged to
the RDD. The master seed is requested once, directly before the
run-for-record lane; every run needs only its `campaign-id` (contract
§9.3).

## 0. Inputs

| Input | Required | Content |
|---|---|---|
| Ask + goals | yes | What the deployment must do, quoted verbatim to every child campaign and reviewer. The SSDD does not quote it: its Introduction states what the deployment does. Default goal: *a functional deployment with respect to the ask, every system requirement proven in system tests on the complete testbed*. |
| Project repository | yes | Where the deployment, SSDD, system tests and ledger live; its branch workflow. |
| Catalogs | yes | The repositories the inheritor may draw from, each at a ref: `nasa/fprime`, the `fprime-community` repositories allowed, project libraries. |
| Bench facts | yes | As `fprime-development-campaign` §0, for the whole testbed. |
| Review bound | no | `review rounds: N` for every design loop. |
| Skills source | no | Repository and ref the contract, agents and skills are read from. Default `nasa/fprime` `devel`; a campaign trialling unmerged skills names the branch or PR, and passes it to every child. |
| Master seed | later | Requested once, directly before the run-for-record lane — never with the ask, and never held by this session (contract §9.3). The human keeps it and derives every run's seed from it. |

## 1. Setup (anchor session)

1. Read fresh from the skills source: the contract, `phase-review-loop.md`,
   `fprime-development.agent.md`, and every skill named below. Clone the
   project repository and the catalogs at their refs.
2. Open the **project ledger**; every lane and every child campaign's
   ledger identity is appended to it. It is the only LLM-written deliverable
   besides the SSDD.
3. Confirm the bench facts read-only; send the human the needs checklist.

## 2. SYSTEM DESIGN lane — no gate; gate 1 follows the prototype

4. **System requirements** — `fprime-component-requirements` Step 2 form at
   system level (`REQ-<System>-NNN`), reviewed by `requirements-review`
   under `phase-review-loop.md`. Fault scenarios (RS-2) include loss of each
   link and device and of the ground system.
5. **Decomposition** — `fprime-system-decomposition`, reviewed by
   `system-decomposition-review`: SSDD `## Architecture` + `## Requirement to Component Map`.
6. **Allocation** — `fprime-system-allocation`, reviewed by
   `system-allocation-review`: SSDD `## Inventory` + `### Red flags`.
7. **System test plan** — `fprime-system-test-design` with the SSDD as the
   UUT document (its `## Requirements` and `## Tests` tables are what `fth`
   reads, contract §3). Coverage follows the allocation's *final*
   verification level after step 6: every row whose verification is a
   system test has at least one test — all `subtopology`, `set` and
   `system` rows, every `component` row allocated to an `INHERITED`,
   `INHERITED-MODIFIED` or `NEW-SUBTOPOLOGY` element, and any row AL-5
   moved. The only rows without a system test are `component` rows owned
   by a `NEW` element's child campaign; they are listed in the plan's
   not-covered list under `delegated: <campaign-id>`. Reviewed by `system-test-review` Mode A. Deployment = the
   base deployment built in lane 3. The L0 line states the check (image
   boots, marker round-trips) so that its bench result in step 10 is a
   ledger line, not a plan change.
8. **Component SDDs** — one `fprime-development-campaign` child per `NEW`
   (component) inventory row — never for a `NEW-SUBTOPOLOGY` — run through
   its DESIGN lane only. Kickoff: the ask (verbatim), the component's
   allocated REQ ids and the SSDD, `approver: project-campaign`,
   `campaign-id: <inventory id>`, the
   skills source, identity rules, LOCAL ONLY, and *stop after your Approve
   SDD gate*. This session performs that gate per
   `fprime-development-campaign` §6 (a ledger line naming the artifact and
   the check). Each SDD lands at the path the inventory links. **Exit:**
   the lane does not proceed to step 9 until every `NEW` component's SDD
   has been received and its gate passed, each with its ledger line; a
   gate that does not pass sends that child back into its DESIGN lane.
   Sub-sessions
   of steps 4–7: one reviewer child per reviewer agent, reused across
   rounds, LOCAL ONLY. Elicitation findings batched once per round.

## 3. BASE DEPLOYMENT lane — ends at gate 1, **Approve SSDD**

9. `fprime-topology-development` builds the prototype deployment from the
    `INHERITED` rows at their pinned refs, unmodified (an
    `INHERITED-MODIFIED` row is wired as published, or as a placeholder
    where it cannot work unmodified): instances, base ids, rate groups,
    connections, phased init; `fprime-cmake-build-system` for the build.
    Include the coverage-dump component the tooling requires for
    `fth dev-campaign` (`fprime-system-test-harness` §3a). New components
    are not wired yet; where the topology needs a placeholder, use the
    tester component of `fprime-system-test-harness`. A `NEW-SUBTOPOLOGY`
    whose members are all inherited is built here
    (`fprime-topology-development` §5); one with a `NEW` member is built in
    lane 4 step 15 once that member's campaign has exited.
10. **Prototype on the bench**: the image builds for the target, boots,
    the L0 marker round-trips (`fprime-system-test-design` §1), and the
    built topology matches the SSDD — every `INHERITED` inventory row is
    instantiated, every instance is in the inventory, and the regenerated
    topology diagram equals the SSDD's. The result is a ledger line with
    `repo@sha` of the base deployment — the **test configuration** every
    child receives. It is not a plan change.
11. **Automatic redesign.** When step 10 fails, or the prototype differs
    from the SSDD, return to step 5 without asking: correct the
    decomposition, allocation, plan and any affected component SDD (their
    reviews run again), rebuild, re-run. At most three passes; then, or
    when the failure shows a system requirement is wrong, return to the
    human with what failed. Each pass is a ledger line.
12. **Gate 1 — Approve SSDD (block).** The human reads the **SSDD
   package**, with a prototype that ran. Build the package with
   `ssdd-package <project>` (https://github.com/LeStarch/rdd-creator): it
   writes `docs/ssdd-package/` holding the SSDD and the SDD of every `NEW`
   element of the inventory, each as Markdown and as one self-contained
   HTML page, plus an index, and refuses when a `NEW` component has no
   SDD or a diagram is missing. Attach the package to the gate message as
   one archive and name the commit it was built from; rebuild and
   re-attach it whenever the SSDD or a new component's SDD changes before
   the ruling. The gate is not asked without it. The gate
   message is ≤ 10 lines: `### Red flags` first (each `INHERITED-MODIFIED`
   row approved by name or struck), the prototype's ledger line, routed
   adjudication items, residual counts, bound. Plan, fault menu, review
   records, catalog notes are named attachments. Rulings go in the ledger.
   A struck red flag returns to step 6; a rejected component SDD returns
   to step 8; either re-runs steps 9–11.

## 4. COMPONENT CAMPAIGNS lane — no human gate by default

13. `INHERITED-MODIFIED` changes (approved at gate 1 only): `fprime-maintenance`,
    upstream issue filed, standalone stage-2 plan (contract §1 row 2) run
    before the component is treated as inherited; the base deployment is
    rebuilt and step 10 repeated, and the new `repo@sha` is the test
    configuration.
14. Each child of step 8 resumes at its DEVELOPMENT lane and runs through
    RELEASE, in the order the allocation's dependencies dictate (a
    component another one's tests need goes first), with the test
    configuration and the bench rules, and stops at a delivered package
    that has passed its seed-free rehearsal — it does not request a seed
    and does not run for record. This session checks each package per
    `fprime-development-campaign` §6; every ruling is a ledger line that
    names the artifact and the check. Children run sequentially on one
    bench; a child's harness lands in the project repository on a
    per-component branch merged into the base deployment branch when the
    child exits.
15. Exit: every child's package verified and its UUT SHA, package identity
    and ledger recorded in the project ledger; all new components wired into
    the deployment and the remaining `NEW-SUBTOPOLOGY` rows built from them
    (`fprime-topology-development` §5–§6).

## 5. SYSTEM TESTS lane — no gate

16. `fprime-system-test-harness` on the system plan: UUT = the project
    deployment repository at its frozen SHA, harness = the same repository's
    system `test/int` (contract §3 manifest `uut`/`harness`), reviewed by
    `system-test-review` Mode B; vectors and rehearsal per
    `fprime-system-test-design` §6. Triage per the contract: harness →
    here; a component defect → that component's campaign reopens at its
    step 8 (and its RfR below is what qualifies it); baseline →
    plan-change diff to the human.
17. `fth deliver` for the system package; seed-free rehearsal
    (`fprime-run-for-record` §3). Freeze the deployment SHA.

## 6. RUN-FOR-RECORD lane — ends at gate 2, **Read RDD**

18. **Master seed (block).** Request it once, here (contract §9.3); the
    human derives each run's seed. Then, for every `NEW` component, rebuild
    its package against the frozen deployment if it was delivered earlier
    (`fth deliver`, rehearsal) and run its one RfR with the derived seed
    `campaign-id = <id>`; then the system RfR with `campaign-id =
    system`. Each RfR in a clean runner child, one bench actor at a time,
    `fprime-run-for-record` §§2–7, no retries.
19. **Staleness** (contract §9.4): a component QL whose manifest `repos[]`
    SHAs differ from the system manifest's for any shared repository, or
    that was not re-run in step 18, is `STALE` in the RDD. The RDD is not
    handed over with a `STALE` row unless the human waived it by name in
    the ledger.
20. **RDD assembly** by the RDD tool (independent of `fth`; contract §9.5
    names its inputs and refusals): `rdd <project>` reads the SSDD, the
    top-level topology FPP (every component and subtopology of the
    release), and per element its SDD, checklist, coverage and grade —
    from the published F Prime quality index, else beside the SDD, else
    `unavailable`. Before running it, see that each `NEW` component has
    `docs/sdd.md`, `docs/checklist.md` (after `fprime-checklist-closeout`)
    and `docs/coverage/index.html`, and that `docs/rdd.json` names the
    system coverage report, the system test reports and every `ql.zip`
    with its sha256. The tool verifies every hash and refuses a `NEW`
    component that is absent or lacks one of those; nothing is
    hand-assembled. `--allow-incomplete` builds a draft marked incomplete
    and is never used for the RDD of gate 2. If the tool is not
    installed, the campaign stops here and reports the input set — it
    does not write the RDD by hand.
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
  seed — requested once, directly before the run-for-record lane). Any other return to the human is a red flag,
  a plan-change diff, a tooling refusal, an unseal row needing triage, or a
  bench fact.
- Everything is local; nothing is posted to GitHub by this session or any
  child. Repository PRs follow the repositories' workflows.
- The RDD and every QL are tool-built; the SSDD and the ledgers are the
  LLM-written deliverables.

## Forbidden

- Modifying an inherited component without a gate-1-approved red flag.
- Taking a component campaign past its DESIGN lane before gate 1, or
  into DEVELOPMENT without the test configuration SHA.
- Modifying an inherited component to make the prototype run.
- Approving a child gate without the ledger line naming the evidence;
  asking the human to gate a component outside Approve SSDD.
- Deriving, holding or forwarding any seed in this session or a child
  other than the runner; asking for the master seed before the first
  package has passed its seed-free rehearsal.
- Handing over an RDD with an unwaived `STALE` row, or one not built by the
  RDD tool.
- Everything `fprime-development-campaign` and contract §7 forbid.
