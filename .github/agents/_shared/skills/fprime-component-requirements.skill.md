---
name: fprime-component-requirements
description: Requirements phase of F Prime component development. Guides the agent through eliciting, documenting, and confirming component requirements before any design or code is written. Trigger when starting a new component or when the user says to define requirements. Keywords: F Prime, requirements, SDD, behavior, interface, specification.
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

Requirements are written as **"shall"** or **"should"** statements.
Each requirement must be something that can be
**validated/verified/tested**.

Requirements are typically shipped as a table:

| ID | Shall Statement | Description / Context | Test Method |
|---|---|---|---|
| REQ-\<Component\>-001 | The component shall \<behavior\>. | \<Human-readable context\> | \<Unit test / integration test / inspection\> |
| REQ-\<Component\>-002 | ... | ... | ... |

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

Present the full requirements table to the user and **wait for
confirmation** before proceeding to design. Requirements are the
contract — everything downstream is verified against them.

---

## Output Artifact

A requirements document (typically captured in the component's
`docs/sdd.md` or as a structured list in the PR description) with:

- Component name, module, and work type
- Requirements table (ID, shall statement, description, test method)
- Interface summary (data exchanged, not port types)
- Any constraints or assumptions noted

---

## Anti-Patterns

- Guessing at what telemetry channels to add
- Assuming command arguments without asking
- Inventing error-handling behavior
- Skipping requirements and jumping to FPP design
- Copying another component's requirements without confirming
  applicability
- Specifying port types or FPP constructs in requirements (those
  belong in the Design phase)
