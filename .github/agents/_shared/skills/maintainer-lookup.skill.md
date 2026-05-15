---
name: maintainer-lookup
description: Use when an agent needs to ping the right maintainer for a finding (low-confidence finding, improper resolution, disagreement escalation, or recommend-close).
---

# Skill: Maintainer lookup (whom to ping)

Every F Prime review agent that needs to escalate a finding —
because it is low-confidence (review contract §4), because a thread
was improperly resolved (review contract §7), because the contributor
and agent disagree (review contract §11), or because the aggregator
is recommending close (review-summary §5e) — uses this skill to
resolve the right GitHub handles to `cc`.

This skill returns a list of handles. Callers format the ping line
themselves (the wording varies by trigger).

---

## 1. Four-step resolution

Apply in order; stop at the first step that yields at least one
handle. Always include the fallback as a safety net unless the
earlier step explicitly provided a complete maintainer set.

### Step 1 — `README.md` "Core Maintainer(s)" table

Parse the F Prime repository `README.md` for a markdown table whose
header begins with `| Product` and contains a `Core Maintainer(s)`
column. For the `fprime` repository, the relevant row is:

```
| F Prime | @LeStarch, @thomas-bc |
```

The skill returns `[@LeStarch, @thomas-bc]`.

The exact section currently lives near the "Meet the Team" heading
in `README.md`. The skill MUST re-parse `README.md` at runtime
rather than hard-coding the handles, so updates to the maintainer
list are picked up automatically.

For non-`fprime` products that may use this contract in the future
(e.g., `fprime-tools`, `fprime-gds`), the same parser maps the repo
name to its row.

### Step 2 — Security Overseer (security agent only)

The security agent additionally consults the "Role / Team Member"
table at the top of `README.md` and adds the `Security Overseer`
entry to the maintainer list. For `fprime` this is `@bitWarrior`.

Non-security agents skip this step.

### Step 3 — `git log` recent approvers

```bash
git log --merges --first-parent -n 20 \
  --pretty=format:'%an <%ae> %s' \
  -- <touched-file-path>
```

Extract approver handles from merge-commit messages (the `Approved-
by:` trailers, or the `Merged via PR` GitHub convention). Limit to
distinct contributors who have approved within the last 90 days.
Add these handles to the list.

This step is best-effort; when grepping the log produces no clear
handles, skip.

### Step 4 — Fallback

If none of steps 1–3 yielded a single handle, return the project
fallback set:

```
[@LeStarch, @thomas-bc]
```

This guarantees that every low-confidence / improperly-resolved /
disagreement ping has at least one recipient.

---

## 2. Per-trigger conventions

Callers format the `cc` line according to the trigger:

| Trigger | Line shape |
|---|---|
| Low-confidence finding (review contract §4) | `cc @<m1> @<m2> — low-confidence finding, please confirm.` |
| Improper resolution (review contract §7) | `cc @<m1> @<m2> — contributor resolved without addressing; please adjudicate.` |
| Disagreement escalation (review contract §11) | `cc @<m1> @<m2> — needs human adjudication.` |
| Recommend close (aggregator §5e) | `cc @<m1> @<m2> — please confirm close.` |

---

## 3. De-duplication

If the same maintainer is returned by multiple steps (e.g., README
and git-log both list `@LeStarch`), include them once.

Order the handles by the order steps yielded them (README first,
then Security Overseer where applicable, then git-log).

---

## 4. Worked example — security finding on `Svc/CmdDispatcher`

1. Step 1: `README.md` → `@LeStarch, @thomas-bc`.
2. Step 2: Security Overseer → `@bitWarrior`.
3. Step 3: `git log -- Svc/CmdDispatcher/CmdDispatcher.cpp` →
   suppose the recent merges are by `@thomas-bc` (already in list)
   and `@SterlingPeet`. Add `@SterlingPeet`.
4. Returned list: `[@LeStarch, @thomas-bc, @bitWarrior,
   @SterlingPeet]`.

Caller formats:

```
cc @LeStarch @thomas-bc @bitWarrior @SterlingPeet — low-confidence finding, please confirm.
```

For brevity, the security agent may cap the ping at the first 3
handles in the returned list when the line would otherwise wrap on
narrow screens; the README + Security Overseer entries are always
preserved.

---

## 5. Worked example — supply-chain finding on a workflow

1. Step 1: `README.md` → `@LeStarch, @thomas-bc`.
2. Step 2: Supply-chain is not the security agent → skip.
3. Step 3: `git log -- .github/workflows/cmake-test.yml` → recent
   approvers are `@thomas-bc` (already in list) and `@kevin-f-ortega`.
4. Returned list: `[@LeStarch, @thomas-bc, @kevin-f-ortega]`.

---

## 6. One-line summary

`README "Core Maintainer(s)" first → +Security Overseer for security
agent → recent merge approvers from git log →
fallback @LeStarch, @thomas-bc. De-duplicate, preserve order.`
