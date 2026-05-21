---
name: write-system-functional-doc
description: Analyze an F Prime component, subtopology, or multi-component subsystem and write a high-level system-functional document for docs/reference/system-functional/.
argument-hint: <component-or-subsystem-path(s) e.g. Svc/CmdDispatcher, Svc/Subtopologies/CdhCore, or "Svc/RateGroupDriver Svc/ActiveRateGroup Svc/PassiveRateGroup">
triggers: ["user"]
---

## Overview

Write a **system-functional document** for the specified F Prime capability. These documents live in `docs/reference/system-functional/` and provide high-level functional descriptions aimed at systems engineers — not implementation details.

The input can be one of three things:

1. **A single component path** (e.g. `Svc/CmdDispatcher`) — a single component that provides a capability on its own.
2. **A subtopology path** (e.g. `Svc/Subtopologies/CdhCore`) — a formalized group of components bundled together via FPP's subtopology mechanism.
3. **Multiple component paths** (e.g. `Svc/RateGroupDriver Svc/ActiveRateGroup Svc/PassiveRateGroup`) — an informal set of components that collaborate to provide a capability, even though they are not packaged as a subtopology.

In all cases, the document describes the **functional capability** — not the individual components. The components are implementation details that deliver the capability.

## Reference Documents

Before writing, read the existing system-functional documents to match their tone, depth, and formatting:

1. Read `docs/reference/system-functional/sequencing.md` — a good example of a multi-component capability (CmdSequencer + SeqDispatcher + CmdDispatcher working together). Notice how it describes the capability holistically without dwelling on individual component boundaries.
2. Read `docs/reference/system-functional/dictionary.md` — reference-style listing of capabilities and options.
3. Read `docs/reference/system-functional/index.md` — the index page that lists all system-functional documents.

## Identify the Scope

Determine what kind of input was provided and identify all participating components:

### If a single component path

1. Read the component's FPP model, SDD, and source files.
2. Examine its ports to identify components it directly interacts with. Look at port types and connection patterns.
3. Decide: does this component deliver a capability on its own, or is it part of a larger subsystem? If part of a larger subsystem, expand your scope to include the collaborating components.

### If a subtopology path

1. Read the subtopology's FPP definition (the `topology` block) to identify all component instances and their connections.
2. Read the subtopology's SDD for the overall design intent.
3. Read each participating component's FPP model and SDD to understand individual roles.
4. Identify the external interfaces (ports exposed to the parent topology) — these define the capability boundary.

### If multiple component paths (informal subsystem)

1. Read each component's FPP model, SDD, and source files.
2. Trace the data flow between them: which ports connect them, what triggers what.
3. Identify the overall capability they deliver together and the external interfaces to the rest of the system.

## Research Each Participating Component

For every component identified in the scope:

1. Read the FPP model file(s) (`*.fpp`) to understand ports, commands, events, telemetry, and parameters.
2. Read the SDD (`docs/sdd.md`) within the component directory for requirements, design, and functional description.
3. Read the implementation source (`.cpp` and `.hpp` files) to understand behavior, error handling, and off-nominal cases.
4. Read any configuration headers referenced by the component (e.g. files in `config/` or subtopology config modules).
5. Note how this component interacts with the other components in the subsystem — what does it send, what does it receive, what triggers it.

## Write the Document

Create a new markdown file at `docs/reference/system-functional/<name>.md` where `<name>` is a short, descriptive kebab-case name for the **capability** (e.g. `command-dispatch.md`, `rate-group-scheduling.md`, `communication-stack.md`).

Follow this structure (adapt sections as appropriate):

```markdown
# <Capability Name> Functionality

## References

- Link to each participating component's SDD on GitHub (use `https://github.com/nasa/fprime/blob/devel/...` URLs)
- Link to the subtopology SDD if applicable
- Link to relevant FPP User Guide sections if applicable

## Overview

A 2-4 sentence high-level description of what this capability does and why it exists. Write for a systems engineer audience — describe *what* the system does, not *how* the code works.

If multiple components collaborate, briefly describe their roles and how they work together in a sentence or two. For example: "This capability is provided by the RateGroupDriver, which divides a clock signal, and one or more ActiveRateGroup or PassiveRateGroup instances, each of which calls a set of components at a specific rate."

## <Functional Aspect 1>

Describe the first major functional aspect. Use plain language. Cover:
- What the function/capability is
- How it is used or triggered
- Any configurable aspects
- Constraints or limits

## <Functional Aspect 2>

Continue with additional aspects as needed.

## Off Nominal

Describe error handling, failure modes, and recovery behavior across the subsystem.
```

### Writing Guidelines

- **Audience**: Systems engineers, not software developers. Avoid code-level details.
- **Tone**: Match the existing documents — concise, factual, declarative.
- **Scope**: Describe the *functional capability*, not the software components. The document should read as a description of what the system does, not a tour of the source code. Component names may appear in the Overview to orient the reader, but the body should focus on behavior.
- **Multi-component capabilities**: When a capability spans multiple components, describe the data/control flow between them as a unified process. Do not write separate sections per component — organize by functional aspect instead. For example, for rate groups: sections on "Clock Division", "Rate Group Execution", "Overrun Detection" rather than "RateGroupDriver Component", "ActiveRateGroup Component".
- **Subtopology specifics**: If the capability is delivered as a subtopology, mention that it is available as a reusable subtopology and note any configurable aspects (base IDs, queue sizes, swappable component instances). Do not document the subtopology mechanism itself — just the functional capability it provides.
- **References**: Always include a References section linking to **all** participating component SDDs and any related documentation.
- **No code snippets**: Do not include code. Use plain English to describe behavior.
- **Configuration**: Mention compile-time or runtime configuration options by describing what they control, not by referencing specific config macros.
- **Numbering**: Use numbered lists for ordered processes (like validation steps). Use bullet lists for unordered items.

## Update the Index

After creating the document, update `docs/reference/system-functional/index.md` to add a link to the new document. Follow the existing format:

```markdown
- __<Display Name>__ - <Brief one-line description>
```

## Create a PR

1. Create a branch and commit the new document and the updated index.
2. Open a PR with a clear title like "Add system-functional doc for <capability>".
3. Wait for CI to pass.
