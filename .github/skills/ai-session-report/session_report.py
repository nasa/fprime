#!/usr/bin/env python3
"""Turn an AI session context into a markdown report: one file per session, one section per exchange.

  session_report.py path   --context <id|url> [--date YYYY-MM-DD]
  session_report.py append --context <id|url> --role prompt|response|note [--author NAME]
                           [--meta key=value ...] [--timestamp ISO] [--text TEXT | --file PATH | stdin]

Reports live in $AI_SESSION_REPORT_DIR (default ~/ai-session-reports) as <YYYY-MM-DD>-<id>.md.
Where the report is sent afterwards is not this script's concern.
"""

import argparse
import datetime as dt
import os
import re
import sys

REPORT_DIR = os.environ.get(
    "AI_SESSION_REPORT_DIR", os.path.expanduser("~/ai-session-reports")
)


def context_slug(context):
    context = context.strip().rstrip("/").rsplit("/", 1)[-1]
    context = re.sub(r"^devin-", "", context)
    return re.sub(r"[^A-Za-z0-9_.-]", "-", context)


def report_path(context, day):
    return os.path.join(REPORT_DIR, f"{day:%Y-%m-%d}-{context_slug(context)}.md")


def header(context, day, author, meta):
    lines = ["---", f"date: {day:%Y-%m-%d}", f"session: {context}"]
    if author:
        lines.append(f"user: {author}")
    lines += [f"{key}: {value}" for key, value in meta.items()]
    lines += ["---", "", f"# Session {context_slug(context)}", ""]
    return "\n".join(lines) + "\n"


def entry(role, when, author, text):
    who = f" — {author}" if author else ""
    return f"## [{when:%Y-%m-%dT%H:%M:%SZ}] {role}{who}\n\n{text.rstrip()}\n\n"


def read_text(args):
    if args.text is not None:
        return args.text
    if args.file:
        with open(args.file, encoding="utf-8") as fh:
            return fh.read()
    return sys.stdin.read()


def parse_when(value):
    if not value:
        return dt.datetime.now(dt.timezone.utc)
    return dt.datetime.fromisoformat(value.replace("Z", "+00:00")).astimezone(
        dt.timezone.utc
    )


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("command", choices=["path", "append"])
    parser.add_argument(
        "--context", required=True, help="Session id or URL identifying the context"
    )
    parser.add_argument(
        "--role", choices=["prompt", "response", "note"], help="Required for append"
    )
    parser.add_argument(
        "--author", default="", help="Who produced the text (prompts: the user)"
    )
    parser.add_argument("--text")
    parser.add_argument("--file")
    parser.add_argument(
        "--meta",
        action="append",
        default=[],
        metavar="KEY=VALUE",
        help="Front-matter field written when the report is first created",
    )
    parser.add_argument(
        "--date", help="Session date used in the file name (default: entry date)"
    )
    parser.add_argument("--timestamp", help="Entry time, ISO 8601 (default: now, UTC)")
    args = parser.parse_args()

    when = parse_when(args.timestamp)
    day = dt.datetime.strptime(args.date, "%Y-%m-%d") if args.date else when
    path = report_path(args.context, day)

    if args.command == "path":
        print(path)
        return 0

    if not args.role:
        parser.error("append requires --role")
    text = read_text(args)
    if not text.strip():
        print("session_report: empty text, nothing appended", file=sys.stderr)
        return 0

    meta = dict(item.split("=", 1) for item in args.meta if "=" in item)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    fresh = not os.path.exists(path)
    with open(path, "a", encoding="utf-8") as fh:
        if fresh:
            fh.write(header(args.context, day, args.author, meta))
        fh.write(entry(args.role, when, args.author, text))
    print(path)
    return 0


if __name__ == "__main__":
    sys.exit(main())
