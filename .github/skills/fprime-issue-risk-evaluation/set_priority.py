#!/usr/bin/env python3
"""Set the project Priority field from Bug Severity and Bug Likelihood.

    Priority = floor((severity + likelihood + 1) / 2), with Highest/Universal = 5 ... Lowest/Singular = 1.

Items missing either Bug Severity or Bug Likelihood are left untouched. Dry-run unless --apply.

    set_priority.py --owner nasa --project 21 [--query 'repo:nasa/fprime is:issue is:open label:bug'] \
                    [--query '... type:Bug'] [--repo nasa/fprime] [--issues 123 456] [--apply]
"""
import argparse
import sys

import project_fields as pf

SEV, LIK, PRI = "Bug Severity", "Bug Likelihood", "Priority"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--owner", required=True)
    ap.add_argument("--project", type=int, required=True)
    ap.add_argument("--repo", help="only items from this owner/repo")
    ap.add_argument("--query", action="append", default=[], help="GitHub issue search; union of all --query, restricts items")
    ap.add_argument("--issues", type=int, nargs="*", default=[], help="explicit issue numbers (with --repo)")
    ap.add_argument("--apply", action="store_true", help="write changes (default: dry run)")
    a = ap.parse_args()

    proj = pf.project(a.owner, a.project)
    f = proj["fields"]
    for name in (SEV, LIK, PRI):
        if name not in f:
            sys.exit(f"field {name!r} missing on project {proj['title']}")
    for name in pf.LEVELS:
        if name not in f[SEV]["options"] or name not in f[PRI]["options"]:
            sys.exit(f"option {name!r} missing on {SEV} or {PRI}")
    for name in pf.LIKELIHOOD:
        if name not in f[LIK]["options"]:
            sys.exit(f"option {name!r} missing on {LIK}")

    allowed = None
    if a.query:
        allowed = set()
        for q in a.query:
            allowed |= pf.search_issue_numbers(q)
    if a.issues:
        allowed = (allowed or set()) | {(a.repo, n) for n in a.issues}

    changed = skipped = kept = 0
    for item_id, c, vals in pf.items(proj["id"], {SEV, LIK, PRI}):
        if not c.get("number"):
            continue
        key = (c["repository"]["nameWithOwner"], c["number"])
        if a.repo and key[0] != a.repo:
            continue
        if allowed is not None and key not in allowed:
            continue
        s, l = vals.get(SEV), vals.get(LIK)
        if s not in f[SEV]["options"] or l not in f[LIK]["options"]:
            skipped += 1
            print(f"skip  #{c['number']}: severity={s} likelihood={l}")
            continue
        want = pf.LEVELS[(pf.LEVELS.index(s) + 1 + pf.LIKELIHOOD.index(l) + 1 + 1) // 2 - 1]
        cur = vals.get(PRI)
        if cur == want:
            kept += 1
            continue
        print(f"{'set ' if a.apply else 'would'} #{c['number']}: {s} + {l} -> {PRI}={want} (was {cur})")
        if a.apply:
            pf.set_option(proj["id"], item_id, f[PRI], want)
        changed += 1
    print(f"{'applied' if a.apply else 'dry-run'}: {changed} changed, {kept} already correct, {skipped} skipped (missing severity/likelihood)")


if __name__ == "__main__":
    main()
