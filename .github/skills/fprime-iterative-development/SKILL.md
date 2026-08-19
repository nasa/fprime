---
name: fprime-iterative-development
description: Use when an F Prime development task should be driven to convergence through repeated local test-and-review iterations — running unit, integration, and manual GDS tests plus the full multi-agent review locally, and fixing findings until tests pass and findings trend to zero. Requires explicit user approval before use. Keywords: F Prime, iterative development, iteration loop, local review, convergence.
---

# Skill: F Prime iterative development loop

Develop a feature or component through the standard phase-gated
process, then converge it in a local test-and-review loop: run unit
tests, integration tests, and manual GDS tests, run the full
multi-agent review **locally only**, fix what they surface, and repeat
until tests pass and findings trend to zero.

---

## 0. Approval gate — required before use

This skill runs a long, resource-intensive loop and a full multi-agent
review. **Never begin it on your own initiative.** Before executing:

1. Present the user a short plan: the development scope, the test
   commands to be run, the manual-test approach, the review setup,
   and the iteration exit criteria (§3).
2. Obtain the user's **explicit approval to run this skill**. A
   general request to "develop X" is not approval to iterate; ask.
3. Do not start Phase 1 (§1) until that approval is given.

## 1. Phase 1 — development (ask-heavy)

Follow `.github/agents/fprime-development.agent.md`, which classifies
the task and sequences the development skills with their phase gates
(requirements and FPP-design approval before implementation). This is
the phase to ask questions — front-load every question you can
anticipate so the iteration loop does not need to block on the user.

Implement the feature, its unit tests (`test/ut/`), and its
integration tests (`test/int/`) per those skills. Before entering the
loop, send the user a brief recap of the loop plan and any final
questions; proceed unless a gating question is unanswered.

## 2. Phase 2 — iteration loop (autonomous)

Prefer autonomy, but ask the user whenever forward progress cannot be
made without input (ambiguous or contradictory requirements surfaced
by a finding, a fix that would change the approved design or
interface, a test failure whose expected behavior is unclear, missing
environment/hardware access). Do not spin in place or guess.

1. **Run unit tests**:
   ```bash
   fprime-util generate --ut && fprime-util build --ut -j"$(nproc)"
   (cd build-fprime-automatic-native-ut && ctest -j"$(nproc)")
   ```
2. **Run integration tests**: build the relevant deployment, launch
   it with the GDS (`fprime-gds`), and run the `test/int/` pytest
   suite against it.
3. **Manually test the feature**: exercise it end to end via the GDS
   (send commands, verify events / telemetry / data products) as an
   operator would. Record observations.
4. **Run the multi-agent review LOCALLY ONLY** — the same pipeline as
   `.github/agents/review-orchestrator.agent.md`, with one critical
   change: **no reviewer, aggregator, or subagent may post anything
   to GitHub** — no PR reviews, inline comments, issue comments,
   statuses, or labels. All findings go to local files only (e.g.
   `review-iter-N/<agent>.md`):
   - Read the orchestrator, `_shared/agent-registry.yml`,
     `_shared/review-contract.md`, and each reviewer agent file fresh
     from `nasa/fprime` `devel`.
   - Run every `role: reviewer` agent from the registry against the
     local diff (`git diff <base>...HEAD`). Every reviewer kickoff
     prompt MUST state in capital letters: LOCAL REVIEW ONLY — DO NOT
     POST TO GITHUB; write findings to the local output file.
   - Aggregate the findings locally per `review-summary.agent.md`
     into `review-iter-N/summary.md`.
5. **Triage and fix**: classify each finding (fix / false positive /
   acknowledged nit), fix test failures and valid findings, and
   record the finding count for the iteration.
6. **Loop**: return to step 1.

## 3. Exit criteria

Stop iterating when:

- all tests pass AND findings trend to zero (no new high/medium
  findings; remaining findings are acknowledged nits or false
  positives) — success; or
- finding counts plateau across two consecutive iterations —
  summarize the residual findings for the user; or
- 5 iterations complete — report status and ask how to proceed.

Deliverables: the implementation and tests (delivered per the user's
PR workflow; the PR description may summarize the final local review
outcome, but never post the raw review as PR comments/reviews), plus
the final iteration's `review-iter-N/` artifacts (per-agent findings
and summary) shared with the user.

## Forbidden actions

- Never run this skill without the explicit approval required by §0.
- **Never post review results to GitHub** — no PR reviews, inline
  comments, issue comments, commit statuses, or labels from the local
  review pipeline. Validation: zero comments/reviews/statuses created
  by this run on any PR or commit.
- Never skip the requirements/design approval gates of
  `fprime-development.agent.md`.
- Never modify or weaken tests to make them pass.
- Do not give reviewer subagents a token with write scope; grant read
  access only when needed for context.
