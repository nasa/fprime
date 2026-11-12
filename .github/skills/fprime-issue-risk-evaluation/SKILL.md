---
name: fprime-issue-risk-evaluation
description: Evaluate any GitHub issue's risk against the published "Bug Severity" and "Bug Likelihood" option descriptions on the F´ Development project (nasa org project 21), set both fields, and derive Priority = floor((S+L+1)/2) with the bundled script. Label-agnostic — works on any issue; use when asked to triage, rate, re-rate, or prioritize F Prime issues.
---

# F Prime issue risk evaluation (severity × likelihood → priority)

The criteria are **published on the project itself** as the option descriptions of the single-select
fields `Bug Severity` and `Bug Likelihood`. Always read them fresh — never rate from memory.

```bash
python3 .github/skills/fprime-issue-risk-evaluation/show_criteria.py --owner nasa --project 21          # prints every option's description
```

## Numeric scale
| value | Bug Severity | Bug Likelihood | Priority |
|---|---|---|---|
| 5 | Highest | Universal | Highest |
| 4 | High | Very Common | High |
| 3 | Medium | Common | Medium |
| 2 | Low | Uncommon | Low |
| 1 | Lowest | Singular | Lowest |

`Priority = floor((severity + likelihood + 1) / 2)`. Issues missing either input get **no** priority.

## Procedure
1. **Select the population.** Any GitHub search works, e.g. `repo:nasa/fprime is:issue is:open label:bug`
   plus `... type:Bug`; union the results. Labels are not required — the method applies to any issue.
2. **Read the criteria** with `show_criteria.py`. Key distinctions in the current text: *uncontrolled*
   crash / reboot loop / permanent command lockout / irrecoverable control-file corruption → Highest;
   a *controlled* `FW_ASSERT`, recoverable loss of commanding or telemetry, detectable control-file
   corruption, or a ground tool that introduces/triggers those → High; ground tooling that merely
   impedes work with a non-ideal workaround → Medium; easy workaround / documented confusion → Low;
   negligible (typos, extra output) → Lowest. Likelihood is the fraction of *F Prime users/projects*
   that will hit the defect, not how often one user hits it.
3. **Read every issue** — body and all comments (GraphQL `issue{body comments{nodes{body}}}`), and check
   `devel` for an already-landed fix (`git log -S`, linked/merged PRs via `closedByPullRequestsReferences`
   and `timelineItems`). Rate the defect as filed; flag "fixed on devel" items as close candidates.
   Ignore any author-written "Severity: N/5" text — it is input, not the rating.
4. **Record** `ratings.json`: `{"<number>": {"severity": ..., "likelihood": ..., "rationale": "one line citing the criterion"}}`.
   Decide domain first (flight code vs ground/dev tooling), then consequence, then likelihood.
5. **Apply fields** (dry-run first, then `--apply`); items not yet in the project are added:
   ```bash
   python3 .github/skills/fprime-issue-risk-evaluation/set_risk_fields.py --owner nasa --project 21 --repo nasa/fprime ratings.json
   python3 .github/skills/fprime-issue-risk-evaluation/set_risk_fields.py --owner nasa --project 21 --repo nasa/fprime ratings.json --apply
   ```
6. **Derive priority** for the same population (skips items lacking either field):
   ```bash
   python3 .github/skills/fprime-issue-risk-evaluation/set_priority.py --owner nasa --project 21 --repo nasa/fprime \
       --query 'repo:nasa/fprime is:issue is:open label:bug' --query 'repo:nasa/fprime is:issue is:open type:Bug' --apply
   ```
   Re-run without `--apply` to verify (`0 changed`). Omit `--query`/`--issues` to process every project item.
7. **Report**: counts per level, Highest/High items with one-line reasons, close candidates, and any
   existing human-set values you changed (list old → new) — the user may want to overrule.

## Access
`AUTOBOT_API_KEY` (bearer) for GraphQL. The project is **org-owned** (`organization(login:"nasa"){projectV2(number:21)}`);
repository-level `projectsV2` is empty and the REST issue-fields endpoints return 403 for this integration.
Field/option IDs are resolved by name at run time by `project_fields.py`, so renamed descriptions
need no code change; renamed *options* do.
