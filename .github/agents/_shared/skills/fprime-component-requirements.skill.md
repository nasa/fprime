---
name: fprime-component-requirements
description: Requirements phase of F Prime component development. Guides the agent through eliciting, documenting, and confirming component requirements before any design or code is written. Trigger when starting a new component or when the user says to define requirements. Keywords: F Prime, requirements, SDD, behavior, interface, specification.
---

# Skill: F Prime Component Requirements

Requirements define **what** the component must do — its behaviors,
interfaces, and constraints. This phase produces a written requirements
list that drives all subsequent design and testing.

---

## STOP — Ask the User

**You cannot invent requirements.** Requirements come from the user
(the system engineer, developer, or mission context). If any of the
following are unclear, **ask before proceeding**:

- What is the component's purpose / role in the system?
- What other components does it interact with (upstream / downstream)?
- What commands should the ground operator be able to send?
- What telemetry should the component report?
- What events (logs) should it emit and at what severity?
- What parameters should be configurable at runtime?
- Are there timing constraints (rate-group driven, event-driven,
  background)?
- Are there fault-handling or off-nominal requirements?
- What existing F Prime components (Svc/*, Drv/*) should be reused?

---

## Step-by-Step Process

### Step 1 — Gather Context

Ask the user for:

1. **Component name and module**: Where does this live in the project
   tree? (e.g., `MyProject/Components/ThermalController`)
2. **Component kind**: Passive, queued, or active? (See
   `docs/user-manual/framework/component-and-port-selection.md` for
   guidance on choosing.)
3. **High-level purpose**: One-paragraph description of what this
   component does.

### Step 2 — Define Behavioral Requirements

For each distinct behavior, produce a requirement in the form:

```
REQ-<Component>-<001>: The component shall <behavior>.
```

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

For each external connection:

- **Input ports**: What data/invocations does the component receive?
  From whom? Sync/async/guarded?
- **Output ports**: What does the component send out? To whom?
- **Port data types**: What arguments cross each port?

> **If the port type doesn't already exist**, note it — a new port
> definition will be needed in the Design phase.

### Step 4 — Confirm with User

Present the full requirements list to the user and **wait for
confirmation** before proceeding to design. Requirements are the
contract — everything downstream is verified against them.

---

## Output Artifact

A requirements document (typically captured in the component's
`docs/sdd.md` or as a structured list in the PR description) with:

- Component name, module, and kind
- Numbered behavioral requirements (REQ-<Name>-001, etc.)
- Interface summary (ports, commands, telemetry, events, parameters)
- Any constraints or assumptions noted

---

## Anti-Patterns

- ❌ Guessing at what telemetry channels to add
- ❌ Assuming command arguments without asking
- ❌ Inventing error-handling behavior
- ❌ Skipping requirements and jumping to FPP design
- ❌ Copying another component's requirements without confirming
  applicability
