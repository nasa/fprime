---
name: pr-diff-scoping
description: Use when determining whether a finding was introduced by the current PR (must-fix candidate) or is preexisting and unrelated (future-work candidate).
---

# Skill: Decide whether a finding is "introduced by this PR"

Every reviewer agent must classify each finding as either:

- **Introduced by this PR** (line added or modified, OR a new caller
  added by the PR widens reachability) → eligible for `**must fix**` /
  `**suggestion**` / `**could fix**` per `triage-classifier.skill.md`.
- **Preexisting** (the PR did not introduce or widen the offending
  behavior) → must be tagged `**future work**`, never blocking.

This skill defines the test.

---

## 1. Three rules

Apply in order:

1. **The offending line is added or modified in the PR diff.**
   Inspect the PR's unified diff (`git diff <base>...<head> -- <path>`
   or `GET /repos/{o}/{r}/pulls/{n}/files`). If the line appears with
   a `+` marker in any hunk on that file, the finding is
   **introduced**.

2. **The offending line is unchanged, but a new caller added by the
   PR widens its reach.** Check whether any file in the PR diff
   contains a `+`-marked line that calls (or otherwise reaches) the
   enclosing symbol of the offending line. If so, the finding is
   **introduced** (even though the line itself is untouched). The
   PR widened the set of execution paths that hit the line.

3. **The offending line is unchanged AND no new caller exists.**
   The finding is **preexisting** → `**future work**`.

Rules 1 and 2 are sufficient conditions for "introduced". Rule 3 is
the negative case.

---

## 2. What counts as a "new caller"?

A new caller exists when ANY of the following is true on the PR diff:

- A `+` line in another file contains a direct call site of the
  enclosing symbol (function call, port invocation, command
  dispatch, port connection in a topology).
- A `+` line modifies a topology file (`topology.fpp`,
  `instances.fpp`, `*Topology.cpp`) to wire a port to a component /
  handler whose body contains the offending line.
- A `+` line registers a new command dispatch, parameter handler,
  event handler, or telemetry channel that reaches the offending
  line.
- A `+` line introduces a new entry point (main, test harness, CLI)
  that exercises the offending line.

Reaching the offending line transitively through multiple
intermediate callers counts. The transitive reach is bounded by what
the agent can statically determine; if it is unclear, the agent
treats this as **introduced** and additionally appends a
maintainer-ping per `maintainer-lookup.skill.md`.

When in doubt, prefer **introduced**. The cost of a false positive
(flagging a preexisting issue as introduced) is small — the
contributor pushes back, the agent escalates to a maintainer per the
disagreement-handling flow in the review contract §11. The cost of a
false negative (missing a regression because the agent classified it
as future work) is much larger.

---

## 3. Untouched files

If the offending line lives in a file not in the PR diff:

- It can still be **introduced** under rule 2 (new caller from
  another file).
- Otherwise it is **preexisting**.

The agent does not flag completely-untouched, never-newly-reached
behavior unless its scope explicitly covers latent issues (most
agents do not).

---

## 4. Worked examples

### Example A — line added, must fix candidate

```diff
+ FW_ASSERT(cmdArg < CMD_MAX);
```

`FW_ASSERT` on a ground-controlled command argument. The line is `+`
in the diff. **Introduced.** Severity decides the tag.

### Example B — handler unchanged, new caller in topology

The function `Foo::handleCmd_DoTheThing()` was already in the
repository before this PR. The handler body unchanged. The PR adds:

```diff
+ topology.connect(cmdDispatcher, Foo::handleCmd_DoTheThing);
```

Before this PR, `handleCmd_DoTheThing` was unreachable from
ground-controlled commands. After this PR, it is reachable. Even
though the offending body is unchanged, the PR widened its reach.
**Introduced.**

### Example C — preexisting assert, unrelated PR

The PR refactors a different component. The agent walks the codebase
and finds a ground-reachable assert in a third component. The
offending line is unchanged and no caller is added by the PR.
**Preexisting.** Tag `**future work**`.

### Example D — modification, not addition

```diff
- if (len < SAFE) { copy(dst, src, len); }
+ if (len < UNSAFE) { copy(dst, src, len); }
```

The line was modified, weakening the bound. **Introduced** by
modification. Tag based on severity (likely `**must fix**`).

### Example E — refactor that exposes preexisting issue

The PR refactors a private function into a public one. The body is
unchanged, but the public interface is new. If the function is now
reachable from outside the original module, that is a new caller —
**introduced**.

---

## 5. Programmatic checks

```bash
# Files changed in the PR
gh pr diff <PR_NUMBER> --name-only --no-color

# Diff for a specific file
gh pr diff <PR_NUMBER> -- <path>

# Resolve enclosing symbol for a line (best-effort via ctags or
# language-specific tooling; agents may use their own analysis)
```

The agent does NOT need a perfect static-analysis pipeline; it needs
the unified diff, the head and base SHAs, and the ability to grep
for symbol references. F Prime's repository layout (topology files
in known locations) makes most "new caller" cases tractable with
grep.

---

## 6. One-line summary

`added/modified line ⇒ introduced; new caller in diff that widens
reach to the offending line ⇒ introduced; otherwise preexisting →
future work. When in doubt, prefer introduced and ping a maintainer.`
