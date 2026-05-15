# Skill: Trace a value back to determine if it is ground-controlled

The security agent flags asserts, overflow paths, and validation
gaps reachable by **ground input** (commands, parameters, uplink,
telemetry filters, file uplink). To do so, it must trace each
predicate in an assert or arithmetic operation back to its
**source class**:

- `programmer-constant` — compile-time constant, no input
  reachability.
- `internal-state` — derived from internal component state, not
  ground-reachable (modulo prior ground influence on that state, see
  §4).
- `ground-input` — derived from a ground command argument,
  parameter, uplink data, or any other value the ground operator
  controls.
- `hardware-input` — derived from a driver input or hardware
  register. Tracing for this class lives in
  `fprime-hardware-input-tracing.skill.md`.

This skill is the trace procedure for the ground-input class.

---

## 1. Ground-input entry points

The following F Prime constructs receive ground input directly:

| Entry point | Where the data enters |
|---|---|
| **Command arguments** | `Component::CMD_<NAME>_cmdHandler(...)` parameters declared in the `.fpp` command definition. |
| **Parameters** | `Component::paramSet_<NAME>(...)` and `Component::paramGet_<NAME>(...)` flows; values originate from the parameter database, written from ground. |
| **Uplinked file content** | `Svc::FileUplink`-derived input ports / handler bodies. |
| **Telemetry filter / packet selection** | `Svc::TlmPacketizer`, `Svc::ComLogger` configurations that ground can influence. |
| **Cmd opcodes / sequencing** | `Svc::CmdDispatcher` dispatch path; opcode is ground-controlled, dispatch args inherit. |
| **Async input ports labeled "from ground" in the topology** | Any `async input port` wired in the topology from a ground-facing component (CmdDispatcher, ComQueue, FileUplink, etc.). |

Each handler parameter at one of these entry points is **ground-
input** at the moment it enters the agent's component. The trace
proceeds forward from there.

---

## 2. Forward trace within a component

For an offending line `L` in component `C` (e.g., `FW_ASSERT(x <
LIMIT)`):

1. Identify the variable `x` being asserted on.
2. Walk backward from `L` within `C`'s method body using simple
   intra-procedural data-flow:
   - Direct assignment: `x = y;` → trace `y`.
   - Function-call result: `x = foo(a, b);` → if `foo` is in `C`,
     recurse; if `foo` is in `Fw`/`Os`/`Drv`, consult the source-
     class rules in §3.
   - Arithmetic combination: `x = a + b;` → trace both `a` and `b`;
     `x` is `ground-input` if either is.
   - Member access: `x = this->m_foo;` → mark `m_foo` as
     `internal-state` for this trace; cross-reference any code path
     that writes `this->m_foo` from a ground-input source class
     (§4).
3. If the variable is a handler argument at an entry point in §1,
   classify as `ground-input` and stop.
4. If the variable is a literal or a `constexpr` value, classify as
   `programmer-constant` and stop.

---

## 3. Library and primitive source classes

| Construct | Source class |
|---|---|
| Literal, `constexpr`, `enum` value, `static const` | `programmer-constant` |
| `Fw::Time::getTimeBase()`, `Os::IntervalTimer`, similar | `internal-state` |
| `Os::File::read()` results (when reading a path that ground supplied) | `ground-input` (file content is ground-uplinked) |
| Component port input parameters at the entry points in §1 | `ground-input` |
| Random / unpredictable internal state | `internal-state` |
| `Fw::Buffer` payload received over an external port | depends on the port's wiring; consult the topology |

When in doubt, the agent classifies as the more dangerous class
(`ground-input` if there is any plausible ground-reachable path)
and appends a maintainer ping per `maintainer-lookup.skill.md`.

---

## 4. Cross-procedural and cross-component flow

When the data crosses component boundaries (via an output port to
another component's input port), the trace must follow the wiring
in the topology:

1. From the offending line, identify the variable's origin within
   the local component.
2. If the origin is an input port handler argument, look up the
   port in the topology file (`.fpp` or `Topology.cpp`) to find the
   source component and port.
3. Recurse on the source component's output port: what value does
   it pass? Apply §2 within that component.
4. Continue until reaching a §1 entry point (→ `ground-input`), a
   §3 primitive (→ classify), or a hardware-input port (→ hand off
   to `fprime-hardware-input-tracing.skill.md`).

The topology files relevant to the trace usually live in:

- `<Project>/Top/Topology.fpp`
- `<Project>/Top/Topology.cpp`
- `Svc/<Component>/<Component>.fpp` (for component-internal port
  declarations)

---

## 5. Worked example — ground-reachable assert

```cpp
// In Svc/CmdDispatcher/CmdDispatcher.cpp (illustrative)
void CmdDispatcher::Dispatch_cmdHandler(
    FwOpcodeType opCode,
    U32 cmdSeq,
    U32 dispatchOpCode  // ← command argument
) {
    FW_ASSERT(dispatchOpCode < this->m_dispatchMax);   // L
    // ...
}
```

Trace from line `L`:

1. Variable `dispatchOpCode`.
2. Backward in the function body: no reassignment; the parameter is
   the source.
3. The parameter is a command-handler argument → §1 → `ground-input`.

The predicate operand `dispatchOpCode` is `ground-input`. Therefore
`FW_ASSERT` on it is a ground-reachable assert →
`security-review.agent.md` category 1 → `**must fix**`.

Suggestion: replace the assert with a validation:

```suggestion
if (dispatchOpCode >= this->m_dispatchMax) {
    this->log_WARNING_HI_InvalidOpcode(dispatchOpCode);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
    return;
}
```

---

## 6. Worked example — value derived from internal state but
ground-influenced

```cpp
this->m_dispatchMax = newMax;   // newMax came from CMD_SET_MAX
// ...
FW_ASSERT(dispatchOpCode < this->m_dispatchMax);
```

Tracing `m_dispatchMax`:

1. `m_dispatchMax` is `internal-state`.
2. Cross-reference: is there a write path to `m_dispatchMax` from a
   ground-input source class?
3. `CMD_SET_MAX_cmdHandler(newMax)` writes it → `newMax` is
   `ground-input`.
4. Therefore `m_dispatchMax` is effectively `ground-input` (with
   sticky propagation: once an internal-state variable is written
   from a ground source, treat it as `ground-input` for the
   duration of subsequent traces).

The assert is still a ground-reachable assert via a one-step indirection.
`**must fix**`.

---

## 7. Confidence calibration

The agent reports `high confidence` when:

- The trace reaches a §1 entry point within at most 3 hops, OR
- The trace is constrained to one component and reaches a §3
  primitive cleanly.

The agent reports `low confidence` when:

- The trace crosses ≥ 4 component boundaries.
- The trace involves dynamic dispatch (function pointers, virtual
  calls into a base class with multiple subclasses).
- The agent cannot resolve a topology wiring (the relevant `.fpp` or
  `Topology.cpp` is not in the agent's read scope, or the wiring is
  conditional on a build option).

Low confidence → tag at the right severity + maintainer ping per
`maintainer-lookup.skill.md`.

---

## 8. One-line summary

`Walk backwards from the offending variable; classify origin as
programmer-constant / internal-state / ground-input / hardware-input.
Cross-component flow follows topology wiring. When in doubt, the
more dangerous class wins and the maintainer is pinged.`
