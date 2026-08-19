---
name: triage-classifier
description: Use when picking one of the four review triage tags (must fix, suggestion, could fix, future work) for a finding, including the suggestion-block-versus-tag decoupling rule.
---

# Skill: Triage a finding into one of the four tags

Every inline comment posted by an F Prime review agent starts with a
reviewer label in brackets (e.g. `[Security]`) followed by exactly one
bolded tag. This skill is the decision procedure for picking the tag.
It is referenced by every reviewer agent.

The four tags and their meanings live in the review contract §1.
The decision below assumes the agent has already determined that a
finding is in-scope; this skill picks the tag.

---

## 1. Decision tree

```
                ┌─────────────────────────────────────────────┐
                │  Is the offending behavior introduced or    │
                │  widened by this PR?  (see                   │
                │  skills/pr-diff-scoping.skill.md §1)         │
                └─────────────────────┬───────────────────────┘
                          │ no                  │ yes
                          ▼                     ▼
                  ╔═══════════════╗     ┌──────────────────────┐
                  ║ future work   ║     │ How severe is the    │
                  ║ (preexisting)║     │ behavior?            │
                  ╚═══════════════╝     └─────────┬────────────┘
                                                  │
                                                  ▼
                              ┌─────────────────────────────────────┐
                              │ Severity ladder:                    │
                              │  - blocks merge / safety-critical?  │
                              │    → must fix                       │
                              │  - non-blocking + concrete fix?     │
                              │    → suggestion                     │
                              │  - non-blocking, minor, no fix?     │
                              │    → could fix                      │
                              └─────────────────────────────────────┘
```

**Suggestion-block usage is independent of the tag.** A fenced
` ```suggestion ` block may attach to any of `must fix`, `suggestion`,
or `could fix` whenever a concrete diff is available. See review
contract §1 and §9.

---

## 2. Severity rubrics

### `**must fix**`

Pick this tag iff the finding is **all** of:

- Introduced or widened by this PR.
- In-scope for the agent.
- Blocks merge (safety, security, correctness, policy violation, or
  CI integrity).

`**must fix**` always blocks merge in the per-agent verdict. Within
the CI-safety agents (security, supply-chain), an outstanding
`**must fix**` within CI-safety scope also forces `CI safety: No-Go`.

### `**suggestion**`

Pick this tag iff:

- Introduced or widened by this PR.
- In-scope for the agent.
- Non-blocking (i.e., the merge can proceed without the fix).
- The agent has a **concrete, expressible fix** that fits in a
  fenced ` ```suggestion ` block.

The `**suggestion**` tag MUST be paired with a suggestion block. If
no concrete fix is expressible, use a different tag.

### `**could fix**`

Pick this tag iff:

- Introduced or widened by this PR.
- In-scope for the agent.
- Non-blocking.
- Minor — not severe enough to warrant a `**suggestion**` tier even
  if a fix were available (e.g., a stylistic nit, a redundant null
  check, a non-functional naming inconsistency).
- May or may not include a suggestion block.

**`could fix` is NOT the default fallback.** If the agent has no
concrete fix to express but the finding is severity-worthy, choose
`**must fix**` (blocking) rather than `**could fix**` (minor).

### `**future work**`

Pick this tag iff:

- The finding is in-scope for the agent.
- The PR did NOT introduce or widen the offending behavior. The
  offending line is unchanged AND no new caller added by the PR
  widens its reach (see `pr-diff-scoping.skill.md`).
- The agent considered the finding worth recording for future work.

`**future work**` never blocks merge. It is the agent's mechanism for
acknowledging latent issues without burdening the current PR.

---

## 3. Anti-pattern: silent downgrading

The agent does **not** downgrade severity to avoid blocking a PR.

- A safety-critical finding that the agent can't write a suggestion
  for is still `**must fix**` — not `**could fix**`.
- A finding the agent is unsure about is still tagged at the right
  severity, with a maintainer ping appended (see review contract §4
  and `maintainer-lookup.skill.md`).

The three agent priorities (review contract §8) apply:

1. **Priority 1** — Do not discard or omit a finding.
2. **Priority 2** — Prefer suggestions over plain comments when a
   concrete fix is available.
3. **Priority 3** — Be succinct (≤ 6 lines of prose).

Priority 2 never overrides Priority 1, and Priority 3 never overrides
either.

---

## 4. Low-confidence handling

When the agent's internal confidence rubric (defined per-agent) is
low:

1. **Pick the appropriate tag on severity grounds** — do not
   downgrade.
2. **Append a maintainer ping** at the end of the comment body:
   ```
   cc @<maintainer1> @<maintainer2> — low-confidence finding, please confirm.
   ```
3. The maintainer-lookup procedure lives in
   `maintainer-lookup.skill.md`.

This applies per-finding (not per-comment) — multiple low-confidence
findings on the same PR each get their own maintainer ping at the
bottom of their respective comments.

---

## 5. Worked examples

### Example A: ground-reachable assert (must fix)

A new command handler calls `FW_ASSERT(arg < MAX_LIMIT)` on a
ground-controlled command argument. The agent traces `arg` to a
ground input via `fprime-ground-input-tracing.skill.md`.

- Introduced by this PR (new handler).
- In-scope for the security agent.
- Safety-critical — the assert is reachable by ground input, which
  could DoS the spacecraft.
- Concrete fix: replace assert with a validation + error response.

**Tag:** `**must fix**`. **Include a suggestion block** with the
proposed validation.

### Example B: unverified new GitHub Action (must fix)

A workflow added by the PR uses `uses: random-org/some-action@main`.
The agent cannot determine the trust level of the org. The action's
workflow `permissions:` includes `contents: write`.

- Introduced by this PR.
- In-scope for the supply-chain agent.
- The privileged surface makes this safety-critical for CI.
- Concrete fix: pin to a specific commit SHA (the agent provides one
  if it can fetch it; otherwise leaves a `(verify the SHA before
  applying)` annotation).

**Tag:** `**must fix**` with maintainer ping (low confidence on the
exact SHA), suggestion block included.

### Example C: redundant comment style (could fix)

A new function has both a brief comment and an inline reiteration of
the function name above its definition. Stylistic, no functional
impact.

- Introduced by this PR.
- Non-blocking.
- Minor.

**Tag:** `**could fix**`. Suggestion block optional (probably
attached, since the fix is trivial).

### Example D: preexisting latent issue (future work)

The PR refactors a function but does not touch a separate assert that
the agent thinks is reachable by ground input. The assert was there
before the PR and the PR did not add a new caller.

- Preexisting.
- The PR neither introduced nor widened it.

**Tag:** `**future work**`. No suggestion block. Mention in the
agent's summary `future work` column count.

### Example E: in-scope but no concrete fix, severity-worthy (must fix)

The PR's new code path leaks resources on an error path. The agent
sees the leak but the correct fix depends on what the surrounding
component's lifecycle contract is, which the agent can't infer.

- Introduced by this PR.
- Severity-worthy (resource leak on a flight code path).
- No concrete fix expressible.

**Tag:** `**must fix**` with maintainer ping. NOT `**could fix**`
(severity drives the tag).

---

## 6. One-line summary

`introduced ↦ severity-driven (must fix / suggestion / could fix);
preexisting ↦ future work; no defaulting; suggestion blocks are
orthogonal to tags; low confidence → ping the maintainer at the
tagged severity, never below.`
