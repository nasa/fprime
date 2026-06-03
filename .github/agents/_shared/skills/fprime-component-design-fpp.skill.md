---
name: fprime-component-design-fpp
description: FPP design phase of F Prime component development. Guides the agent through creating the FPP model that codifies the component's interface — ports, commands, events, telemetry, and parameters. Trigger when the requirements are confirmed and the next step is to write the .fpp file. Keywords: F Prime, FPP, design, model, ports, commands, telemetry, events, parameters, component definition.
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
   `fprime-component-requirements.skill.md`).
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
| Health ping | `async input port pingIn: Svc.Ping` | `import Svc.Ping` |
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
confirmed requirements. Ask the user if:
- The command kind (sync/async/guarded) is ambiguous
- The event severity level is unclear
- Parameter defaults or valid ranges are unspecified

### Step 4 — Build and Verify

```bash
fprime-util build
```

The FPP model should compile and generate the base class. Fix any FPP
errors before proceeding.

### Step 5 — Confirm Design with User

Present the FPP model to the user and **wait for confirmation**. The
model is the interface contract — implementation and tests are written
against it.

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
- Hardcoding opcodes/IDs without checking for conflicts (let the
  build system assign them, or ask the user for the base ID)
- Using bare primitive types (`int`, `unsigned`) — use `U32`,
  `FwSizeType`, etc. (CPP-3, CPP-28)
