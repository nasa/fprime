# Skill: Deterministic Repository Partitioning into Review Units

Used by the full-codebase review (`full-codebase-review.agent.md`,
batch contract `_shared/review-contract-batch.md`) to split a
repository into an exhaustive, deterministic set of **review units**
recorded in `review-units.yaml`. The manifest is the anti-sampling
guarantee: every tracked file is assigned to exactly one unit, and
the run is not complete until every unit is reviewed.

---

## 1. Inputs

- The repository working tree at the exact commit SHA under review.
- The file universe: `git ls-files` at that SHA (tracked files only;
  submodule pointers are listed as single entries, their contents
  are out of scope for this repo's review).

## 2. Partitioning rules (applied in order)

1. **Exclusions.** Files matching these patterns are assigned to the
   reserved unit `excluded` (listed in the manifest for coverage
   accounting, but not reviewed): binary assets (images, fonts,
   archives), lockfiles, and generated files committed to the tree
   that a `# generated` header or the build system identifies as
   autocoder output. Everything else is reviewable — including
   docs, CI workflows, cmake, and scripts.
2. **Module seeding.** Seed one unit per module directory: for
   F Prime, each direct child of `Fw/`, `Os/`, `Svc/`, `Drv/`,
   `Utils/` and similar top-level component trees (e.g.
   `Fw/Buffer`, `Svc/CmdDispatcher`, `Os/Posix`); plus dedicated
   units for `cmake/`, `.github/` (workflows, actions, agent specs),
   `docs/` subtrees, and top-level loose files (`misc-root`).
   A module's `docs/` and `test/` subdirectories stay WITH the
   module — reviewers assess code and its tests/docs together.
3. **Size capping.** Target ≤ 2,500 reviewable LOC per unit
   (`wc -l` over the unit's text files). A module exceeding the cap
   is split along its subdirectory boundaries (e.g.
   `Svc/FileDownlink--test` split from `Svc/FileDownlink`), then, if
   a single flat directory still exceeds the cap, by lexicographic
   file ranges (`<module>--files-aa`, `--files-ab`, ...). Never
   split a single file.
4. **Coalescing.** Adjacent sibling modules each under 500 LOC may
   be coalesced into one unit (e.g. `Fw/Ports/*` port definitions)
   to avoid hundreds of trivial units; the coalesced unit keeps the
   parent path as its id (`Fw/Ports`).
5. **Unit ids.** The unit id is the module path with `/` replaced by
   `--` (e.g. `Svc--CmdDispatcher`), plus the split suffix when
   rule 3 applied. Ids are stable across runs at the same SHA and
   change only when the tree structure changes.

## 3. Manifest shape (`review-units.yaml`)

```yaml
repo: <owner/repo>
commit: <sha>
generated_by: repo-partitioning.skill.md v1
units:
  <unit-id>:
    loc: <N>
    files:
      - <path>       # sorted lexicographically
  excluded:
    files:
      - <path>
```

Unit ids sorted lexicographically; file lists sorted; no timestamps.

## 4. Mandatory coverage check

After generating the manifest, verify and record in the manifest
header comment:

```
# coverage-check: <total tracked files> tracked = <sum of unit files> assigned + <excluded count> excluded; disjoint: yes
```

- Every `git ls-files` entry appears in exactly one unit (or
  `excluded`).
- No file appears twice.

If either check fails, the manifest is invalid and MUST be
regenerated — the review MUST NOT start from an invalid manifest.

## 5. Re-partitioning on a new SHA

A re-review at a new commit regenerates the manifest at that SHA in
place. Because ids derive from stable module paths, most units keep
their ids and `git diff` on the results repo shows precisely which
units gained/lost files and which findings changed. The ledger is
reset to `pending` ONLY for units whose file list or file contents
changed between the two SHAs (`git diff --name-only <old> <new>`
mapped through the manifest); unchanged units keep `completed`
status with their existing artifacts.
