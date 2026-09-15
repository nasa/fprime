#!/usr/bin/env python3
"""Turn an AI session context into a markdown summary: one file per session, with a table of PRs produced.

  session_summary.py path  --context <id|url> [--date YYYY-MM-DD]
  session_summary.py write --context <id|url> [--author NAME] [--outcome success|partial|failed]
                           [--prompt TEXT] [--meta key=value ...] [--from-report PATH]
                           [--pr "URL [| title [| state]]" ...] [--text TEXT | --file PATH | stdin]

The body (stdin/--file/--text) is the agent-written narrative; this script adds the front matter,
title, and PR table. `--from-report` copies date/session/user/meta from an ai-session-report file.
Summaries live in $AI_SESSION_SUMMARY_DIR (default ~/ai-session-summaries) as <YYYY-MM-DD>-<id>.md.
Where the summary is sent afterwards is not this script's concern.
"""

import argparse
import datetime as dt
import os
import re
import sys

SUMMARY_DIR = os.environ.get(
    "AI_SESSION_SUMMARY_DIR", os.path.expanduser("~/ai-session-summaries")
)
PR_URL = re.compile(
    r"https?://[^/\s]+/(?P<repo>[^/\s]+/[^/\s]+)/(?:pull|merge_requests|pull-requests)/(?P<num>\d+)"
)


def context_slug(context):
    context = context.strip().rstrip("/").rsplit("/", 1)[-1]
    context = re.sub(r"^devin-", "", context)
    return re.sub(r"[^A-Za-z0-9_.-]", "-", context)


def summary_path(context, day):
    return os.path.join(SUMMARY_DIR, f"{day:%Y-%m-%d}-{context_slug(context)}.md")


def read_report_header(path):
    meta = {}
    with open(path, encoding="utf-8") as fh:
        lines = fh.read().split("\n")
    if not lines or lines[0].strip() != "---":
        return meta
    for line in lines[1:]:
        if line.strip() == "---":
            break
        key, sep, value = line.partition(":")
        if sep:
            meta[key.strip()] = value.strip()
    return meta


def parse_pr(spec):
    parts = [part.strip() for part in spec.split("|")]
    url = parts[0]
    match = PR_URL.search(url)
    repo = match.group("repo") if match else ""
    label = f"#{match.group('num')}" if match else url
    title = parts[1] if len(parts) > 1 and parts[1] else ""
    state = parts[2] if len(parts) > 2 and parts[2] else ""
    return repo, f"[{label}]({url})", title, state


def pr_table(specs):
    rows = ["| Repository | PR | Title | State |", "|---|---|---|---|"]
    if not specs:
        rows.append("| — | — | No pull requests produced | — |")
    for spec in specs:
        repo, link, title, state = parse_pr(spec)
        rows.append(f"| {repo} | {link} | {title} | {state} |")
    return "\n".join(rows) + "\n"


def header(context, day, meta):
    lines = ["---", f"date: {day:%Y-%m-%d}", f"session: {context}"]
    lines += [f"{key}: {value}" for key, value in meta.items()]
    lines += ["---", "", f"# Session summary {context_slug(context)}", ""]
    return "\n".join(lines) + "\n"


def read_text(args):
    if args.text is not None:
        return args.text
    if args.file:
        with open(args.file, encoding="utf-8") as fh:
            return fh.read()
    return sys.stdin.read()


def parse_day(value):
    if value:
        return dt.date.fromisoformat(value)
    return dt.datetime.now(dt.timezone.utc).date()


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    sub = parser.add_subparsers(dest="command", required=True)
    for name in ("path", "write"):
        cmd = sub.add_parser(name)
        cmd.add_argument("--context", required=True, help="Session id or URL")
        cmd.add_argument(
            "--date", help="Session date (default: from report or today UTC)"
        )
        if name == "write":
            cmd.add_argument("--author", help="Requesting user")
            cmd.add_argument("--outcome", choices=("success", "partial", "failed"))
            cmd.add_argument(
                "--prompt", help="One-line description of the initial request"
            )
            cmd.add_argument("--meta", action="append", default=[], metavar="KEY=VALUE")
            cmd.add_argument(
                "--from-report", help="ai-session-report file to copy metadata from"
            )
            cmd.add_argument(
                "--pr",
                action="append",
                default=[],
                metavar="SPEC",
                help='"URL [| title [| state]]"',
            )
            cmd.add_argument("--text")
            cmd.add_argument("--file")
    args = parser.parse_args()

    meta = {}
    if args.command == "write" and args.from_report:
        meta = read_report_header(args.from_report)
    day = parse_day(args.date or meta.pop("date", None))
    meta.pop("session", None)
    path = summary_path(args.context, day)
    if args.command == "path":
        print(path)
        return 0

    if args.author:
        meta["user"] = args.author
    for pair in args.meta:
        key, sep, value = pair.partition("=")
        if not sep:
            parser.error(f"--meta expects key=value, got {pair!r}")
        meta[key.strip()] = value.strip()
    if args.prompt:
        meta["prompt"] = args.prompt
    if args.outcome:
        meta["outcome"] = args.outcome

    body = read_text(args).strip()
    os.makedirs(SUMMARY_DIR, exist_ok=True)
    with open(path, "w", encoding="utf-8") as fh:
        fh.write(header(args.context, day, meta))
        fh.write("## Pull requests\n\n" + pr_table(args.pr) + "\n")
        fh.write(body + "\n")
    print(path)
    return 0


if __name__ == "__main__":
    sys.exit(main())
