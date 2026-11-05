---
name: ai-session-summary
description: Use when an AI session (a Devin session or similar agent conversation) must be condensed into a summary — what was asked, what was delivered (with a table of the pull requests produced), what went wrong, and what was learned. Produces one markdown summary per session; where the summary is delivered is decided by the caller.
---

# Skill: AI session summary (context → summary)

Condenses a session **context** — the prompts and responses of one AI
session, or the report produced by `ai-session-report` — into a short
markdown **summary** on local disk. This skill covers only that
transformation. Destination, credentials, and the policy for *when* to
summarize are supplied by the caller (a knowledge note, a playbook, or
the user).

`session_summary.py` beside this file assembles the file: it writes the
front matter, title, and PR table, and takes the narrative body from
you. Python 3 stdlib only.

---

## 1. Summary format

One summary per session, at `$AI_SESSION_SUMMARY_DIR/<YYYY-MM-DD>-<id>.md`
(default directory `~/ai-session-summaries`; `<id>` is the session id
without any `devin-` prefix, derived from an id or a session URL).

```
---
date: 2026-09-15
session: devin-<id>
user: <username>
url: <session URL>
repo: <owner/repo worked on>
prompt: <one line: the initial request>
outcome: success | partial | failed
---

# Session summary <id>

## Pull requests

| Repository | PR | Title | State |
|---|---|---|---|
| nasa/fprime | [#5980](https://github.com/nasa/fprime/pull/5980) | Add ai-session-report skill | open |

## Request
## Result
## Notable events
## Lessons
```

The PR table lists every pull request **produced or updated** by the
session (not PRs merely mentioned). With none, the table carries a
single "No pull requests produced" row. The four body sections are
written by you, each a few sentences or bullets:

- **Request** — what was asked, including later course corrections.
- **Result** — what was delivered: PRs, commits, files, notes, artifacts.
- **Notable events** — blockers, mistakes, corrections, surprises.
- **Lessons** — anything worth remembering for future sessions; omit
  the section rather than pad it.

---

## 2. Writing the summary

Gather the context first — the session's `ai-session-report` file if
one exists, otherwise the event history (for Devin: `devin_mcp` →
`devin_session_events`, as described in `ai-session-report` §3) — and
the URL, title, and state of every PR the session produced. Then:

```bash
S=.github/skills/ai-session-summary/session_summary.py

python3 $S write --context "$SESSION_ID" --author "$USERNAME" \
  --from-report ~/ai-session-reports/2026-09-15-<id>.md \
  --meta url="$SESSION_URL" --meta repo="$OWNER_REPO" \
  --prompt "Add ai-session-report skill to F Prime" --outcome success \
  --pr "https://github.com/nasa/fprime/pull/5980 | Add ai-session-report skill | open" <<'EOF'
## Request
...

## Result
...

## Notable events
...

## Lessons
...
EOF

python3 $S path --context "$SESSION_ID"    # where the summary is
```

`--from-report` copies `date`, `user`, and any metadata from the report's
front matter so they need not be repeated; explicit flags override it.
Repeat `--pr` once per pull request; title and state are optional.
`write` replaces the whole file, so re-run it to refresh a summary as
the session progresses.

---

## 3. Rules

- Factual and condensed. Do not reproduce the exchanges; the report
  (or the session itself) is the record.
- Never write secret values into a summary.
- Report failures and reversals honestly — a summary that only lists
  successes is a defective summary.
- Summarizing is best-effort: a failure to write must not block the
  session's actual task.
