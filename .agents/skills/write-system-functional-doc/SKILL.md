---
name: write-system-functional-doc
description: Analyze an F Prime component or subsystem and write a high-level system-functional document for docs/reference/system-functional/.
argument-hint: <component-path e.g. Svc/CmdDispatcher>
triggers: ["user"]
---

## Overview

Write a **system-functional document** for the specified F Prime component or subsystem. These documents live in `docs/reference/system-functional/` and provide high-level functional descriptions aimed at systems engineers — not implementation details.

## Reference Documents

Before writing, read the existing system-functional documents to match their tone, depth, and formatting:

1. Read `docs/reference/system-functional/sequencing.md` — narrative functional description with References, Overview, and subsections for each aspect of the capability.
2. Read `docs/reference/system-functional/dictionary.md` — reference-style listing of capabilities and options.
3. Read `docs/reference/system-functional/index.md` — the index page that lists all system-functional documents.

## Research the Component

Thoroughly investigate the target component at `$ARGUMENTS`:

1. Read the FPP model file(s) (`*.fpp`) to understand ports, commands, events, telemetry, and parameters.
2. Read the SDD (`docs/sdd.md`) within the component directory for requirements, design, and functional description.
3. Read the implementation source (`.cpp` and `.hpp` files) to understand behavior, error handling, and off-nominal cases.
4. Read any configuration headers referenced by the component (e.g. files in `config/`).
5. If the component interacts closely with other components (e.g. it dispatches to or is dispatched by other components), briefly review those related components to understand the broader subsystem context.

## Write the Document

Create a new markdown file at `docs/reference/system-functional/<name>.md` where `<name>` is a short, descriptive kebab-case name for the capability (e.g. `command-dispatch.md`, `telemetry-collection.md`).

Follow this structure (adapt sections as appropriate for the component):

```markdown
# <Capability Name> Functionality

## References

- Link to the component SDD(s) on GitHub (use `https://github.com/nasa/fprime/blob/devel/...` URLs)
- Link to any related component SDDs
- Link to relevant FPP User Guide sections if applicable

## Overview

A 2-4 sentence high-level description of what this capability does and why it exists. Write for a systems engineer audience — describe *what* the system does, not *how* the code works.

## <Functional Aspect 1>

Describe the first major functional aspect. Use plain language. Cover:
- What the function/capability is
- How it is used or triggered
- Any configurable aspects
- Constraints or limits

## <Functional Aspect 2>

Continue with additional aspects as needed.

## Off Nominal

Describe error handling, failure modes, and recovery behavior.
```

### Writing Guidelines

- **Audience**: Systems engineers, not software developers. Avoid code-level details.
- **Tone**: Match the existing documents — concise, factual, declarative.
- **Scope**: Describe the *functional capability*, not the software component. For example, write about "command dispatching" not "the CmdDispatcher class".
- **References**: Always include a References section linking to the component SDD(s) and any related documentation.
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
