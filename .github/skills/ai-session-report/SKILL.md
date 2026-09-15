---
name: ai-session-report
description: Use when an AI session (a Devin session or similar agent conversation) must be recorded as a report — live, exchange by exchange, or by backfilling from the session's event history. Produces a markdown report of the prompts and responses; where the report is delivered is decided by the caller.
---

# Skill: AI session report (context → report)

Turns a session **context** — the ordered user prompts and agent
responses of one AI session — into a markdown **report** on local disk.
This skill covers only that transformation. It does not know or care
where the report goes afterwards; the destination, credentials, and the
policy for *when* to report are supplied by the caller (a knowledge
note, a playbook, or the user).

`session_report.py` beside this file does the work. Python 3 stdlib only.

---

## 1. Report format

One report per session, at `$AI_SESSION_REPORT_DIR/<YYYY-MM-DD>-<id>.md`
(default directory `~/ai-session-reports`; `<id>` is the session id
without any `devin-` prefix, derived from an id or a session URL).

```
---
date: 2026-09-15
session: devin-<id>
user: <username>
<key>: <value>          # any --meta pairs given on first write
---

# Session <id>

## [2026-09-15T18:20:00Z] prompt — <username>

<verbatim user message>

## [2026-09-15T18:21:30Z] response

<verbatim agent message>
```

Roles are `prompt` (user → agent), `response` (agent → user, i.e. each
`message_user`), and `note` (anything else worth keeping, e.g. a
redaction marker). Text is verbatim; the only permitted edit is
replacing a secret value with `[REDACTED]`.

---

## 2. Live reporting (one exchange at a time)

```bash
S=.github/skills/ai-session-report/session_report.py

# each incoming user message
python3 $S append --context "$SESSION_ID" --role prompt --author "$USERNAME" \
  --meta url="$SESSION_URL" --meta repo="$OWNER_REPO" <<'EOF'
<user message>
EOF

# each outgoing response (same tool batch as the message_user call)
python3 $S append --context "$SESSION_ID" --role response <<'EOF'
<exact text passed to message_user>
EOF

python3 $S path --context "$SESSION_ID"    # where the report is
```

`--meta` pairs are written only when the report is first created; it is
harmless to pass them every time. Append calls print the report path so
the caller can hand the file to its delivery mechanism.

---

## 3. Backfilling a whole session

When a session was not reported live, rebuild the report from its event
history:

1. List message events for the session (for Devin: `devin_mcp` →
   `devin_session_events` with `{"action": "list", "session_id":
   "devin-<id>", "categories": ["message"], "first": 100}`, paging with
   `after`), then fetch full text with `{"action": "details",
   "event_ids": [...]}` (≤ 20 ids per call).
2. For each event in chronological order run `append` with
   `--timestamp <event time>` and `--date <session start date>`;
   `incoming` events are `--role prompt --author <username>`,
   `outgoing` events are `--role response`.
3. If a report already exists for the session, read its last `## [...]`
   timestamp and append only newer events.

---

## 4. Rules

- Verbatim. Never summarize, reword, or drop exchanges.
- Never write secret values into a report.
- One writer per session; sequence append calls, do not parallelize them.
- Reporting is best-effort: a failure to append must not block the
  session's actual task.
