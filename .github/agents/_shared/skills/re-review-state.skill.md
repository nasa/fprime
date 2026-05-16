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
<!-- fprime-agent: <agent-name>; finding-key: <hex>; v1[; reply-kind: <kind>] -->
```

Filter to comments where `<agent-name>` matches `<self>`. The
matching set is the agent's prior comments on this PR.

### 1b. Fetch each thread's resolution status and reply chain

Use the GraphQL query in `post-inline-review.skill.md` §5. For each
prior comment, retrieve:

- `thread.id` (needed for `resolveReviewThread` /
  `unresolveReviewThread`).
- `thread.isResolved` (drives the improperly-resolved case).
- `thread.resolvedBy.login` (recorded for audit; not used in the
  decision).
- The `thread.comments[]` list (drives disagreement detection: the
  agent looks for any comment authored by a user other than itself).

### 1c. Index by finding-key

Build a dictionary keyed by `finding-key` whose value is `{ comment_id,
thread_id, path, line, is_resolved, has_contributor_replies,
has_prior_disagreement_reply }`. `has_prior_disagreement_reply` is
true iff a comment in the thread carries
`reply-kind: disagreement` in its HTML footer.

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

| `thread.isResolved` | `has_contributor_replies` AND NOT `has_prior_disagreement_reply` | Action |
|---|---|---|
| `true` | — | **Improperly resolved.** Un-resolve + reply (see §3a-i). |
| `false` | `true` | **Disagreement escalation.** Reply once + maintainer ping (see §3a-ii). |
| `false` | `false` | **Do nothing.** Leave the comment as-is. **Never repost.** |

#### 3a-i. Improper-resolution action

1. Call `unresolveReviewThread(input: { threadId })` (GraphQL).
2. POST a reply on the thread with the improper-resolution body
   shape from review contract §9.
3. Append a maintainer ping per `maintainer-lookup.skill.md`. The
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
2. Append a maintainer ping per `maintainer-lookup.skill.md`. Always
   pings (the whole point is human adjudication).
3. Do NOT resolve the thread. Leave it open for the maintainer.
4. Do NOT repost the original finding.
5. Increment `disagreements escalated` in the Since-last-run
   counter.

### 3b. Threads where prior present but current absent (`resolved`)

For each `k` in `resolved`:

| `thread.isResolved` | Action |
|---|---|
| `false` | **Clean resolution.** Reply `Fixed in <head-sha>.` + GraphQL `resolveReviewThread`. |
| `true` | **Acknowledged.** Reply `Fixed in <head-sha>.` only — no need to re-resolve. |

Increment `resolved` in Since-last-run. Decrement `outstanding` (do
NOT decrement any tag column).

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
  **<tag>** Follow-up to <link to prior comment>: <new issue>
  ```

  Then follow the normal fresh-finding body shape from review
  contract §9.

  Increment `incorrect-fix follow-ups` in Since-last-run AND
  `newly added` (an incorrect-fix follow-up is also a newly-added
  comment).

- Otherwise: **brand-new finding.** POST a new inline comment per
  the fresh-finding body shape. Increment `newly added` in
  Since-last-run.

---

## 4. Phase D — Update the per-agent summary

Edit the prior summary review (located by the HTML marker) in place.

Update:

- The four tag columns: increment for any newly-posted comments
  (incorrect-fix follow-ups and brand-new findings). Never decrement.
- The `outstanding` column: recompute as
  `(cumulative tag-column sum) − (cumulative resolved count)`.
- The `Verdict:` line: `Go` iff outstanding must-fix == 0, else
  `No-Go`.
- The `Run:` line: increment the run ordinal.
- The `Since last run:` line: write the six counters
  (`resolved`, `still open`, `newly added`, `incorrect-fix
  follow-ups`, `improperly resolved`, `disagreements escalated`).

`still open` = `|intersect|` (after subtracting improperly-resolved
and disagreement-escalated entries, since those are accounted in
their own counters but still represent the same finding-keys that
"remain open"; in the simple accounting model `still open` is
`|intersect|` and the other two counters are subsets reported
separately).

The cumulative tag columns and outstanding-driven verdict are
defined in the review contract §2.

---

## 5. Guardrails (do not violate)

- **Never repost** a finding whose `finding-key` matches an existing
  comment from the same agent on this PR.
- **Never resolve** a comment whose `finding-key` is still in
  `current_keys`.
- **Never silently accept** a contributor's resolution of a thread
  whose `finding-key` is still present. Un-resolve and reply per the
  improperly-resolved flow.
- **Never argue.** On disagreement, the agent posts ONE escalation
  reply + maintainer ping. Subsequent runs leave the thread alone
  (the de-dup key is the `reply-kind: disagreement` HTML attribute).
- **Never decrement** a tag column. Resolution affects only
  `outstanding`.

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
heuristic is: post a fresh comment and prefix the body with `(This
issue was previously resolved on <commit-sha> and was re-introduced.)`

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
B: re-run analysis, compute current finding-keys.
C: decide per row of the contract §7 table — do-nothing,
resolve, reply-improper-resolution, reply-disagreement, post-new,
post-incorrect-fix-follow-up.
D: update the summary block in place.`
