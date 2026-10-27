---
name: fprime-component-requirements
description: >-
  Requirements phase of F Prime component development. Guides the
  agent through eliciting, documenting, and confirming component
  requirements before any design or code is written. Trigger when
  starting a new component or when the user says to define
  requirements. Keywords: F Prime, requirements, SDD, behavior,
  interface, specification.
---

# Skill: F Prime Component Requirements

Requirements define **what** the component must do — its behaviors,
interfaces, and constraints. This phase produces a written requirements
list that drives all subsequent design and testing.

For context on the overall development process and where requirements
fit, see the
[F´ Development Process](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/development-practice.md).
For guidance on work types and component kinds, see
[Selecting Component, Port, and Command Kinds](https://github.com/nasa/fprime/blob/devel/docs/user-manual/framework/component-and-port-selection.md).

---

## STOP — Ask the User

You may **draft** requirements based on user input, but you cannot do
requirements in isolation from the user. Draft requirements must stem
from user-provided context and **must be reviewed with the user before
proceeding into development**.

If any of the following are unclear, **ask before proceeding**:

- What is the component's purpose / role in the system?
- What other components does it interact with (upstream / downstream)?
- What commands should the ground operator be able to send?
- What telemetry should the component report?
- What events (logs) should it emit and at what severity?
- What parameters should be configurable at runtime?
- Is this work event-driven, deadline-driven, or background? (See
  `docs/user-manual/framework/component-and-port-selection.md`
  for an explanation of the types of work.)
- Are there any critical deadlines for this work?
- Are there fault-handling or off-nominal requirements?
- What existing F Prime components (Svc/*, Drv/*) can be built on top
  of?

---

## Step-by-Step Process

### Step 1 — Gather Context

Ask the user for:

1. **Component name and module**: Where does this live in the project
   tree? (e.g., `MyProject/Components/ThermalController`)
2. **Work type**: Event-driven, deadline-driven (cyclic), or
   background? This determines the component kind (active, passive, or
   queued). See
   `docs/user-manual/framework/component-and-port-selection.md`.
3. **High-level purpose**: One-paragraph description of what this
   component does.

### Step 2 — Define Behavioral Requirements

Requirements are written as **"shall"** statements; each must be
something that can be **verified** (a pass/fail observable). Goals
that are not pass/fail ("should") go in the SDD narrative, not the
table.

The SDD opens with a short **Introduction** — a few sentences on what
the component is and does, self-contained, with no links or pointers
to further context — and §Requirements is then the table alone: no
request or intent prose, no Terms or glossary subsection. Ship the
requirements as a three-column table — ID, requirement, verification
method — with no rationale or review column and no rationale clauses
in the rows ("… so that …" is deleted; the reviewer and the
verification column carry the why):

| ID | Requirement | Verification |
|---|---|---|
| REQ-\<Component\>-001 | The component shall \<behavior\>. | Unit test / integration test / inspection — strongest feasible |

Keep every row to one sentence of at most 15 words. For a device driver, do not restate
the datasheet: "shall read N bytes at an offset", "shall write N
bytes at an offset" and one row "reads and writes shall be compliant
with \<datasheet, revision\>" are sufficient; frame bytes, opcodes and
timings are design facts verified by unit test against the datasheet.
Keep the fault-scenario rows (invalid input, concurrent request, host
error, wrong device) — they are the rows reviewers find missing.
When a requirement is retired or merged, delete the row and renumber
nothing; no "retired" rows and no change history in the table. Fault
scenarios the component does not own are not listed in the SDD at all
(no Waivers subsection); record them, with their owner, in the review
record or the plan's decision log.

Categories to cover:

| Category | Ask the user |
|---|---|
| **Nominal behavior** | What does it do on each cycle / invocation? |
| **Commands** | What operator commands does it accept? What are the args? |
| **Telemetry** | What values does it periodically report? |
| **Events** | What noteworthy occurrences does it log? Severity? |
| **Parameters** | What runtime-configurable values does it store? |
| **Error handling** | How does it respond to invalid inputs or faults? |
| **State management** | Does it have modes or states? Transitions? |

### Step 3 — Define Interface Requirements

For each external connection, define what **data** the component
exchanges — not port types (those are a design decision, not a
requirement):

- **Inputs**: What data/invocations does the component receive? From
  whom?
- **Outputs**: What data does the component send out? To whom?
- **Data types**: What are the arguments/payloads crossing each
  interface?

> Port types (sync/async/guarded) and FPP port definitions are
> determined during the Design phase based on the work type and timing
> constraints established here.

### Step 4 — Confirm with User

Present the introduction and the requirements table to the user — that
is all the user reads —
and **wait for confirmation** before proceeding to design. Requirements are the
contract — everything downstream is verified against them.

---

## Output Artifact

The **Requirements** section of the component's `docs/sdd.md`, and
nothing longer:

- an **Introduction** of a few sentences (what the component is and
  does; no links or references to further context)
- the requirements table (ID, requirement, verification) — one line
  per row of ≤ 15 words, no retired rows, no rationale or review column
- nothing else: no request, intent, Terms, glossary or Waivers
  subsection; introduction plus table fit in 60 lines

Interface data and constraints go into the shall statements; review
records, dispositions and elicitation answers stay outside the SDD.
Succinct beats complete: the reader has minutes, not hours.

Tables, not rows, for the repeated things (learned on SunCamMgr):

- Persisted device settings go in a **parameter table**, non-persisted
  device operations in a **command table**, channels in a **telemetry
  table**, sizes/limits in a **constants table**; one requirement points
  at each table ("shall push every Table 1 setting") instead of a row per
  setting or command. Tables follow the requirements list.
- Verification column uses exactly `Unit Test`, `Integration Test` or
  `Inspection` — nothing else, no qualifiers.
- One row per requirement, numbered; never `054-a/b/c`. No rows that are
  implementation choices ("shall be active"), negative scope ("shall not
  implement §13–19") or tests ("five consecutive captures deliver a
  product" is a system test, not a requirement).
- Timeouts and settle windows are **tick counts** of the component's
  `run` port; the deployment chooses the rate-group period.
- Parameter names are `UPPER_SNAKE_CASE`; event severities are written as
  the F Prime enumerators (`WARNING_HI`), not prose.

---

## Anti-Patterns

- Transcribing the datasheet into requirements (opcodes, frame bytes,
  timings) instead of one compliance row.
- A request, intent, Terms, glossary or Waivers subsection, links in
  the introduction, or any paragraph in §Requirements; keeping retired
  rows; adding rationale / review-disposition columns or "so that"
  clauses to the table.

- Guessing at what telemetry channels to add
- Assuming command arguments without asking
- Inventing error-handling behavior
- Skipping requirements and jumping to FPP design
- Copying another component's requirements without confirming
  applicability
- Specifying port types or FPP constructs in requirements (those
  belong in the Design phase)
