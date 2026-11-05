# F Prime Multi-Agent Review — Batch (Full-Codebase) Contract Addendum

This file is the transport addendum to `review-contract.md` for
**batch mode**: reviewing the repository's existing contents directly
from the working tree, rather than a PR diff. It changes ONLY how
reviewers receive their input and where findings land. Reviewer
scopes, finding classes, triage taxonomy, the zero-trust principle,
and the low-confidence rubric are unchanged and continue to be
governed by `review-contract.md` and each agent file.

Where `review-contract.md` and this addendum conflict **in batch
mode**, this addendum wins. In PR mode this file does not apply.

---

## 1. Mode differences at a glance

| Concern | PR mode | Batch mode |
|---|---|---|
| Input | PR diff at head SHA | A **review unit**: explicit file list from the manifest |
| Read scope | Full files touched by diff | Every file in the unit, in full, plus any reachable context |
| "Introduced by this PR" test | `pr-diff-scoping.skill.md` | Not applicable — every finding is filed as-is |
| Output | Inline PR review comments | Finding records in a results-repo artifact file |
| Triage tags | must fix / suggestion / could fix / future work | must fix / suggestion / could fix / **advisory** (see §4) |
| Aggregation | PR review summary | Per-unit `unit-summary.md` + repo-wide roll-up |

Because there is no diff, **nothing is "preexisting"** — provenance
and severity are orthogonal, and batch mode has no provenance axis.
The PR-mode `**future work**` tag is therefore replaced in batch mode
by `**advisory**`: "in-scope, real, but low-priority" (see §4). All
other tags keep their PR-mode severity meaning.

---

## 2. Inputs to a batch reviewer pass

A batch reviewer pass is invoked with:

- `repo`: the repository under review (e.g. `nasa/fprime`) and the
  exact commit SHA being reviewed. All analysis is at that SHA.
- `unit`: one unit entry from `review-units.yaml` (see the
  `repo-partitioning.skill.md` skill): unit id + file list.
- `results_path`: the unit's artifact directory in the results repo
  (`<repo-short-name>/units/<unit-id>/`).

The reviewer MUST read **every file in the unit in full**. It MAY
read any other file in the repository for context (callers, headers,
FPP models, docs) but files outside the unit produce findings ONLY
when the defect manifests in a unit file. This keeps every finding
attributable to exactly one unit and prevents cross-unit duplicates.

---

## 3. Finding record format (diffable artifacts)

Each reviewer writes exactly one artifact file per unit:

```
<repo-short-name>/units/<unit-id>/<reviewer-short-name>.md
```

The artifact is regenerated in full on every run, and MUST be
deterministic given the same code and findings, so that `git diff`
between runs shows exactly the new, changed, and resolved findings.

### 3a. Artifact shape

```markdown
# <Reviewer display name> — <unit-id>

<!-- fprime-batch-agent: <reviewer-short-name> v1 -->
<!-- repo: <owner/repo> -->
<!-- commit: <sha> -->
<!-- unit: <unit-id> -->
<!-- counts: {"must_fix": N, "suggestion": N, "could_fix": N, "advisory": N} -->
<!-- verdict: Go | No-Go -->

## Findings

### <finding-id> `<finding-class>` **<tag>** — <file>:<line>

<≤ 6 lines of prose: what is wrong, why it matters, per the
reviewer's P3 priority. Optional fenced suggestion block with a
concrete fix.>
```

If a reviewer has no findings for a unit, the artifact contains the
header/metadata block and a single line `No findings.` — the file is
ALWAYS written, so an absent file unambiguously means "this reviewer
has not completed this unit".

### 3b. Ordering (mandatory, for diff stability)

Findings are sorted by: file path (lexicographic), then line number
(ascending), then finding-class, then finding-id. No timestamps, no
run counters, no wall-clock data anywhere in the artifact.

### 3c. Stable finding IDs

`finding-id` = first 12 hex chars of
`sha256("<reviewer-short-name>|<finding-class>|<file-path>|<anchor>")`
where `<anchor>` is the finding's single most-representative source
line, whitespace-normalized (collapse runs of whitespace to one
space, strip leading/trailing). Line numbers are deliberately
EXCLUDED from the hash so that unrelated edits above a finding do
not churn its ID; `git diff` then shows a stable ID with only its
`:<line>` locator moving. When the anchored line itself changes, the
old ID disappears and a new one appears — which is the correct diff
semantics ("the finding site changed").

### 3d. Verdict

`Go` iff `must_fix == 0` for the unit. Otherwise `No-Go`. Same
semantics as `review-contract.md` §2 but scoped to the unit.

---

## 4. Triage tags in batch mode

| Tag | Batch-mode meaning | Blocking? |
|---|---|---|
| `**must fix**` | Confirmed in-scope defect that would block a PR introducing it today. | Yes (unit No-Go) |
| `**suggestion**` | Non-blocking improvement with a concrete fix (fenced suggestion block required). | No |
| `**could fix**` | Minor in-scope issue worth fixing. | No |
| `**advisory**` | In-scope, real, low-priority (replaces PR-mode `future work`; there is no "preexisting" axis). | No |

Severity drives the tag exactly as in PR mode
(`triage-classifier.skill.md` applies; read "the PR introduces" as
"the code contains").

---

## 5. Cross-agent de-duplication (batch analogue of contract §6a)

Before writing its artifact, a reviewer MUST read the unit's already
present sibling artifacts (other reviewers' files in the same unit
directory). If another reviewer already filed a finding at the same
site-key (file + anchor line) covering the same underlying issue,
the later reviewer records a one-line **concurrence** instead of a
full duplicate:

```
### <finding-id> `concur:<other-reviewer>/<other-finding-id>` **<tag>** — <file>:<line>
Concurs with <other-reviewer> <other-finding-id>.
```

Concurrences count in the reviewer's own `counts` metadata.

---

## 6. Per-unit summary

After all reviewers complete a unit, the unit aggregator pass writes
`<repo-short-name>/units/<unit-id>/unit-summary.md`:

- The per-reviewer results table (reviewer, counts by tag, verdict,
  or `ERROR: <reason>` for a failed reviewer — no silent fallback,
  mirroring `review-summary.agent.md`).
- Unit verdict: `Go` iff every reviewer completed AND every
  reviewer's verdict is `Go`. A failed / missing reviewer forces the
  unit to `No-Go` — a unit is never marked complete with a reviewer
  silently skipped.
- The consolidated must-fix list for the unit (finding-id, file:line,
  one-line description), sorted per §3b.

---

## 7. The ledger (resumability product)

`<repo-short-name>/ledger.yaml` in the results repo is the single
source of truth for run state. Any fresh orchestrating session can
resume from it with no other context. Shape:

```yaml
repo: <owner/repo>
commit: <sha under review>
manifest: review-units.yaml   # sibling file, same directory
units:
  <unit-id>:
    status: pending | in-progress | completed | failed
    attempts: <N>
    reviewers_completed: [<short-name>, ...]
    note: <one line, only for failed>
```

Rules:

- Unit ids sorted lexicographically. No timestamps or session ids
  (keep diffs clean); operational metadata belongs in commit
  messages.
- `completed` requires: all registered batch reviewers' artifacts
  present with matching `commit` metadata, plus `unit-summary.md`.
- A unit is retried (fresh pass, `attempts` incremented) on failure.
  After 3 failed attempts it is left `failed` with a note; the run
  as a whole reports incomplete until every unit is `completed`.
- Every ledger update is a separate commit to the results repo, so
  interruption at any point loses at most the in-flight unit.

---

## 8. Repo-wide roll-up

The roll-up pass writes `<repo-short-name>/summary.md`. It is
regenerated **incrementally after every completed unit** (committed
alongside that unit's ledger update), so a usable whole-repo table
exists at any point mid-run; the final regeneration happens when
every unit is `completed` (or terminally `failed`). Because the
roll-up is a mechanical, reproducible function of the unit artifacts,
regenerating it never affects run state — the ledger alone gates
forward progress. Contents:

- Manifest coverage statement: every tracked file is in exactly one
  unit, and every unit is accounted for (mid-run regenerations list
  not-yet-reviewed units as `pending`).
- Overview table at the very top: repo verdict, repo-wide totals per
  triage tag, and counts of Go / No-Go units, followed by a
  per-reviewer totals table (rows = registered reviewers, columns =
  triage-tag totals plus count of units where that reviewer voted
  No-Go) and anchor links to the major sections.
- Table of contents: one row per unit, sorted by unit id, with the
  unit's per-tag totals (one column per triage tag) and its verdict.
  The unit name is a self-link (relative Markdown anchor) to that
  unit's section below.
- Per-unit sections, sorted by unit id: one table per unit mirroring
  the PR aggregator's layout — rows are the registered reviewers,
  columns are the triage-tag counts plus the reviewer's verdict
  (`ERROR: <reason>` rows for missing/failed reviewers, no silent
  fallback). Each reviewer name links (relative path) to that
  reviewer's artifact file; the section header links to the unit's
  `unit-summary.md`.
- The repo-wide consolidated must-fix table, sorted per §3b: one row
  per finding with the file:line, a short mechanical excerpt of the
  finding's first sentence, and a link to the detailed finding (the
  reviewer artifact's heading anchor). Traceability tokens (finding
  id, finding class) ride in an HTML comment inside the row —
  invisible when rendered, present in the raw text for diffing.
- Repo verdict: `Go` iff all units `Go`. Any failed unit or No-Go
  unit forces repo `No-Go`.

All links are relative anchors or in-repo relative paths, generated
deterministically, so the roll-up stays diffable across runs. The
roll-up contains no narrative not derivable from the artifacts —
it must be reproducible from the unit artifacts alone.
