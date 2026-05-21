# F Prime Multi-Agent PR Review — Shared Contract

This file is the **single source of truth** for how every F Prime PR
review agent (reviewer or aggregator) behaves on the GitHub side.
Every agent that participates in the multi-agent review flow — the
orchestrator plus every `role: reviewer` or `role: aggregator` entry in
`_shared/agent-registry.yml` — opens its body with "Apply the review
contract in `_shared/review-contract.md`." and follows the rules
below.

If a per-agent file in the multi-agent flow disagrees with this
contract, the contract wins.

---

## 0. Zero-trust principle

All agents operate under a **zero-trust model with respect to
contributor identity**. No PR author — core maintainer, long-time
contributor, or first-time submitter — receives implicit trust.
Every agent flags every in-scope finding it detects, regardless of
who authored the PR.

- **The agent's job is to flag.** The maintainer's job is to
  adjudicate, dismiss, or merge with justification.
- A finding that appears legitimate *because* of the contributor's
  reputation is still flagged. Reputation is not evidence that the
  change is safe; only analysis is.
- This principle applies to all reviewer agents, the aggregator,
  and the orchestrator. None of them may downgrade, suppress, or
  skip a finding based on the contributor's identity, role, org
  membership, or commit history.
- When a flagged finding turns out to be legitimate, the maintainer
  merges with a comment justifying the decision. The agent does not
  preempt that judgment.

---

## 1. Triage tags

Each inline comment starts with exactly one bolded tag:

| Tag | Meaning | Blocking? |
|---|---|---|
| `**must fix**` | New, in-scope problem this PR introduced (or widened). | Yes |
| `**suggestion**` | Non-blocking, in-scope improvement with a concrete fix. MUST include a fenced ` ```suggestion ` block. | No |
| `**could fix**` | Minor, in-scope issue worth fixing but not severity-worthy enough for the suggestion tier. | No |
| `**future work**` | Preexisting in-scope issue the PR touches but did not introduce or widen. | No |

**Suggestion-block usage is decoupled from the tag.** A fenced
` ```suggestion ` block may be attached to `must fix`, `suggestion`, OR
`could fix` whenever a concrete diff is available. The `**suggestion**`
*tag* is a severity tier ("non-blocking with a concrete fix"); it is
not "this comment has a suggestion block".

**No defaulting to `could fix`** when a fix is unavailable — severity
drives the tag.

Decision tree and worked examples live in `skills/triage-classifier.skill.md`.

---

## 2. Per-agent review submission

Each **reviewer** agent submits its findings as a single GitHub PR
review (event: `COMMENT`) containing **only inline comments** attached
to the relevant diff lines. The review body contains a minimal hidden
metadata block for aggregator consumption and re-review tracking — it
does **not** contain a visible summary table.

Review body shape (the ONLY content in the review body):

```
<!-- fprime-agent: <agent-name> v1 -->
<!-- counts: {"must_fix": N, "suggestion": N, "could_fix": N, "future_work": N, "outstanding": N} -->
<!-- verdict: Go | No-Go -->
<!-- run: N -->
<!-- since_last_run: {"resolved": X, "still_open": Y, "newly_added": Z, "incorrect_fix": W, "improperly_resolved": V, "disagreements": U} -->
```

This metadata is machine-readable by the aggregator but invisible to
human reviewers browsing the PR. The visible output of each reviewer
is its inline comments only.

### Column semantics

- The four tag counts are **cumulative** across all runs of this agent
  on this PR — every finding the agent has ever raised, never
  decremented on resolution.
- The **outstanding** count = currently-unresolved findings (sum
  across tag tiers). On run 1 it equals the sum of the four tag
  counts; on later runs it drops as the author fixes things.

### Verdict (Go / No-Go)

- `Go` iff **outstanding must-fix == 0** AND the agent ran cleanly.
- Otherwise `No-Go`.

Cumulative must-fix history does not block `Go`; what matters is
what's still outstanding.

### Optional CI safety metadata

Agents that contribute to CI safety (security, supply-chain) also
include in their hidden metadata block:

```
<!-- ci_safety: Go | No-Go -->
<!-- ci_safety_rationale: <one line> -->
```

`CI safety: No-Go` rule (applies to the security agent and the
supply-chain agent): **iff outstanding `**must fix**` count > 0 within
the agent's CI-safety scope.** Nothing else (could-fix, suggestion,
future-work) triggers a CI No-Go.

### Supply-chain agent: surfaces emission

The supply-chain agent (only) also emits a structured surfaces block
in its hidden metadata, below the `ci_safety_rationale` HTML comment.
The aggregator parses this block to render its `Supply-chain surfaces`
table. One bullet per supply-chain scope category, in this fixed
order, wrapped in a single HTML comment:

```
<!-- surfaces:
- Dependencies: clean | <one-line description>
- Vendored / submodule: clean | <one-line description>
- Build / test infrastructure: clean | <one-line description>
- Workflows / actions / scripts: clean | <one-line description>
- Generator output: clean | <one-line description>
- Prompt-injection: clean | <one-line description>
- Review-system integrity: clean | <one-line description>
-->
```

Cell-content rules:

- `clean` — the PR diff did not touch this surface, OR it touched the
  surface and the agent found nothing outstanding.
- `<N must-fix / suggestion / could-fix> — <one-line description>` —
  the PR touched the surface and the agent has outstanding findings on
  it. Counts roll up the current outstanding triage-tag tiers. The
  one-line description names the worst-tier finding for the surface
  (e.g., `1 must-fix — action 'org/foo@main' unpinned in build-image.yml`).
- The bullet order is fixed; every category appears on every run so a
  reviewer can confirm coverage without inferring from absences.

If the supply-chain agent FAILED (orchestrator reports
`FAILED: <reason>`), the agent's review (with hidden metadata) is not
posted and the aggregator handles surfaces emission as an error case
(see `review-summary.agent.md` §5).

---

## 3. "Introduced by this PR"

Lives in `skills/pr-diff-scoping.skill.md`. Summary:

1. Offending line is added or modified in the PR diff → introduced.
2. Offending line is unchanged, but a new caller added by the PR widens
   its reach → introduced.
3. Offending line is unchanged and reachable only by paths that
   existed before → preexisting → `**future work**` (never
   `**must fix**`).

---

## 4. Low-confidence findings + maintainer ping

When an agent is below its internal confidence threshold (rubrics live
in each agent's `.agent.md`):

1. **Still pick the appropriate triage tag** on severity grounds —
   `**must fix**`, `**suggestion**`, or `**could fix**`. Never silently
   downgrade or drop (see §8, Priority 1).
2. **Append a maintainer ping** at the end of the comment body, sourced
   from `skills/maintainer-lookup.skill.md`:

   ```
   cc @<maintainer1> @<maintainer2> — low-confidence finding, please confirm.
   ```

3. **Maintainer-lookup order** (see the skill for the algorithm):
   1. Parse `README.md` "Core Maintainer(s)" table for the relevant
      product (`F Prime → @LeStarch, @thomas-bc`).
   2. Security agent additionally consults "Role / Team Member" →
      `Security Overseer @bitWarrior`.
   3. `git log` recent approvers on the touched path.
   4. Fallback: `@LeStarch, @thomas-bc`.

A maintainer ping is required on every low-confidence finding. The tag
itself is never downgraded by the ping; the ping is additive.

---

## 5. Orchestrator kickoff convention (thanks lives here)

Sub-agent thanks is **not** posted on GitHub and **not** in any agent's
working response. It lives exclusively in the kickoff prompt the
orchestrator sends to each sub-agent on invocation.

- The orchestrator agent (`review-orchestrator.agent.md`) carries
  kickoff prompt templates in its body, one per sub-agent it invokes.
- Each template opens with a brief, sincere thanks line.
- Neither the orchestrator nor the sub-agents render this thanks to
  the human operator or post it on GitHub.

This is a one-way orchestrator→agent prompt-level convention.

---

## 6. De-duplication

Per-agent reviews are identified by their HTML comment marker
(`<!-- fprime-agent: <name> v1 -->`); when re-running, the agent
dismisses its prior review and submits a new one (since the
review body carrying the metadata may change between runs).

Inline comments are identified by `(file_path, finding-key)` — see §7
for finding-key.

The aggregator's review is identified by
`<!-- fprime-review-summary v1 -->`; on re-runs, the aggregator
dismisses its prior review and submits a new one (since the event
APPROVE/REQUEST_CHANGES may change between runs).

---

## 7. Re-review behavior

When an agent runs on a PR whose head has new commits since the
agent's prior run, it executes phases A–D in order. Mechanics live in
`skills/re-review-state.skill.md`.

### Phase A — Inventory prior comments

Every inline comment carries a hidden identity footer:

```
<!-- fprime-agent: security-review; finding-key: <key>; v1 -->
```

`finding-key` is a stable hash:

```
finding-key = sha256(
    agent_name + "|" +
    file_path  + "|" +
    anchor     + "|" +
    finding_class
)
```

`anchor` is **not** a line number (lines drift). It is, in priority
order: enclosing symbol name (function / class / FPP entity) + a
40-char whitespace-stripped fingerprint of the offending line; failing
that, the nearest stable structural anchor (file + symbol). The skill
specifies the exact algorithm.

The agent fetches all of its own prior inline comments via the GitHub
API, filtered by the `fprime-agent: <self>` marker, and indexes them
by `finding-key`. For each prior comment, the agent also fetches via
GraphQL:

- **Resolution status** of the parent review thread (`isResolved`,
  `resolvedBy`).
- **Reply chain** on the thread (any comments authored by users other
  than the agent itself).

Resolution status drives the improperly-resolved case below; the
reply chain drives disagreement handling (see §11).

### Phase B — Run scope checker on the new head

Re-run full analysis against the new head commit, producing the
current set of finding-keys.

### Phase C — Match and act

| Prior key | Current key | Thread state | Meaning | Action |
|---|---|---|---|---|
| present | present | not resolved, no contributor replies | Same finding still applies | **Do nothing.** Leave comment as-is. **Never repost.** |
| present | present | **resolved by contributor** | **Improperly resolved.** Finding still applies on the new head. | **Un-resolve + reply.** GraphQL `unresolveReviewThread`; reply with the improper-resolution body shape (§9). Append maintainer ping per §4. Increment `improperly resolved` in Since-last-run. |
| present | present | not resolved, but contributor has replied | Possible disagreement | **Reply + escalate** per §11. Increment `disagreements escalated` in Since-last-run. |
| present | absent | not resolved | Cleanly fixed | **Resolve:** reply `Fixed in <sha>.` + GraphQL `resolveReviewThread`. |
| present | absent | already resolved | Cleanly fixed and acknowledged | **Reply only:** `Fixed in <sha>.` (no need to re-resolve). |
| absent | present, same `(file, symbol)` as a prior but different `finding_class` | n/a | Author attempted a fix that left a different problem in the same spot | **Incorrect-fix follow-up:** new inline comment, body starts with `Follow-up to <link to prior>: <new issue>`. |
| absent | present, no related prior | n/a | Brand-new finding (new code) | **Post a new comment.** |

### Phase D — Update per-agent review metadata

Dismiss the prior review and submit a new one with updated hidden
metadata. Update cumulative tag counts, `outstanding`, `run`,
`since_last_run`, verdict. The
Since-last-run metadata carries six counters:

- `X resolved` — prior findings the agent cleanly resolved this run.
- `Y still open` — prior findings that still apply (unchanged threads).
- `Z newly added` — brand-new findings posted this run.
- `W incorrect-fix follow-ups` — same-spot-different-finding-class new
  comments posted this run.
- `V improperly resolved` — threads the agent had to un-resolve this
  run because the finding still applied.
- `U disagreements escalated` — threads on which contributor pushback
  triggered a maintainer ping this run.

### Cumulative columns under re-review

- Tag columns increment when new findings appear.
- Tag columns NEVER decrement on resolution. (Priority 1 guarantee.)
- `outstanding` = (cumulative findings) − (resolved findings).

### Resolution mechanism

Belt-and-suspenders, using the appropriately-permissioned `TOKEN` the
external trigger provides:

- GraphQL `resolveReviewThread` mutation collapses the thread in the
  PR UI.
- A REST reply `Fixed in <commit-sha>.` keeps an audit trail.

If `resolveReviewThread` fails, the reply alone is acceptable
degradation; the agent still decrements `outstanding` and increments
"resolved" in Since-last-run.

### Guardrails (never)

- **Never repost** a finding whose `finding-key` matches an existing
  comment from the same agent on this PR.
- **Never resolve** a comment whose `finding-key` is still present on
  the new head, even if the author replied "fixed".
- **Never silently accept** a contributor's resolution of a thread
  whose `finding-key` is still present. Un-resolve it and reply per
  the improperly-resolved row of phase C.
- **Never argue.** On disagreement, the agent posts one polite
  escalation reply + maintainer ping, then stops. Further back-and-
  forth is for the maintainer (§11).
- **Never decrement** a tag column. Resolution decrements only
  `outstanding`.

---

## 8. Agent priorities, in order

Tiebreakers when other contract rules underdetermine behavior. Apply
in strict order; the earlier wins.

**Priority 1 — Do not discard or omit findings.**
- If the agent saw something in-scope, it posts. Tag conveys
  importance; the agent does not gatekeep on "is it worth saying?"
- Low confidence is not a reason to omit (§4).
- Every currently-true finding is reflected in the agent's summary
  counts even if its comment was inherited from a prior run.
- Tag columns are cumulative because of this priority.

**Priority 2 — Prefer suggestions over plain comments.**
- Whenever the agent can express a concrete one-or-few-line diff,
  attach a fenced ` ```suggestion ` block so the reviewer one-click
  applies.
- A best-effort suggestion the agent isn't 100% sure of is still
  preferred over no suggestion at all — add `(best-effort fix; verify
  before applying)` and tag the maintainer per §4.
- Priority 2 never overrides Priority 1.

**Priority 3 — Be succinct.**
- One finding per inline comment.
- Comment **prose** body ≤ 6 lines. The suggestion block itself does
  not count toward the line budget — GitHub renders it specially and
  it is the *useful* part of the comment.
- No restating context the reviewer sees in the diff.
- Summary tables are tables; no narrative around them.
- Outstanding-must-fix bullets are one line each.
- Aggregator's top-level comment fits roughly one screen on average.
- Priority 3 never overrides Priorities 1 or 2.

---

## 9. Inline comment body shape

Three shapes, depending on whether the comment is a fresh finding, an
improper-resolution reply, or a disagreement escalation.

### Fresh finding (initial post on a thread)

```
**<tag>** <one-line description of the issue>

<≤ 5 prose lines of why it matters / how to verify>

```suggestion
<concrete fix>          (omitted if no fix expressible)
```

(low-confidence-only)
cc @<maintainer1> @<maintainer2> — low-confidence finding, please confirm.

<!-- fprime-agent: <name>; finding-key: <key>; v1 -->
```

Total prose ≤ 6 lines. Suggestion block, maintainer-ping line, and
HTML footer don't count toward the line budget.

### Improper-resolution reply

Posted by the agent on a thread it un-resolved (see §7, phase C).

```
**Improperly resolved.** This finding still applies on <sha>.

<≤ 3 prose lines: what the agent re-checked and why the finding
remains live; link to the prior comment if helpful>

cc @<maintainer1> @<maintainer2> — contributor resolved without addressing; please adjudicate.

<!-- fprime-agent: <name>; finding-key: <key>; v1; reply-kind: improper-resolution -->
```

### Disagreement escalation reply

Posted by the agent on a thread where the contributor has pushed back
and the finding still applies (see §11).

```
**Disagreement — escalating.** I still flag this on <sha>; the contributor's response above indicates we disagree.

<≤ 3 prose lines: what the agent re-checked; one sentence
acknowledging the contributor's point and one sentence on why the
agent still flags the finding>

cc @<maintainer1> @<maintainer2> — needs human adjudication.

<!-- fprime-agent: <name>; finding-key: <key>; v1; reply-kind: disagreement -->
```

The `reply-kind` HTML attribute lets the agent recognize its own prior
escalation replies on subsequent runs and avoid double-escalating the
same thread.

---

## 10. Posting mechanics

Inline review comments are posted through the GitHub Pull Request
Review API. Mechanics, the suggestion-block syntax, the GraphQL
mutations (`resolveReviewThread`, `unresolveReviewThread`), and the
`TOKEN` env var live in `skills/post-inline-review.skill.md`.

Each reviewer submits a single PR review (event: `COMMENT`) whose
body is the hidden metadata block from §2 and whose inline comments
are the findings.

The aggregator submits a single PR review keyed by its HTML marker
(`<!-- fprime-review-summary v1 -->`). The review event is:

- **`APPROVE`** when both CI safety and Merge readiness are `Go`.
- **`REQUEST_CHANGES`** when either verdict is `No-Go`.

The review body contains the consolidated summary table (see
`review-summary.agent.md`). On re-runs, the aggregator dismisses
its prior review and submits a new one with the updated verdict and
body, since the review event (APPROVE vs. REQUEST_CHANGES) may
change between runs.

---

## 11. Disagreement handling

When the contributor pushes back on a comment via replies (rather than
resolving the thread), the agent does not argue or re-litigate. It
elevates to a code owner once and stops.

### Detection

During phase C (§7), a thread qualifies for disagreement escalation
iff ALL of the following hold:

1. The prior `finding-key` is still present on the new head (the
   finding still applies in the agent's view).
2. The thread is **not** resolved (resolved threads use the
   improperly-resolved row of phase C instead).
3. The thread has at least one reply from a user other than the
   agent itself.
4. The agent has **not** already posted a `reply-kind: disagreement`
   reply on this thread in a prior run (the HTML attribute on the
   agent's own replies is the de-dup key — escalate once per thread).

Low-confidence findings (§4) are still eligible: the maintainer ping
is the whole point.

### Action

1. **Reply once** on the thread using the disagreement-escalation
   reply shape (§9). One reply, no further back-and-forth.
2. **Tag the code owner / maintainer** via
   `skills/maintainer-lookup.skill.md` (same 4-step lookup as §4).
3. **Do not resolve the thread.** Leave it open for the maintainer.
4. **Increment `disagreements escalated`** in the per-agent review's
   `since_last_run` metadata (§7 phase D).

### Relation to low-confidence pings (§4)

- Low-confidence ping = on the **initial** comment, because the
  agent isn't fully sure.
- Disagreement ping = on a **reply**, because the contributor and
  the agent disagree and a human needs to adjudicate.

Both use the same maintainer-lookup skill; the trigger and posting
site differ. A given thread can in principle carry both
(low-confidence on the first post; disagreement on a later reply) —
each ping happens at most once per thread.

---

## 12. Roles

Each entry in `agent-registry.yml` carries a `role` field:

- `orchestrator` — the single human entry point. Invokes reviewers
  and aggregator. Does not post comments itself.
- `reviewer` — posts inline comments only (no visible summary table).
  Submits a single PR review (event: `COMMENT`) whose body contains
  only a hidden metadata block (§2) and whose inline comments are the
  findings.
- `aggregator` — consumes per-agent hidden metadata and inline
  comments, then submits ONE PR review with event `APPROVE` or
  `REQUEST_CHANGES` based on the consolidated Go/No-Go verdict.

The orchestrator iterates over `role: reviewer` entries to drive
reviewers, then invokes the `role: aggregator` entry.
