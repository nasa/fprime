---
name: fprime-model-queries
description: >-
  Answer questions about an F Prime component or topology from the FPP
  model instead of reading autocoded C++. Covers grep recipes for the
  `.fpp` keywords, the FPP-to-C++ naming table (handlers, commands,
  events, telemetry, parameters, tester base), where autocode lives,
  and codegen gotchas. Use before opening a `*Ac.hpp`, before guessing
  a handler or macro name, and when tracing wiring across a topology.
  Keywords: F Prime, FPP, grep, handler name, autocode, Ac.hpp,
  TesterBase, topology wiring, port type.
---

# Skill: Querying the F Prime model

The `.fpp` file is the source of truth and is 5–10× smaller than the
autocode it produces. Every generated C++ name is a deterministic
function of the model, so read the model and derive the name; open the
generated header only for an exact signature you cannot derive.

---

## 1. Grep recipes

FPP keywords are multi-word. Match the whole prefix or you will miss
most declarations (`async input port`, not `input port`).

| Want | Pattern (`grep -nE` on the `.fpp`) |
|---|---|
| Input ports | `^\s*(async|sync|guarded) input port` |
| Output ports | `^\s*output port` |
| Commands | `^\s*(async|sync|guarded) command` |
| Events | `^\s*event ` |
| Telemetry | `^\s*telemetry ` |
| Parameters | `^\s*param ` |
| Special ports | `^\s*(command (recv|reg|resp)|event|text event|telemetry|time get|param (get|set)|product (get|request|send|recv)) port` |
| Interfaces used | `^\s*import ` inside a `component` |
| Everything, one screen | `^\s*(async|sync|guarded|output|command|text|time|param|product|event|telemetry|import|include|state machine)\b` |

Port arrays are declared as `port name: [N] Type`; `N` is usually a
constant from `default/config/AcConstants.fpp` (or the project's
override), so follow the name there to get the number.

Across the tree:

- Who uses a port type: `grep -rlE "port \w+:.*Svc\.Ping" --include=*.fpp`.
- Where a symbol is defined: `fpp-locate-defs <file>`; or grep
  `build-fprime-*/locs.fpp` for `Svc.Ping`.
- Who is wired to an instance: in the deployment's `topology.fpp` and
  every `Svc/Subtopologies/*/*.fpp`, grep the instance name inside
  `connections` blocks **and** the pattern specifiers
  (`command connections instance cmdDisp`, `event connections instance
  eventLogger`, ...). A pattern connects every instance in the topology,
  including the named one itself.

---

## 2. Name mapping (model → generated C++)

`<Comp>ComponentBase` is the autocoded base class; the implementation
derives from it. `<Comp>TesterBase` / `<Comp>GTestBase` are the
unit-test counterparts. Full signatures and semantics:
[Autocoded Functions and Component Classes](https://github.com/nasa/fprime/blob/devel/docs/user-manual/framework/autocoded-functions.md).

| Model | Implementation must define | Implementation may call | Tester side |
|---|---|---|---|
| input port `foo` | `foo_handler(FwIndexType portNum, ...)`; async ports also get an optional `foo_preMsgHook` | | `invoke_to_foo(portNum, ...)` |
| output port `bar` | | `bar_out(portNum, ...)`, `isConnected_bar_OutputPort(portNum)`, `getNum_bar_OutputPorts()` | `from_bar_handler(...)`, `ASSERT_from_bar_SIZE(n)`, `ASSERT_from_bar(idx, ...)` |
| command `CMD` | `CMD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, args...)` | `cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse)`, enum `OPCODE_CMD` | `sendCmd_CMD(instance, cmdSeq, args...)`, `ASSERT_CMD_RESPONSE_SIZE(n)`, `ASSERT_CMD_RESPONSE(idx, opcode, seq, resp)` |
| event `E` (severity `warning high`) | | `log_WARNING_HI_E(args...)`, enum `EVENTID_E`. Severities spell `FATAL`, `WARNING_HI`, `WARNING_LO`, `COMMAND`, `ACTIVITY_HI`, `ACTIVITY_LO`, `DIAGNOSTIC` | `ASSERT_EVENTS_E_SIZE(n)`, `ASSERT_EVENTS_E(idx, args...)`, `ASSERT_EVENTS_SIZE(n)` |
| telemetry `T` | | `tlmWrite_T(value, timeTag = ...)`, enum `CHANNELID_T` | `ASSERT_TLM_T_SIZE(n)`, `ASSERT_TLM_T(idx, value)`, `ASSERT_TLM_SIZE(n)` |
| param `P` | optional `parameterUpdated(FwPrmIdType id)` | `paramGet_P(Fw::ParamValid& valid)`, enum `PARAMID_P` | `paramSet_P(value, valid)`, `paramSend_P(instance, cmdSeq)`, `paramSave_P(instance, cmdSeq)` |
| product container `C` | `dpRecv_C_handler(DpContainer&, Fw::Success)` | `dpGet_C(size, container)`, `dpSend(container)` | `ASSERT_PRODUCT_GET/REQUEST/SEND(...)` |
| internal port `ip` | `ip_internalInterfaceHandler(args...)` | `ip_internalInterfaceInvoke(args...)` | |

Port handler argument types follow the port definition's `args`
exactly; `FwIndexType portNum` is always first. Command handlers must
end with `cmdResponse_out`.

---

## 3. Where autocode lives, and when to read it

- `build-fprime-automatic-native/F-Prime/<module path>/<Comp>ComponentAc.hpp`
  (`-ut` cache for `TesterBase`/`GTestBase`). Never edit these files.
- `fprime-util impl` / `fprime-util impl --ut` writes the exact stubs
  to `<Comp>-template.{hpp,cpp}`; use it instead of hand-deriving a
  long signature. Both need a build cache (`fprime-util generate`).
- `fprime-util fpp-check` from the module directory validates an edited
  `.fpp` without a full build.
- `fprime-util fpp-to-dict` on a deployment yields the dictionary JSON
  with resolved opcodes, event/channel IDs, enum values and formats —
  use it for GDS and integration-test work rather than computing IDs by
  hand (`base id` + local id).
- `fpp-to-json` is **not** a shortcut: it emits ~15× the size of the
  source file.

---

## 4. Codegen gotchas

- Command `string` arguments always autocode to `Fw::CmdStringArg`
  (`FW_CMD_STRING_MAX_SIZE`, default 40) whatever `string size N` the
  model says; longer values fail deserialization with `FORMAT_ERROR`.
  Port string arguments honor their declared size.
- Pattern specifiers (`event connections instance X`, `telemetry
  connections instance X`, `command connections instance X`) include
  the instance `X` itself; a sink component's own events and telemetry
  are routed back into it.
- `log_*` and `tlmWrite_*` check `isConnected_*` before invoking, so an
  unconnected special port is silent, not fatal.
- `event ... throttle N` stops emitting that event after `N` instances;
  a self-routed event under a pattern specifier recurses up to the
  throttle.
- Only `deployment topology X` autocodes `<X>TopologyAc.*` and a
  dictionary; a bare `fpp-to-cpp` on a scratch model emits no topology
  autocode, so it cannot be used to check pattern wiring.
- Port numbers are compile-time constants: a port array of size `N`
  rejects `portNum >= N` with an assertion in the generated base class.
- Subtopology instances are addressed as `<Subtopology>.<instance>`
  in the deployment topology (e.g. `CdhCore.cmdDisp`).
