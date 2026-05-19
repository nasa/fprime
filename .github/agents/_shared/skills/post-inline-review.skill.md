---
name: post-inline-review
description: Use when posting inline review comments, the per-agent summary review, or interacting with GitHub review threads (resolve, un-resolve, reply) for an F Prime PR review agent.
---

# Skill: Post an inline review on a GitHub PR

Reusable procedure for any F Prime review agent that needs to post
inline review comments, the per-agent summary review, or interact with
GitHub review threads (resolve / un-resolve / reply).

This skill assumes:

- An appropriately-permissioned token is exposed as `${TOKEN}` in the
  environment (provided by whatever external trigger invoked the
  agent). Required scopes: read access to the repository, write
  access to pull-request reviews, and write access to discussions /
  review threads for the GraphQL mutations.
- The agent knows the owner, repo, PR number, head commit SHA, and
  its own short name (matches the `agent-registry.yml` entry).

---

## 1. Suggestion-block syntax

GitHub renders a fenced block whose info string is exactly
`suggestion` as a one-click "Apply suggestion" diff against the line
range the comment is anchored to.

```markdown
**must fix** Unbounded copy from ground argument.

Validate `len` against the destination buffer before copying. The
incoming `len` is ground-controlled and can exceed `sizeof(dst)`.

```suggestion
if (len > sizeof(dst)) { return Status::INVALID_LENGTH; }
memcpy(dst, src, len);
```

<!-- fprime-agent: security-review; finding-key: abc123…; v1 -->
```

The suggestion block replaces the entire line range the comment is
anchored to. For multi-line replacements, anchor the comment to the
full range (`start_line` + `line`) rather than a single line.

---

## 2. Posting one review with many inline comments

A single PR review can carry many inline comments. Prefer one review
per agent run rather than many small reviews — the GitHub UI groups
them together.

**First run** — the metadata block (see §4) goes in `body` alongside
the inline `comments[]`:

```http
POST /repos/{owner}/{repo}/pulls/{pull_number}/reviews
Authorization: Bearer ${TOKEN}
Accept: application/vnd.github+json
Content-Type: application/json

{
  "commit_id": "<head SHA>",
  "event": "COMMENT",
  "body": "<per-agent hidden metadata block — see §4>",
  "comments": [
    {
      "path": "Svc/CmdDispatcher/CmdDispatcher.cpp",
      "line": 142,
      "side": "RIGHT",
      "body": "**must fix** … \n\n<!-- fprime-agent: security-review; finding-key: abc; v1 -->"
    },
    {
      "path": "Svc/CmdDispatcher/CmdDispatcher.cpp",
      "start_line": 200,
      "line": 207,
      "start_side": "RIGHT",
      "side": "RIGHT",
      "body": "**suggestion** … \n\n```suggestion\n…\n```\n\n<!-- fprime-agent: security-review; finding-key: def; v1 -->"
    }
  ]
}
```

**Re-run** — the inline-comments review has an **empty `body`** (no
metadata); the metadata is dismissed and resubmitted separately per
§4:

```json
{
  "commit_id": "<head SHA>",
  "event": "COMMENT",
  "body": "",
  "comments": [ ... ]
}
```

`event: COMMENT` is correct for all **reviewer** agents — never
`APPROVE` and never `REQUEST_CHANGES` (the merge-readiness verdict
is the aggregator's job, not the individual reviewer's).

The **aggregator** (`review-summary`) uses `APPROVE` or
`REQUEST_CHANGES` based on its CI safety and merge readiness
verdicts — see review-contract.md §10.

`commit_id` MUST be the head SHA the agent analyzed; this is what
binds the comments to specific line positions.

---

## 3. Replying to an existing review thread

Replies post to the in-line comment that started the thread (the
`in_reply_to` field).

```http
POST /repos/{owner}/{repo}/pulls/{pull_number}/comments/{comment_id}/replies
Authorization: Bearer ${TOKEN}
Content-Type: application/json

{
  "body": "Fixed in <commit-sha>.\n\n<!-- fprime-agent: security-review; v1; reply-kind: resolution -->"
}
```

Replies are used for:

- `Fixed in <sha>.` after a clean resolution.
- The **Improperly resolved.** reply on an un-resolved thread (see
  the improper-resolution body shape in the review contract §9).
- The **Disagreement — escalating.** reply when contributor pushback
  meets the escalation criteria (review contract §11).

---

## 4. Per-agent hidden metadata review

On first run, the metadata block lives in the `body` of the
combined review described in §2 (which also carries the inline
`comments[]` array). There is no separate metadata-only review on
first run.

On re-run, the inline comments go in a fresh review with an empty
`body` (see §2 re-run template). The metadata is handled
separately: dismiss the prior metadata review via:

```http
PUT /repos/{owner}/{repo}/pulls/{pull_number}/reviews/{review_id}/dismissals
```

Then submit a new metadata-only review (`event: COMMENT`, no
`comments[]` array) with the updated body. The review body contains
**only** HTML-comment metadata (counts, verdict, run ordinal,
since-last-run) — no visible summary table. The HTML marker is
the de-dup key.

---

## 5. Resolving a review thread (GraphQL)

Two mutations, both keyed by the **thread ID** (NOT the comment ID).
The thread ID is fetched via a GraphQL query against
`pullRequest.reviewThreads` filtered by the comment's databaseId.

### resolveReviewThread

```graphql
mutation Resolve($threadId: ID!) {
  resolveReviewThread(input: { threadId: $threadId }) {
    thread { id isResolved }
  }
}
```

Headers:

```
Authorization: Bearer ${TOKEN}
Accept: application/vnd.github+json
```

### unresolveReviewThread

```graphql
mutation Unresolve($threadId: ID!) {
  unresolveReviewThread(input: { threadId: $threadId }) {
    thread { id isResolved }
  }
}
```

Used by the improperly-resolved row of the re-review decision table
(review contract §7, phase C).

### Fetching thread state

```graphql
query Threads($owner: String!, $name: String!, $number: Int!) {
  repository(owner: $owner, name: $name) {
    pullRequest(number: $number) {
      reviewThreads(first: 100) {
        nodes {
          id
          isResolved
          resolvedBy { login }
          comments(first: 100) {
            nodes {
              id
              databaseId
              author { login }
              body
              createdAt
            }
          }
        }
      }
      # paginate via pageInfo.endCursor / hasNextPage as needed
    }
  }
}
```

The agent uses this to:

1. Index its own prior comments by `finding-key` (parsed out of the
   HTML footer).
2. Read `isResolved` and `resolvedBy.login` (drives the
   improperly-resolved decision).
3. Read the reply chain (drives disagreement detection in review
   contract §11).

---

## 6. Failure modes and fallbacks

| Failure | Fallback |
|---|---|
| `resolveReviewThread` returns `403` or the token lacks the discussion-write scope | Post the `Fixed in <sha>.` reply and proceed. The thread visibly remains open but the audit trail is preserved. Decrement `outstanding` and increment `resolved` in Since-last-run regardless. |
| `unresolveReviewThread` returns `403` | Post the improperly-resolved reply anyway. The thread remains visibly resolved on GitHub but the reply + maintainer ping is visible inline. Increment `improperly resolved` regardless. |
| Inline-comment POST returns `422 Pull Request Review thread cannot be created on this line of the diff` | The line is not in the PR's diff. Re-anchor to the nearest line that is in the diff (typically the function header) and prefix the comment body with `(Anchored above the offending line; the diff does not include line N.)` |
| Token missing entirely | Fail fast. The agent emits a single line to the orchestrator: `Cannot post review: TOKEN not provided.` and exits. The orchestrator treats this as a FAILED reviewer per review-summary.agent.md §5. |

---

## 7. Rate limits and retries

- Treat any `5xx` response as retryable with exponential backoff
  (1s, 2s, 4s, 8s, give up).
- Treat `429` and `403` with `X-RateLimit-Remaining: 0` as backoff
  per `X-RateLimit-Reset` header.
- Treat `403` without a rate-limit header as permission failure (no
  retry).
- Do NOT retry `422` errors — they indicate a malformed request and
  retrying will produce the same error.

---

## 8. Worked example: the full flow on one PR

1. Read PR head SHA. Bind every subsequent call to this SHA.
2. Fetch the agent's prior metadata review by HTML marker (review
   contract §6). Note its review ID, run count, and the
   `finding-key` index.
3. Run the agent's analysis on the new head. Compute the new
   `finding-key` set.
4. Match prior vs current per review contract §7 phase C. Build the
   action list: `post-new`, `reply-fixed`, `resolve-thread`,
   `reply-improper`, `unresolve-thread`, `reply-disagreement`,
   `post-incorrect-fix-followup`, `do-nothing`.
5. Execute the action list. Compose the per-agent hidden metadata
   block from the resulting state.
6. POST the umbrella review (inline comments + hidden metadata body)
   or, on re-run, POST the inline comments as a fresh review, dismiss
   the prior metadata review, and submit a new one with the updated
   metadata.
7. Return success to the orchestrator.

---

## External references

The endpoints and mutations referenced in this skill are documented
on the GitHub developer site. If a request behaves differently from
what this skill describes, the GitHub documentation is authoritative;
open a PR to update this skill so the next agent sees the corrected
behavior.

- GitHub REST API — Pulls: Reviews:
  https://docs.github.com/en/rest/pulls/reviews
- GitHub REST API — Pulls: Comments (inline review comments):
  https://docs.github.com/en/rest/pulls/comments
- GitHub REST API — Issues: Comments (top-level PR comments via the
  shared issue-comments endpoint):
  https://docs.github.com/en/rest/issues/comments
- GitHub GraphQL — `PullRequestReviewThread` object
  (`resolveReviewThread` / `unresolveReviewThread` mutations):
  https://docs.github.com/en/graphql/reference/objects#pullrequestreviewthread
- GitHub GraphQL — Mutations index:
  https://docs.github.com/en/graphql/reference/mutations
- GitHub REST API — Rate limiting and conditional requests:
  https://docs.github.com/en/rest/overview/resources-in-the-rest-api#rate-limiting

When the API surface evolves (endpoint paths, scope requirements,
response shapes), update the relevant section of this skill in the
same PR that addresses the change so downstream agents inherit the
new behavior automatically.
