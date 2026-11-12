---
name: fprime-component-design-fpp
description: >-
  FPP design phase of F Prime component development. Guides the agent
  through creating the FPP model that codifies the component's
  interface — ports, commands, events, telemetry, and parameters.
  Trigger when the requirements are confirmed and the next step is to
  write the .fpp file. Keywords: F Prime, FPP, design, model, ports,
  commands, telemetry, events, parameters, component definition.
---

# Skill: F Prime Component Design (FPP)

The design phase **codifies requirements into an FPP model** — the
single source of truth for the component's interface. The FPP compiler
generates C++ base classes, test harnesses, and dictionaries from this
model.

For FPP syntax and modeling details, see the
[FPP User's Guide](https://nasa.github.io/fpp/fpp-users-guide.html).
For component/port/command kinds, see
[Selecting Component, Port, and Command Kinds](https://github.com/nasa/fprime/blob/devel/docs/user-manual/framework/component-and-port-selection.md).

**Reference the F Prime design patterns** where possible — standard
solutions exist for common needs:

- Rate Group Pattern — `docs/user-manual/design-patterns/rate-group.md`
- Health Checking — `docs/user-manual/design-patterns/health-checking.md`
- Manager-Worker — `docs/user-manual/design-patterns/manager-worker.md`
- Application-Manager-Driver — `docs/user-manual/design-patterns/app-man-drv.md`
- Common Port Patterns — `docs/user-manual/design-patterns/common-port-patterns.md`
- Hub Pattern — `docs/user-manual/design-patterns/hub-pattern.md`
- Subtopologies — `docs/user-manual/design-patterns/subtopologies.md`

---

## STOP — Confirm Before Designing

Before writing any FPP:

1. **Requirements must be confirmed** (see
   `fprime-component-requirements`).
2. **Ask the user** about any ambiguity in:
   - Port types (does a suitable port already exist, or do we need a
     new one?)
   - Command argument types and names
   - Telemetry channel update frequency semantics (on-change vs.
     periodic)
   - Event severity levels (DIAGNOSTIC, ACTIVITY_LO, ACTIVITY_HI,
     WARNING_LO, WARNING_HI, FATAL)
   - Parameter default values
   - Whether to use existing F Prime standard ports (`Fw.Signal`,
     `Svc.Sched`, `Svc.Ping`, etc.)

---

## Step-by-Step Process

### Step 0 — Scaffold with fprime-util

Start with `fprime-util new --component` for the base structure. This
generates the initial directory layout, CMakeLists.txt, and FPP stub.

### Step 1 — Choose Component Kind

Based on requirements (see
`docs/user-manual/framework/component-and-port-selection.md` for the
full decision guide):

| Work Type | Component Kind | When to Use |
|---|---|---|
| Cyclic (hard deadline) | `passive` | Rate-group driven, no own thread |
| Cyclic with internal queue | `queued` | Deferred work within a cycle |
| Event-driven | `active` | Owns a thread, processes async messages |

**Ask the user if unsure.** The component kind determines thread
ownership and port synchronization semantics.

### Step 2 — Define Ports

For each interface requirement, define input/output ports. Port kinds:
- `sync input` — executes in caller's thread
- `async input` — enqueued, executes in component's thread
- `guarded input` — sync with component-wide mutex

See the
[FPP User's Guide](https://nasa.github.io/fpp/fpp-users-guide.html)
for port definition syntax.

**Standard framework ports to consider including:**

| Need | Port | Import |
|---|---|---|
| Rate-group scheduling | `sync input port run: Svc.Sched` | `import Svc.Sched` |
| Health ping | `async input port pingIn: Svc.Ping drop` | `import Svc.Ping` |
| Time access | `time get port timeCaller` | (special port) |
| Command handling | `command recv port cmdIn` | (special port) |
| Event logging | `event port eventOut` | (special port) |
| Telemetry | `telemetry port tlmOut` | (special port) |

> If the component is `active`, it probably needs health-ping support.
> Requirements should imply whether rate-group scheduling is needed.
> **Ask the user only if unsure** about which standard services to
> connect to.

### Step 3 — Define Commands, Telemetry, Events, Parameters

Follow the
[FPP User's Guide](https://nasa.github.io/fpp/fpp-users-guide.html)
for syntax. Follow
[F Prime Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines)
for naming conventions:
- Commands: `UPPER_SNAKE_CASE`
- Telemetry channels: `PascalCase`
- Events: `PascalCase`
- Parameters: `PascalCase`

For each construct, derive names, types, and semantics from the
confirmed requirements. Define an event for each command that
describes what the command did, including the command arguments, so
the implementation can emit it (CPP-37). Every event emission must be
traceable to a single call site (CPP-36): give each site its own
event, or an argument (typically an enum) whose value differs at
every site. Ask the user if:
- The command kind (sync/async/guarded) is ambiguous
- The event severity level is unclear
- Parameter defaults or valid ranges are unspecified

Lessons from SunCamMgr (externally powered device):

- Hardware powered up by *another* component after FSW boot gets an
  explicit **setup command** (probe → settle → apply parameters); do not
  probe on `drvConnected`, and a power-up banner arriving during the
  probe acknowledges the setup, it does not fail it.
- A parameter changed while the device is ready is pushed as soon as the
  transaction slot is free; before readiness it is stored for the next
  settings apply. Setting channels change only from device replies
  (acknowledgements only where the device has no readback).
- `parameterUpdated(id)` receives the **local** `PARAMID_*`; command
  opcodes carry the base id. Unit-test the hooks with a non-zero base id —
  base-id-0 tests hide the confusion.
- A setting whose push re-settles the device (resolution) must be
  handled in every activity that can push it (setup, apply, dirty push).

### Step 4 — Build and Verify

```bash
fprime-util build
```

The FPP model should compile and generate the base class. Fix any FPP
errors before proceeding.

### Step 5 — Draft the SDD

Draft `docs/sdd.md` to the shape and length of
[`Svc/DpWriter/docs/sdd.md`](https://github.com/nasa/fprime/blob/devel/Svc/DpWriter/docs/sdd.md).
Numbered sections, in this order and nothing more:

1. **Introduction** — what the component manages and for whom, one
   paragraph. No out-of-scope list here (see Idiosyncrasies).
2. **Requirements** — the approved table.
3. **Design**
   - **Component Diagram** — one box for the component; input ports
     as small squares on the **left** edge, output ports on the
     **right**, each `name : PortType` (`[N]` for arrays); special
     ports (command, event, telemetry, time, parameter) are one note
     under the box. Generated from the FPP model (`fpp-to-layout` +
     fprime-visual, the generator in `tools/`, or a fenced ASCII box).
     No topology, no peer components.
   - **Ports** — table: name, kind (sync/async/guarded, array size),
     type, purpose in ≤ 8 words; the reason for the kind only when
     not obvious, as one clause. Queue-full handling is one sentence
     under the table ("Hooks report BUSY to the command dispatcher on
     queue overflow"), not a description of the hook mechanics.
   - **State** — table of the member variables (name, values,
     meaning) and a Mermaid `stateDiagram-v2` when the component has
     modes.
   - **Configuration and Setup** — the configuration constants (FPP
     constant block or `config/` header) and the topology `instance`
     line, each as a fenced snippet with one line of meaning. No rate,
     queue, buffer-pool or scheduling tuning and no size arithmetic —
     those belong to the deployment.
   - **Port Handlers** — one subsection per handler (commands may
     share one), **two short sentences each**: what admits the call,
     what it does. No flowcharts of handler internals, no outcome
     tables — the events table already lists the outcomes.
4. **Data Product Format** (if any) — tables only: container / record
   id / type / count; struct fields (name, type, value); byte layout
   of any raw record. No container-size calculations, no buffer-bin
   advice, no sequence diagram here.
5. **Ground Interface** — Commands, Parameters, Telemetry, Events
   tables, one row each; a table that is empty is one sentence.
6. **Example Uses** — one Mermaid `sequenceDiagram` of the main
   operation (request → hardware → reply → product). No topology
   diagram (wiring is the deployment's SDD).
7. **Idiosyncrasies** (if any) — a documented-vs-observed table of
   hardware deviations and the out-of-scope table (failure, owner),
   one SHORT line each. No error-handling narrative.
8. **Tests** — the system-test table (ID, title, requirements, pass
   rule) and the unit-test list.

Budget: tables and diagrams carry the content; prose is one sentence
per table and two per handler. Every statement traces to the FPP
model, the code or a requirement, and nothing is said twice — when a
section grows, cut before adding. Design notes, alternatives and
rationale go in the PR description or the review record, never in a
tracked `docs/design-notes.md` (the F Prime lead removes such files
from PRs). No change history ("was/now", revision table) and no
Waivers subsection. A subtopology is written up in
`Subtopology/docs/sdd.md`, not in the component SDD.

Mermaid: `;` ends a statement, so never use it inside a label, message
or `loop` title; render every block with mermaid-cli (`mmdc`) before
pushing — GitHub shows a parse error, not the diagram.

### Step 6 — Confirm Design with User

Present the SDD draft — the one document the user reads, with the
FPP model attached — and **wait for confirmation**. The model is the
interface contract — implementation and tests are written against it.

---

## Output Artifact

- The `.fpp` model (and any new port definitions) that builds.
- The `docs/sdd.md` **Design** section of Step 5 (block diagram with
  ports on the left/right edges, tables, types and constants, behavior
  with a sequence diagram where contexts interleave) — no topology
  delta, no review material.

---

## New Port Definitions

If a requirement needs a port type that doesn't exist:

1. **Ask the user** what the port should carry (arguments, return
   type).
2. Create a new `.fpp` file in an appropriate `Ports/` directory.
   See the
   [FPP User's Guide](https://nasa.github.io/fpp/fpp-users-guide.html)
   for port definition syntax.
3. Add to `CMakeLists.txt` and register with `add_fprime_subdirectory`.

---

## Anti-Patterns

- Guessing port types — always check existing ports first and ask
- Using `async` ports for cyclic/deadline work
- Omitting time get port when telemetry or events are defined
- One event emitted from several sites with indistinguishable
  arguments (CPP-36)
- Hardcoding opcodes/IDs without checking for conflicts (let the
  build system assign them, or ask the user for the base ID)
- Using bare primitive types (`int`, `unsigned`) — use `U32`,
  `FwSizeType`, etc. (CPP-3, CPP-28)
