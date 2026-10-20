# Phase review loop (requirements and design)

The requirements and FPP-design phases of component development each
end in an adversarial review loop before the human gate. This file is
the only place that names the reviewers, the bound and the round
rules; `fprime-development.agent.md` §2 runs it (and
`fprime-component-development` cardinal rule 2 points here). The
writing skills `fprime-component-requirements` and
`fprime-component-design-fpp` do not iterate; the orchestrating agent
loops them.

## Reviewers per phase

| Phase | Artifact under review | Reviewer(s) | Intent input |
|---|---|---|---|
| Requirements | Requirements table (`fprime-component-requirements` Step 2 form) | `requirements-review.agent.md` | The request (issue / human goals, quoted) |
| Design | The `.fpp` model, the `docs/sdd.md` draft (`fprime-component-design-fpp` Step 5), any new port definitions — as a design-only diff | `design-review.agent.md` (categories 1, 3, 4, 6, 7, 9; categories 2, 5, 8 need an implementation and are n/a); `architecture-review.agent.md` (whole model); `design-principles-review.agent.md` | The approved requirements table and the request |

Reviewer agent files are read fresh from `nasa/fprime` `devel`, not
from the branch being worked on; a reviewer not yet on `devel` is read
from the commit that introduces it, named in the record. Reviewers read the
artifact and the
intent input only — never the drafting session's reasoning.

**Preamble for PR-written reviewers** (design, architecture), given
verbatim at kickoff: *There is no PR. The diff is the attached design
artifacts; every line is "introduced" (contract §3). The PR
description and linked issue are the attached requirements table and
request; the SDD draft's rationale stands in for the PR body where a
category reads it. Apply review-contract §0, §1/§1a and §8 only; do
not apply §2, §4 pings, §6a, §7, §9–§11. Write
`review-design-<reviewer>-round-N.md` in the shape of
`fprime-requirements-review` §4 (Findings table `Key | Tag | Class |
Site | Finding | Rewrite`, totals, disagreement log); Key =
`<finding-class>:<FPP element or SDD section>`, stable across rounds.
Low-confidence findings go in the Findings table tagged `low-confidence`, no
ping; the author treats them as could-fix unless it agrees. Do not edit the
artifact.* For the architecture reviewer add: *the baseline is the new model
as a whole (classify it; Step 2 erosion is n/a for a new component); heuristic
5 (`.cpp`/`.hpp`) is n/a; category 4 (topology wiring) is deferred to the
topology phase.*

`design-needs-human-adjudication` (raised by design-review for every
new component) is routed, not closed: the reviewer records it without
maintainer lookup or ping, it is carried to the human gate as the
first item of the record, and it does not count toward the stop
rule's must-fix total. Every other must-fix counts.

## Bound

Default **3 reviews** per phase. Any unambiguous round count in the
invoking prompt overrides it (canonical form `review rounds: N`,
N ≥ 1); it applies to both phases unless given per phase; two
conflicting counts → ask (cardinal rule 1). A prompt asking for no
review gets 1 and a note that 1 is the minimum. The bound in use and
its source (`default` | `prompt`) go in the message to the user when
the loop starts and in line 1 of every round record, and are passed
to each reviewer as `review rounds: N` — this supersedes any round
cap in the reviewer's own file. With N = 1 the artifact reaches the
gate "reviewed once, closes unverified" (see Rounds, step 5).

Why 3: of the two loops on record for texts like these, one
converged inside 3 (12→2→0); the other (11→5→3→0) would have reached
the gate with 3 residual must-fixes, which is the intended behaviour
— the residual goes to the human. 3 is the default until more loops
are on record. Raise it in the prompt for a large artifact or a
request in new territory.

## Rounds

One round = one review by every reviewer in the phase row not skipped
per step 2, then one close by the author. Review k is the k-th such review.

1. **Draft** the artifact per the phase skill.
2. **Review** — start one separate session per reviewer (child
   session or subagent); the kickoff is the reviewer agent file, the
   artifact, the intent input, the preamble above where it applies,
   `review rounds: N — this bound supersedes any round cap in your
   agent file`, and "LOCAL REVIEW ONLY — DO NOT POST TO GITHUB; write
   findings to a file". Reuse each session across rounds so it
   carries its open-finding list. A reviewer whose latest record is
   all-pass is re-run only if its scope was touched by a close.
   **Fallback:** only after an attempt to start a separate session
   fails — the round record names the mechanism tried (tool or API)
   and the error. Then run the reviewer agent inline in a fresh pass
   that quotes the intent input and lists its elements *before*
   re-reading the draft, and marks the record `inline`. An inline
   review counts toward the bound and is reported as inline at the
   human gate.
3. **Close** — classify each must-fix as *editorial* (wording,
   verification method, rationale, FPP form: fix it in the artifact)
   or *elicitation* (new behaviour, fault response, command /
   telemetry content, an interface the user has not stated: ask the
   user, batched once per round, cardinal rule 1). The ask is
   blocking: the next review starts after the user answers; if the
   user defers, log it and the item stays open. An elicitation finding
   is never closed by inventing the answer. For every could-fix record fixed /
   declined-with-reason. Deleting a row or removing a design element
   never closes a must-fix by itself: record the deletion, and the
   reviewer re-runs its set-level checks (RS-1..3; for design, the
   whole-model classification) and decides whether the finding is
   closed, reopened as a gap, or logged as a disagreement. Where the
   author disagrees, record both positions in the disagreement log;
   agreement reached by softening is a disagreement, not a close.
   For design, re-run `fprime-component-design-fpp` Step 4 (build)
   after every close.
4. **Re-review** — changed items plus any set-level check the
   reviewer defines (e.g. RS-1..3 on the whole set). A finding is
   closed only when a reviewer verifies the change.
5. **Stop** when the latest review reports zero must-fix (routed items
   excluded), when the must-fix key set is unchanged between two
   consecutive reviews *and* every remaining must-fix is either in
   the disagreement log with both positions or an elicitation item
   awaiting the user (plateau: log the residual — a plateau with a
   must-fix neither addressed nor logged is not a stop), or when N
   reviews have been run. Do not start a review beyond the bound.
   Author-side closes after the last review are *unverified* and are
   listed as open in the record. Final counts are those of the last
   review run plus the unverified list.

A design finding that invalidates a requirement reopens the
requirements phase: run the requirements loop on the changed rows
(a fresh bound of N, set-level checks on the whole set), present the delta to
the user and obtain approval again, then resume design. The
requirement is never edited silently.

## Record and gate

Keep one `review-<phase>-<reviewer>-round-N.md` per reviewer per
review (or the name the reviewer's own agent file fixes), outside the
component tree or attached, with a running disagreement log. At the
human gate present: the artifact, the bound and its source, reviews
run (and which were inline), the final counts per step 5, the routed
adjudication items first, the residual list if the bound or plateau
was hit, and the disagreement log. Human approval is the gate; a
review-passing artifact is not approved until the human says so.

## Forbidden

- Posting any review content to GitHub (PR reviews, comments, issues,
  statuses, labels) from this loop.
- Giving a reviewer session a token with write scope.
- Skipping the loop because the artifact is small; a short artifact
  takes one short review.
- Declaring a phase review-passing with an open, unrouted must-fix.
- Closing an elicitation finding without the user's answer.
