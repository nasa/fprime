---
name: fprime-system-decomposition
description: >-
  Decompose an F Prime project ask and its system requirements into the
  elements a deployment needs — components, subtopologies, component sets
  and whole-system properties — with a topology sketch and a
  requirement-to-element allocation table, written into the system SDD
  (SSDD). Use at the start of a project campaign, before any component
  exists, and whenever a system requirement changes. Keywords: F Prime,
  system design, SSDD, decomposition, allocation, topology sketch,
  subtopology, system requirement, project campaign.
---

# Skill: F Prime system decomposition

Turns the human's ask and the approved system requirements into the
architecture section of the SSDD: what elements the deployment is made
of and which element answers each requirement. It names *functions*
(command dispatch, UART link, FRAM manager), not repositories; the
inheritor (`fprime-system-allocation`) decides what is inherited and what
is new. Reviewed by `system-decomposition-review.agent.md` under
`_shared/phase-review-loop.md`.

## 1. Inputs

- The ask, quoted verbatim (the "Request + goals" of the project campaign).
- The approved system requirements table: `REQ-<System>-NNN` rows in the
  `fprime-component-requirements` Step 2 form, reviewed by
  `requirements-review.agent.md`. Decomposition does not edit it; a
  requirement the decomposition cannot place reopens requirements.
- Bench facts the human supplied: target platform, devices, links, ground
  system (contract §0 "complete testbed").
- The F Prime catalog read-only: `Svc/`, `Drv/`, `Svc/Subtopologies/`
  (`Svc/Subtopologies/*/docs/sdd.md`), the project's libraries.

## 2. Allocation kinds

A system requirement rarely maps to one component. Every requirement
gets exactly one *kind*; the kind fixes where it is verified.

| Kind | Meaning | Verified by |
|---|---|---|
| `component` | One component's observable behaviour satisfies it | The component campaign's tests (its SDD `## Requirements` carries a child `REQ-<Component>-NNN` that traces to this id) |
| `subtopology` | A standard or project subtopology satisfies it as wired (e.g. `CdhCore`, `ComCcsds`, `DataProducts`, `FileHandling`) | A system test on the subtopology's observables; component-level evidence is inherited |
| `set` | Two or more components must cooperate (a data path, a command → action → telemetry chain) | A system test spanning the set; each member's child requirement traces here |
| `system` | A property of the whole deployment (boot time, memory ceiling, rate-group margin, downlink budget, fault response of the whole) | A system test only; never a component |

A requirement that needs more than one kind is two requirements
(requirements reopen, never silent split here).

## 3. Procedure

1. **Quote the ask and list its elements** before reading the
   requirements (the same anti-anchoring rule as `requirements-review`
   RS-1); then list every requirement id. Both lists are the breadth check
   the reviewer reruns.
2. **Name the elements.** From the requirements, the bench facts and the
   catalog derive the element list: every subtopology the deployment uses,
   every component outside those subtopologies, every hardware boundary
   (driver per device/link) and every ground boundary. One line each:
   `element | kind of element (component / subtopology) | function (≤ 15 words) | hardware or ground boundary, if any`.
   Prefer a standard subtopology to a hand-picked set of its members;
   state why when you do not.
3. **Sketch the topology.** Instances of the elements, rate groups and
   who drives them, the command/event/telemetry spine, the comm stack to
   the ground system, hardware links to the bench devices. A block diagram
   (Mermaid or ASCII) plus an instance table `instance | element | rate group / queue | notes`;
   no base ids, priorities or stack sizes — `fprime-topology-development`
   fixes those in the base-deployment lane.
4. **Allocate.** One row per requirement:
   `REQ id | kind (§2) | target (element, subtopology, set members, or "system") | observable the verdict reads | verification level (component test / system test)`.
   Every `component`-kind row names exactly one element; every `set` row
   lists its members; every `system` row names the measurement.
5. **Fault and degraded modes.** For every fault scenario in the
   requirements (`requirements-review` RS-2) name the element that detects
   it and the element that responds; a scenario with no detector is a
   requirements reopen, not a note.
6. **Write the SSDD sections** (`docs/ssdd.md` in the deployment
   repository): `## Introduction` (the ask, platform, ground system — a few
   sentences), `## Requirements` (the approved table, unchanged — `fth`
   reads this heading, contract §3), `## Architecture` (element list,
   diagram, instance table), `## Allocation` (the table of step 4), then
   leave `## Inventory` to `fprime-system-allocation` and `## Tests` to
   `fprime-system-test-design`. Budgets as `fprime-development.agent.md` §6:
   one line per element, one row per requirement, no prose where a table
   serves.
7. **Hand off** the element list (step 2) and the allocation (step 4) to
   `fprime-system-allocation`; the system test plan takes every
   `subtopology`, `set` and `system` row plus the integration rows of
   `component` kinds.

## 4. Rules (reviewer's checklist, `system-decomposition-review`)

- **DC-1 Every requirement allocated once.** One row per `REQ-<System>-NNN`,
  one kind, one target; none missing, none twice.
- **DC-2 Right kind.** A row is `component` only if one element's own
  observables decide it; a chain of elements is `set`; a whole-deployment
  measurement is `system`. Over-allocating to `component` hides integration
  risk; over-allocating to `system` hides where the fix goes.
- **DC-3 Every element earns its place.** Each element is the target of
  at least one row, or a member of a subtopology/set that is. An element with
  no requirement is gold-plating or a missing requirement — ask.
- **DC-4 Standard before bespoke.** A standard subtopology or `Svc`/`Drv`
  component is preferred to a hand-rolled element of the same function; a
  deviation states the constraint (memory, platform, licence, missing
  feature) in one line.
- **DC-5 Boundaries named.** Every bench device, link and the ground
  system appears as a hardware or ground boundary of exactly one element.
- **DC-6 Faults detected and answered.** Every fault scenario has a
  detecting element and a responding element in the allocation.
- **DC-7 Observable verdicts.** Every row names an observable a system or
  component test can read (event, channel, data product, file, bench
  instrument); "by inspection" is allowed only with the requirement's
  verification column saying so.

## Anti-patterns

- Naming repositories or deciding reuse here (that is allocation).
- Fixing base ids, priorities, stack sizes or connections port-by-port
  (that is the base-deployment lane).
- Splitting or rewording a requirement to make it fit a kind.
- Diagrams with every port; the sketch shows elements and the spine.
- A `system`-kind row whose measurement nobody can take on the bench.
