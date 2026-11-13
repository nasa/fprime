---
name: fprime-system-allocation
description: >-
  Map a system decomposition onto inherited F Prime components (nasa/fprime,
  fprime-community, project libraries) and new components, inheriting
  without change wherever possible; flags every inherited component that
  would have to change as a red-flag item the human approves explicitly,
  and writes the SSDD Inventory the project campaign and the RDD read.
  Use after fprime-system-decomposition and before the base deployment is
  created. Keywords: F Prime, inheritance, reuse, inventory, inherited
  component, new component, modified component, fprime-community, SSDD,
  project campaign.
---

# Skill: F Prime system allocation (inherit or build)

Takes the element list and allocation table of `fprime-system-decomposition`
and decides, element by element, whether the deployment inherits an
existing component unchanged, must change one (red flag), or builds a new
one. Output is the SSDD `## Inventory` section — the partition the project
campaign runs from. Reviewed by `system-allocation-review.agent.md` under
`_shared/phase-review-loop.md`. Doctrine: **inherit without change as much
as possible**; a change to an inherited component is a project risk the
human must see and approve by name.

## 1. Inputs

- SSDD `## Architecture` and `## Allocation` (element list, allocation
  table) and the approved `## Requirements`.
- The catalogs, read-only, at a named ref each: `nasa/fprime` (`Svc/`,
  `Drv/`, `Svc/Subtopologies/`), the `fprime-community` repositories the
  project may use (name each one; e.g. `fprime-community/fprime-yamcs`,
  reference deployments for the target platform), the project's libraries.
  For each candidate read its `.fpp` (`fprime-fpp-navigation`) and
  `docs/sdd.md`; code last, only to confirm a behaviour the SDD omits.
- The platform and ground system from the bench facts.

## 2. Classes

| Class | Meaning | What the campaign does with it |
|---|---|---|
| `INHERITED` | Used as published at `repo@ref`; configured through parameters, `settings.ini`, topology or a project `config/` override only | Wired in the base deployment; evidence = its upstream SDD, checklist and tests at that ref; qualified by the system tests that touch it |
| `INHERITED-MODIFIED` | Needs a change to its source (behaviour, interface, bug fix) — **red flag** | Explicitly listed in the SSDD with justification; approved by the human at the system design gate by name; change made through `fprime-maintenance` with an upstream issue, and the component re-qualified on the standalone stage-2 path (contract §1 row 2) |
| `NEW` | No catalog component provides the function | One `fprime-development-campaign` per component, test configuration = the base deployment |

A *project adapter* — a new thin component that wraps an inherited one so
the inherited one stays unchanged — is `NEW` and is the preferred answer
when an inherited component is almost right (AL-3).

## 3. Procedure

1. **Search before deciding.** For every element list the candidates
   found (`repo@ref`, path, one line on fit). "No candidate" needs the
   searched locations named.
2. **Try to inherit unchanged.** For each candidate check in order: (a)
   fits as published; (b) fits with configuration (parameters, FPP
   constants, `config/` override, topology wiring); (c) fits behind a
   project adapter; (d) fits only with a source change; (e) no fit. (a)–(b)
   → `INHERITED`; (c) → `INHERITED` + a `NEW` adapter; (d) →
   `INHERITED-MODIFIED` with the change described in one line and the
   reason (b) and (c) fail; (e) → `NEW`.
3. **Pin.** Every inherited element gets `repo@ref` (tag or sha; a branch
   name is not a pin) — the same refs the base deployment and the RfR
   manifests will carry (`repos[]`, contract §3).
4. **Collect inherited evidence.** For each `INHERITED` element: its
   upstream `docs/sdd.md` path, whether a component checklist exists
   upstream (`fprime-component-checklist-review` evidence), its unit-test and
   `test/int/` presence. Missing evidence is recorded, not fixed, and
   becomes a system-test obligation (the allocation row's verification moves
   to system test).
5. **Write SSDD `## Inventory`**, one row per element:
   `element | class | origin repo@ref (or "new: <repo>") | configuration / adapter / change (one line) | upstream SDD | upstream checklist | campaign id (NEW only) | allocated REQ ids`.
   Then a `### Red flags` list: every `INHERITED-MODIFIED` row with the
   change, why (b) and (c) failed, the upstream issue to be filed, and the
   re-qualification path. An empty list says `None`.
6. **Hand off.** The `NEW` rows (with campaign ids `<element>` ≤ 8
   characters, used as `campaign-id` in contract §9) and the pinned
   `INHERITED` rows go to the base-deployment lane of
   `fprime-project-campaign`; red flags go to the human gate as its first
   item.

## 4. Rules (reviewer's checklist, `system-allocation-review`)

- **AL-1 Searched, not assumed.** Every `NEW` row names the catalogs
  searched and the nearest candidate rejected; a `NEW` that duplicates a
  catalog component's function is a must-fix.
- **AL-2 Pinned.** Every inherited row carries `repo@tag-or-sha`;
  a branch name or "latest" is a must-fix.
- **AL-3 Adapter before modification.** An `INHERITED-MODIFIED` row shows
  configuration (b) and adapter (c) were tried and why each fails; a
  modification whose adapter alternative is not ruled out is a must-fix.
- **AL-4 Red flags explicit.** Every `INHERITED-MODIFIED` row appears in
  `### Red flags` with change, reason, upstream issue and re-qualification
  path; the gate message lists them first.
- **AL-5 Evidence recorded.** Every `INHERITED` row states SDD and
  checklist presence; a missing item moves the allocated requirement's
  verification to a system test (the allocation table is updated, not the
  requirement).
- **AL-6 Allocation preserved.** Every REQ id of the allocation table
  appears on exactly the inventory rows that implement its target; a REQ
  whose target became two elements (adapter + inherited) lists both.
- **AL-7 Platform fit.** Each inherited element is known to build for the
  target platform (`Os` implementation, toolchain, memory) or the row says
  `platform: unverified` and the base-deployment lane verifies it first.

## Anti-patterns

- Choosing `NEW` because the inherited SDD is thin — thin documentation is
  an evidence gap (AL-5), not a reason to rewrite.
- Modifying an inherited component "while we are there"; the change is
  the one line the red flag names, through `fprime-maintenance`.
- Pinning to a fork or branch that will move before the RfR.
- Hiding a modification inside a project `config/` override that changes
  behaviour the upstream SDD specifies — that is a modification.
