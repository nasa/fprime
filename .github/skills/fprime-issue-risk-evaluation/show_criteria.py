#!/usr/bin/env python3
"""Print the published option descriptions for the risk fields of a project.

    show_criteria.py --owner nasa --project 21 [--field 'Bug Severity' --field 'Bug Likelihood' --field Priority]
"""
import argparse

import project_fields as pf


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--owner", required=True)
    ap.add_argument("--project", type=int, required=True)
    ap.add_argument("--field", action="append", default=None)
    a = ap.parse_args()
    proj = pf.project(a.owner, a.project)
    for name in a.field or ["Bug Severity", "Bug Likelihood", "Priority"]:
        f = proj["fields"].get(name)
        if not f:
            print(f"== {name}: (missing)")
            continue
        print(f"== {name}")
        for o in f["options"].values():
            print(f"--- {o['name']}\n{(o['description'] or '').strip()}\n")


if __name__ == "__main__":
    main()
