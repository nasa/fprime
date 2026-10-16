#!/usr/bin/env python3
"""Apply Bug Severity / Bug Likelihood ratings from a JSON file to project items.

Ratings file: {"<issue number>": {"severity": "High", "likelihood": "Uncommon", "rationale": "..."}, ...}
Items not yet in the project are added first. Dry-run unless --apply.

    set_risk_fields.py --owner nasa --project 21 --repo nasa/fprime ratings.json [--apply]
"""
import argparse
import json
import sys

import project_fields as pf

SEV, LIK = "Bug Severity", "Bug Likelihood"


def add_item(project_id, repo, number):
    owner, name = repo.split("/")
    q = "query($o:String!,$n:String!,$i:Int!){repository(owner:$o,name:$n){issue(number:$i){id}}}"
    iid = pf.gql(q, {"o": owner, "n": name, "i": number})["repository"]["issue"]["id"]
    m = "mutation($p:ID!,$c:ID!){addProjectV2ItemById(input:{projectId:$p,contentId:$c}){item{id}}}"
    return pf.gql(m, {"p": project_id, "c": iid})["addProjectV2ItemById"]["item"]["id"]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--owner", required=True)
    ap.add_argument("--project", type=int, required=True)
    ap.add_argument("--repo", required=True)
    ap.add_argument("ratings")
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()

    ratings = {int(k): v for k, v in json.load(open(a.ratings)).items()}
    proj = pf.project(a.owner, a.project)
    f = proj["fields"]
    for n, r in ratings.items():
        if r["severity"] not in f[SEV]["options"] or r["likelihood"] not in f[LIK]["options"]:
            sys.exit(f"#{n}: bad option {r}")

    existing = {}
    for item_id, c, vals in pf.items(proj["id"], {SEV, LIK}):
        if c.get("number") and c["repository"]["nameWithOwner"] == a.repo:
            existing[c["number"]] = (item_id, vals)

    changes = 0
    for n in sorted(ratings):
        r = ratings[n]
        if n in existing:
            item_id, vals = existing[n]
        else:
            print(f"add   #{n} to project")
            item_id, vals = (add_item(proj["id"], a.repo, n) if a.apply else None), {}
        for field, key in ((SEV, "severity"), (LIK, "likelihood")):
            if vals.get(field) != r[key]:
                print(f"{'set ' if a.apply else 'would'} #{n}: {field}={r[key]} (was {vals.get(field)})")
                if a.apply:
                    pf.set_option(proj["id"], item_id, f[field], r[key])
                changes += 1
    print(f"{'applied' if a.apply else 'dry-run'}: {changes} field changes over {len(ratings)} issues")


if __name__ == "__main__":
    main()
