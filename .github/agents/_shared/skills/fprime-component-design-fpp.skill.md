---
name: fprime-component-design-fpp
description: FPP design phase of F Prime component development. Guides the agent through creating the FPP model that codifies the component's interface — ports, commands, events, telemetry, and parameters. Trigger when the requirements are confirmed and the next step is to write the .fpp file. Keywords: F Prime, FPP, design, model, ports, commands, telemetry, events, parameters, component definition.
---

# Skill: F Prime Component Design (FPP)

The design phase **codifies requirements into an FPP model** — the
single source of truth for the component's interface. The FPP compiler
generates C++ base classes, test harnesses, and dictionaries from this
model.

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
`docs/user-manual/framework/component-and-port-selection.md`):

| Work Type | Component Kind | When to Use |
|---|---|---|
| Cyclic (hard deadline) | `passive` | Rate-group driven, no own thread |
| Cyclic with internal queue | `queued` | Deferred work within a cycle |
| Event-driven | `active` | Owns a thread, processes async messages |

**Ask the user if unsure.** The component kind determines thread
ownership and port synchronization semantics.

### Step 2 — Define Ports

For each interface requirement, define a port in the FPP model:

```fpp
@ Description of what this port does
<kind> input port <name>: <PortType>

output port <name>: <PortType>
```

Port kinds for input ports:
- `sync input` — executes in caller's thread
- `async input` — enqueued, executes in component's thread
- `guarded input` — sync with component-wide mutex

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

### Step 3 — Define Commands

```fpp
@ Description of command
async command COMMAND_NAME(
    arg1: U32 @< Description of arg1
    arg2: Fw.String @< Description of arg2
) opcode 0x00
```

Follow
[F Prime Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines)
for naming conventions. Commands use UPPER_SNAKE_CASE. Confirm with
user if the command kind (sync/async/guarded) is ambiguous.

### Step 4 — Define Telemetry Channels

```fpp
@ Description of channel
telemetry ChannelName: <Type> \
    id 0x00 \
    update on change  @< or: update always
```

Follow
[F Prime Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines)
for naming. Channels use PascalCase. Derive data type and update
semantics from requirements.

### Step 5 — Define Events

```fpp
@ Description of event
event EventName(
    arg1: U32 @< Description
) severity <SEVERITY> \
    id 0x00 \
    format "Event occurred with value {}"
```

Severity levels (ask the user if the appropriate level is unclear):
- `DIAGNOSTIC` — debug-level, not shown by default
- `ACTIVITY_LO` — routine activity
- `ACTIVITY_HI` — notable activity
- `WARNING_LO` — off-nominal, non-critical
- `WARNING_HI` — off-nominal, attention needed
- `FATAL` — unrecoverable, system-level response expected

### Step 6 — Define Parameters

```fpp
@ Description of parameter
param ParamName: <Type> default <value> \
    id 0x00
```

Follow
[F Prime Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines)
for naming. Derive parameter names, types, default values, and valid
ranges from requirements.

### Step 7 — Assemble the Complete FPP File

Structure:

```fpp
module <ModuleName> {

    @ Component description
    <kind> component <ComponentName> {

        # --- Ports ---
        ...

        # --- Special ports (commands, events, telemetry, time) ---
        ...

        # --- Commands ---
        ...

        # --- Telemetry ---
        ...

        # --- Events ---
        ...

        # --- Parameters ---
        ...
    }
}
```

### Step 8 — Create CMakeLists.txt

```cmake
register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/<ComponentName>.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/<ComponentName>.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/<ComponentName>.hpp"
)
```

Add the component directory to the deployment with
`add_fprime_subdirectory`.

### Step 9 — Verify Compilation

```bash
fprime-util build
```

The FPP model should compile and generate the base class. Fix any FPP
errors before proceeding.

### Step 10 — Confirm Design with User

Present the FPP model to the user and **wait for confirmation**. The
model is the interface contract — implementation and tests are written
against it.

---

## New Port Definitions

If a requirement needs a port type that doesn't exist:

1. **Ask the user** what the port should carry (arguments, return
   type).
2. Create a new `.fpp` file in an appropriate `Ports/` directory:

```fpp
module <Module> {
    @ Description
    port <PortName>(
        arg1: <Type>
        arg2: <Type>
    )
}
```

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
