---
name: re-review-state
description: Use when re-running a review on a PR that already has prior comments; computes stable finding-keys and decides per finding whether to do nothing, reply-fixed and resolve, escalate as improperly resolved, escalate as a disagreement, or post an incorrect-fix follow-up.
---

# Skill: Re-review state (phases A–D, finding-keys, decision table)

This skill is the procedural mechanics of how a reviewer agent
behaves on the second-and-later run on the same PR. It is referenced
by every reviewer agent and by the aggregator.

The behavioral rules and the decision table live in the review
contract §7. This skill walks through the algorithm in detail.

---

## 1. Phase A — Inventory prior comments

### 1a. Fetch the agent's prior inline comments

```http
GET /repos/{owner}/{repo}/pulls/{pull_number}/comments?per_page=100
Authorization: Bearer ${TOKEN}
Accept: application/vnd.github+json
```

Paginate via the `Link` header until exhausted. For each comment,
parse the trailing HTML footer:

```
<!-- fprime-agent: <agent-name>; finding-key: <hex>; site-key: <hex>; v2[; reply-kind: <kind>] -->
```

(Legacy `v1` footers omit `site-key`; parse both forms.)

Keep **all** agent-authored comments (any `fprime-agent:` footer):

- Comments where `<agent-name>` matches `<self>` are the agent's
  prior comments on this PR (drive phases C–D as before).
- ALL agent-authored comments — own and others' — are additionally
  indexed by `site-key` for the cross-agent concurrence check
  (review contract §6a). For `v1` comments without a site-key,
  recompute it best-effort from the comment's path and anchor
  context.

### 1b. Fetch each thread's resolution status and reply chain

Use the GraphQL query in `post-inline-review` §5. For each
prior comment, retrieve:

- `thread.id` (needed for `resolveReviewThread` /
  `unresolveReviewThread`).
- `thread.isResolved` (drives the maintainer-adjudicated and
  improperly-resolved cases).
- `thread.resolvedBy.login` (decides between those two cases: a login
  in the core-maintainer set from `maintainer-lookup` §1b means
  adjudicated; anyone else means improperly resolved).
- The `thread.comments[]` list (drives disagreement detection: the
  agent looks for any comment authored by a user other than itself).

### 1b-bis. Locate the agent's prior metadata review

```http
GET /repos/{owner}/{repo}/pulls/{pull_number}/reviews?per_page=100
```

Paginate; keep the review whose body starts with
`<!-- fprime-agent: <self> v1 -->` (there is exactly one per agent for
the life of the PR; if several exist from before in-place updates,
take the newest). Record its `id` (Phase D updates it in place), its
`run` line, and `last_reviewed_head`:

- the `reviewed_head` line of the body, if present;
- otherwise the review's `commit_id` (metadata written before that
  field existed).

If no such review exists, this is run 1 for the agent: Phase B uses
the full PR diff and Phase D posts a fresh metadata review.

### 1c. Index by finding-key and site-key

Build a dictionary keyed by `finding-key` (own comments only) whose
value is `{ comment_id, thread_id, path, line, is_resolved,
resolved_by_maintainer, has_contributor_replies,
has_prior_disagreement_reply }`. `resolved_by_maintainer` is true iff
`is_resolved` and `resolvedBy.login` is in the core-maintainer set.
`has_prior_disagreement_reply` is true iff a comment in the thread
carries `reply-kind: disagreement` in its HTML footer.

Build a second dictionary keyed by `site-key` (ALL agent-authored
comments) whose value is a list of `{ agent_name, comment_id,
thread_id, path, is_resolved, tag, body,
has_own_concurrence_reply, closed_as_duplicate }`.
`has_own_concurrence_reply` is true iff the thread carries a
`reply-kind: concurrence` reply from `<self>`. `closed_as_duplicate`
is true iff the thread carries a `reply-kind: duplicate-close` reply
from the aggregator (`fprime-review-summary` footer); such threads
are not concurrence targets — follow the link to the canonical
thread instead.

---

## 2. Phase B — Run scope checker on the new head

Run the agent's analysis on the PR head SHA. Produce the
`current_findings` set: a list of `{ finding-key, path, line, tag,
body }` tuples.

The `finding-key` is computed identically to phase A:

```
finding-key = sha256(
    agent_name + "|" +
    file_path  + "|" +
    anchor     + "|" +
    finding_class
)
```

The agent-agnostic `site-key` (review contract §6a) reuses the same
inputs minus the agent and class:

```
site-key = sha256(file_path + "|" + anchor)
```

Where:

- `agent_name` is the agent's short name from the registry.
- `file_path` is the repo-root-relative path of the offending line.
- `anchor` is **not** a line number. Computation:
  1. Resolve the **enclosing symbol name** at the offending line —
     the deepest containing function / method / class / FPP entity.
     Use the language-appropriate tooling (`ctags`, `clang -ast-dump`,
     `fpp`-aware parser for `.fpp` files). For `.md` / `.yml` files,
     the section heading or top-level YAML key serves as the symbol.
  2. Compute a **40-character line fingerprint**: the offending
     line, stripped of leading/trailing whitespace and collapsed
     interior whitespace, truncated or padded to exactly 40
     characters with right-space padding.
  3. `anchor = enclosing_symbol + ":" + fingerprint`.
  4. If the symbol cannot be resolved (rare), fall back to
     `anchor = "<file>:" + fingerprint`. The fingerprint alone is
     still stable across line drift.
- `finding_class` is the agent-defined name of the finding (e.g.,
  `ground-reachable-assert`, `cmd-arg-overflow`, `unverified-action`).
  Each agent's `.agent.md` enumerates its finding classes.

The same finding (same symbol, same line content, same class) will
produce the same `finding-key` across reformatting and line drift,
which is the whole point.

### 2a. Re-review scope for new below-must-fix findings

Rule text: review contract §7 Phase B. Mechanics:

1. Compute the incremental diff since the last pass. Preferred: in
   the local clone of the review-target repo, after fetching the PR
   head (`git fetch origin pull/{n}/head`),

   ```bash
   git diff --unified=0 <last_reviewed_head>...<head_sha>
   ```

   If `last_reviewed_head` is not in the local object store (the
   author force-pushed and the old head was discarded), or no clone
   is available, use
   `GET /repos/{owner}/{repo}/compare/{last_reviewed_head}...{head_sha}`
   instead — noting that its `files[]` list is capped at 300 entries
   and is not paginated. Build `delta_hunks`: for each file, the set
   of added / modified line ranges on the head side.
2. Widen `delta_hunks` with **newly reached** code: symbols whose
   callers were added or changed inside `delta_hunks`, per the
   introduced/preexisting rules of `pr-diff-scoping` applied to this
   incremental diff.
3. For each `f` in `current_findings` with **no prior finding-key**
   and `f.tag != must fix`: keep `f` only if `f.path/f.line` falls in
   `delta_hunks` or its enclosing symbol is newly reached. Drop it
   otherwise — it is outside this run's scope and is not counted in
   any column. Must-fix candidates, incorrect-fix follow-ups (§3c),
   and every finding with a prior key are never dropped.
4. Fall back to **no scoping** (full PR diff, all tiers) when: there
   is no prior metadata review; `last_reviewed_head == head_sha`;
   neither `git diff` nor the compare call can resolve
   `last_reviewed_head` (404 / unknown revision); or the compare
   response has exactly 300 files (possible truncation). Always widen
   on doubt; never narrow.

Apply this filter *before* the site-key concurrence check in §3c so
that out-of-scope observations do not generate concurrence replies
either.

---

## 3. Phase C — Match and act

The full decision table is in the review contract §7. Operationally:

```
prior_keys   = { k : k in agent.prior_comments_index }
current_keys = { k : k in current_findings }

intersect    = prior_keys ∩ current_keys
resolved     = prior_keys − current_keys
new          = current_keys − prior_keys
```

### 3a. Threads where prior and current both present (`intersect`)

For each `k` in `intersect`, decide which row of the table applies:

| `thread.isResolved` | `resolved_by_maintainer` | `has_contributor_replies` AND NOT `has_prior_disagreement_reply` | Action |
|---|---|---|---|
| `true` | `true` | — | **Maintainer adjudicated.** Do nothing (see §3a-0). |
| `true` | `false` | — | **Improperly resolved.** Un-resolve + reply (see §3a-i). |
| `false` | — | `true` | **Disagreement escalation.** Reply once + maintainer ping (see §3a-ii). |
| `false` | — | `false` | **Do nothing.** Leave the comment as-is. **Never repost.** |

#### 3a-0. Maintainer-adjudicated action

The maintainer has ruled the finding does not need fixing. Post
nothing, do not un-resolve, do not repost. No counter bookkeeping
here: the thread is `isResolved`, so the Phase D recomputation (§4)
counts it in `R` — on this run, on every later run, and after the
finding-key eventually disappears (§3b) — exactly once. Do not also
report it under `still open`.

#### 3a-i. Improper-resolution action

1. Call `unresolveReviewThread(input: { threadId })` (GraphQL).
2. POST a reply on the thread with the improper-resolution body
   shape from review contract §9.
3. Append a maintainer ping per `maintainer-lookup`. The
   improper-resolution case ALWAYS pings the maintainer (not gated
   on confidence) because the contributor's resolve action conflicts
   with the agent's re-analysis.
4. Increment `improperly resolved` in the Since-last-run counter.
5. Do NOT decrement any tag column. Do NOT decrement `outstanding`.

If `unresolveReviewThread` fails with a permissions error, post the
reply anyway and increment the counter. The thread remains visibly
resolved but the reply + maintainer ping is visible inline.

#### 3a-ii. Disagreement-escalation action

1. POST a reply on the thread with the disagreement-escalation body
   shape from review contract §9.
2. Append a maintainer ping per `maintainer-lookup`. Always
   pings (the whole point is human adjudication).
3. Do NOT resolve the thread. Leave it open for the maintainer.
4. Do NOT repost the original finding.
5. Increment `disagreements escalated` in the Since-last-run
   counter.

### 3b. Threads where prior present but current absent (`resolved`)

For each `k` in `resolved`:

| `thread.isResolved` | Action |
|---|---|
| `false`, no own `Fixed in` reply | **Clean resolution.** Reply `[<review_label>] Fixed in <head-sha>.` + GraphQL `resolveReviewThread`. |
| `false`, own `Fixed in` reply present | **Resolve failed earlier** (permissions). Retry `resolveReviewThread` once; do not reply again. |
| `true` | **Already settled** (by the agent on an earlier run, a core maintainer, or the contributor after fixing). Do nothing — no reply, no re-resolve. |

No counter bookkeeping here either; `outstanding` and `resolved` come
from the Phase D recomputation (§4). Never decrement any tag column.

### 3c. Current findings with no prior match (`new`)

For each `k` in `new`:

- If there exists a prior finding `p` such that:
  - `p.path == new.path`
  - `p.enclosing_symbol == new.enclosing_symbol`
  - `p.finding_class != new.finding_class`

  This is an **incorrect-fix follow-up**: the contributor attempted
  a fix that resolved the original finding but left a different one
  in the same spot. POST a new inline comment whose body begins:

  ```
  [<review_label>] **<tag>** Follow-up to <link to prior comment>: <new issue>
  ```

  Then follow the normal fresh-finding body shape from review
  contract §9.

  Increment `incorrect-fix follow-ups` in Since-last-run AND
  `newly added` (an incorrect-fix follow-up is also a newly-added
  comment).

- Otherwise, run the **cross-agent concurrence check** (review
  contract §6a): look up `new.site-key` in the all-agents site-key
  index. If another agent has an OPEN thread (not resolved, not
  `closed_as_duplicate`) at the same site-key describing the **same
  underlying issue**:

  - Do NOT open a new thread. POST one concurrence reply on that
    thread per the concurrence body shape (review contract §9),
    unless `has_own_concurrence_reply` is already true (one
    concurrence per agent per thread).
  - Still count the finding in the agent's own metadata (tag column,
    `outstanding`, `newly added`) at the agent's own severity.
  - On later runs, the agent's Phase C tracks this finding on the
    shared thread (resolve semantics per review contract §6a).

  If the site-key matches but the issue is genuinely different, post
  normally — the site-key alone never suppresses a distinct finding.

- Otherwise: **brand-new finding.** POST a new inline comment per
  the fresh-finding body shape. Increment `newly added` in
  Since-last-run.

---

## 4. Phase D — Update the per-agent metadata review

Rewrite the body of the prior metadata review located in §1b-bis
**in place**:

```http
PUT /repos/{owner}/{repo}/pulls/{pull_number}/reviews/{review_id}
{ "body": "<updated metadata block>" }
```

This edits only the review's summary body; inline comments attached
to that review (run 1's findings) are untouched, and no new
notification or timeline entry is produced. Never dismiss the
metadata review (GitHub rejects dismissal of `COMMENTED` reviews
with a 422) and never post a second one. Only if the `PUT` fails
with `404`/`403` (e.g. the prior review was authored under a
different token identity) fall back to submitting a fresh
metadata-only review; later runs take the newest marker match.

Any **new** inline comments from Phase C go in one separate review
with `body: ""` (`post-inline-review` §4); if there are none, post
no review at all — the body update above is the entire footprint of
a quiet run.

Update:

- The `reviewed_head` line: set to the head SHA analyzed this run.
- The four tag columns: increment for any newly-posted comments
  (incorrect-fix follow-ups and brand-new findings). Never decrement.
- The `outstanding` column: recompute from thread state, never by
  adjusting the prior value. After the Phase C actions above, re-query
  the threads the agent counts in its tag columns (its own comments
  plus threads carrying its concurrence reply, §3c) and let `R` =
  those with `isResolved == true`, plus unresolved ones carrying an
  own `Fixed in` reply (resolve failed on permissions), minus any the
  agent tried to un-resolve this run (§3a-i, even if that failed).
  Then `outstanding = (cumulative tag-column sum) − R`. Adjudicated
  (§3a-0), agent-resolved (§3b), and contributor-resolved-after-fixing
  threads are all simply members of `R`; a thread is counted once
  regardless of how many runs it has been resolved for.
- The `Verdict:` line: `Go` iff outstanding must-fix == 0, else
  `No-Go`.
- The `Run:` line: increment the run ordinal.
- The `Since last run:` line: write the six counters
  (`resolved`, `still open`, `newly added`, `incorrect-fix
  follow-ups`, `improperly resolved`, `disagreements escalated`).

`resolved` = `max(0, R − R_prev)` where
`R_prev = (prior cumulative tag-column sum) − (prior outstanding)`,
both read from the prior metadata body. Threads that became resolved
between runs are counted once, whoever resolved them.

`still open` = `|intersect|` minus maintainer-adjudicated (§3a-0),
improperly-resolved and disagreement-escalated entries; the latter
two are reported in their own counters.

The cumulative tag columns and outstanding-driven verdict are
defined in the review contract §2.

---

## 5. Guardrails (do not violate)

- **Never repost** a finding whose `finding-key` matches an existing
  comment from the same agent on this PR.
- **Never resolve** a comment whose `finding-key` is still in
  `current_keys`.
- **Never silently accept** a non-maintainer's resolution of a thread
  whose `finding-key` is still present. Un-resolve and reply per the
  improperly-resolved flow.
- **Never reopen, reply to, or repost** a thread resolved by a core
  maintainer (§3a-0).
- **Never argue.** On disagreement, the agent posts ONE escalation
  reply + maintainer ping. Subsequent runs leave the thread alone
  (the de-dup key is the `reply-kind: disagreement` HTML attribute).
- **Never decrement** a tag column. Resolution affects only
  `outstanding`.
- **Never open a new thread** at a site-key where another agent's
  open thread already describes the same issue — concur instead
  (review contract §6a).
- **Never un-resolve** a thread carrying a `reply-kind:
  duplicate-close` reply from the aggregator; the linked canonical
  thread is the live home of the finding.

---

## 6. Edge cases

### 6a. The same offending line is re-introduced after a clean resolution

Prior run resolved the finding. A later commit re-introduces the
same offending line in the same symbol. The `finding-key` is
identical to a prior (resolved) one.

Behavior: the agent posts a **new** inline comment. The cumulative
tag column increments. `outstanding` increments. The prior resolved
thread stays resolved (it's a different comment now). Since-last-run
reports the new comment under `newly added`.

This is suboptimal — ideally the comment would re-open the prior
thread — but GitHub does not support that operation. The agent's
heuristic is: post a fresh comment and prefix the body (after the
reviewer label) with `(This issue was previously resolved on
<commit-sha> and was re-introduced.)`

### 6b. The contributor rewrites history (force-push)

If the prior commits the agent reviewed are no longer in the PR
history, the GitHub comments are likely orphaned (their line
positions are no longer valid). The agent treats every prior comment
as if its `finding-key` is absent from `current_keys` ONLY if
GitHub's response indicates the comment is on a nonexistent commit;
otherwise the comments are still attached to the rewritten history
and the normal flow applies.

### 6c. The contributor closes the thread without addressing AND replies

Both improperly-resolved and disagreement-escalation conditions are
true. The improperly-resolved row takes precedence (it's the more
specific case). The agent un-resolves, replies with the
improper-resolution shape, and increments `improperly resolved`. The
escalation handles disagreement-via-resolve and disagreement-via-
reply in one motion; no need to double-post.

If the thread was instead resolved by a core maintainer, §3a-0 wins
over both: the maintainer has adjudicated the disagreement, and the
agent posts nothing.

### 6d. The aggregator FAILED and is not on the PR

If the orchestrator reports the aggregator as FAILED, the reviewer
agents still complete their phases A–D normally. The aggregator's
top-level summary is the part missing. The orchestrator's status
message to the human operator covers this case.

### 6e. The TOKEN lacks `unresolveReviewThread` permission

Post the improper-resolution reply on the thread anyway. The thread
remains visibly resolved on GitHub but the inline reply +
maintainer ping makes the un-acknowledged finding visible. Increment
`improperly resolved` regardless.

---

## 7. One-line summary

`A: index prior comments by finding-key plus thread state.
B: re-run analysis, compute current finding-keys; scope new
below-must-fix findings to the diff since last_reviewed_head.
C: decide per row of the contract §7 table — do-nothing,
resolve, accept-maintainer-adjudication, reply-improper-resolution,
reply-disagreement, post-new,
post-incorrect-fix-follow-up.
D: PUT the updated metadata body onto the existing review
(reviewed_head, counts, run, since_last_run, verdict); post new
inline comments, if any, in one empty-body review.`
