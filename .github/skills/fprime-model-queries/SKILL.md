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

A component's model is usually more than one file:

- `include "Commands.fppi"` pulls in a fragment. Half of `Svc/` keeps
  commands, events, telemetry and parameters in `.fppi` files, so grep
  `--include=*.fpp --include=*.fppi` and follow every `include` line.
- `import Svc.Deframer` inside a component pulls in an *interface*
  (`Fw/Interfaces/`, `Svc/Interfaces/`, `Drv/Interfaces/`, or the
  project's). Its ports belong to the component but are declared in
  the interface file, and interfaces can import other interfaces. Run
  the port recipes on the interface file too.
- Inside a `topology`, `import CdhCore.Subtopology` is a subtopology,
  not an interface.
- `$` escapes a name that collides with a keyword (`$health`, `$recv`);
  the `$` is dropped in C++.

---

## 1. Grep recipes

FPP keywords are multi-word and `event`, `telemetry`, `param` also
start special-port lines (`event port Log`) and topology pattern
specifiers (`event connections instance events`), so anchor on what
follows the name. Patterns are `grep -nE` over `*.fpp` and `*.fppi`.

| Want | Pattern |
|---|---|
| Input ports | `^\s*(async|sync|guarded) input port` |
| Output ports | `^\s*output port` |
| Commands | `^\s*(async|sync|guarded) command` |
| Events | `^\s*event +\$?\w+\s*(\(|\\|severity|$)` |
| Telemetry | `^\s*telemetry +\$?\w+\s*:` |
| Parameters | `^\s*(external )?param +\$?\w+\s*:` |
| Special ports | `^\s*((async|sync|guarded) )?(command (recv|reg|resp)|event|text event|telemetry|time get|param (get|set)|product (get|request|send|recv)) port` |
| Internal ports | `^\s*internal port` |
| Interfaces used | `^\s*import ` in a component file |
| Everything in a component | `^\s*(async|sync|guarded|output|internal|command|text|time|external|param|product|event|telemetry|import|include|match|state machine)\b` |

The events pattern accepts the `event Name \` line-continuation form.
The same keywords in a `topology.fpp` are pattern specifiers and
`instance`/`connections` lines, not declarations.

Port arrays are declared as `port name: [N] Type`; `N` is usually a
constant from `default/config/AcConstants.fpp` (or the project's
override), so follow the name there to get the number.

Across the tree (exclude `build-fprime-*` directories):

- Who uses a port type: `grep -rlE 'port +\$?\w+ *:.*\bPing\b' --include=*.fpp --include=*.fppi`.
  Inside `module Svc` the type is written unqualified (`Ping`, not
  `Svc.Ping`), and components that get the port through an interface
  only show `import <Module>.<Interface>`; grep for that too.
- Where a symbol is defined: `grep -nE '^locate \w+ Svc\.Ping ' build-fprime-*/locs.fpp`
  (needs a build cache). `fpp-locate-defs <file>` lists what *that
  file* defines, not where a used symbol lives.
- Who is wired to an instance: grep the instance name across every
  `.fpp` of the deployment and of the subtopologies it imports
  (`Svc/Subtopologies/*/*.fpp`, under `lib/fprime/` in a project).
  Look inside `connections` blocks **and** at the pattern specifiers
  (`command connections instance CdhCore.cmdDisp`, `event connections
  instance CdhCore.events`, `health connections instance
  CdhCore.$health`, ...). A pattern connects every instance in the
  topology, including the named one itself. Subtopologies themselves
  carry no pattern specifiers.

---

## 2. Name mapping (model → generated C++)

`<Comp>` is the FPP component name, not the directory
(`Svc/CmdDispatcher/` declares `active component CommandDispatcher`,
so the base is `CommandDispatcherComponentBase`). `<Comp>TesterBase` /
`<Comp>GTestBase` are the unit-test counterparts. Enum constants
upper-case the model name: event `OpcodeRegistered` is
`EVENTID_OPCODEREGISTERED`, command `CMD_NO_OP` is `OPCODE_CMD_NO_OP`.
Full signatures and semantics:
[Autocoded Functions and Component Classes](https://github.com/nasa/fprime/blob/devel/docs/user-manual/framework/autocoded-functions.md).

| Model | Implementation must define | Implementation may call | Tester side |
|---|---|---|---|
| input port `foo` | `foo_handler(FwIndexType portNum, ...)`, returning the port's return type if it has one; async ports may add `foo_preMsgHook`, and `hook` ports must add `foo_overflowHook(portNum, ...)` | | `invoke_to_foo(portNum, ...)`, `connect_to_foo`, `isConnected_to_foo`; `doDispatch()` drains an async queue |
| output port `bar` | | `bar_out(portNum, ...)` (asserts on an unconnected or out-of-range port), `isConnected_bar_OutputPort(portNum)`, `getNum_bar_OutputPorts()` | `from_bar_handler(...)` (returns the port's return type), `pushFromPortEntry_bar(...)`, `ASSERT_from_bar_SIZE(n)`, `ASSERT_from_bar(idx, ...)` |
| serial port `s` | `s_handler(FwIndexType portNum, Fw::LinearBufferBase&)` | `s_out(portNum, buffer)` | `from_s_handler(portNum, buffer)` |
| command `CMD` | `CMD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, args...)`; async commands may add `CMD_preMsgHook(opCode, cmdSeq)` | `cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse)`, enum `OPCODE_CMD` | `sendCmd_CMD(instance, cmdSeq, args...)`, `ASSERT_CMD_RESPONSE_SIZE(n)`, `ASSERT_CMD_RESPONSE(idx, opcode, seq, resp)` |
| event `E` (severity `warning high`) | | `log_WARNING_HI_E(args...)`, enum `EVENTID_E`. Severities spell `FATAL`, `WARNING_HI`, `WARNING_LO`, `COMMAND`, `ACTIVITY_HI`, `ACTIVITY_LO`, `DIAGNOSTIC`. `throttle N` adds `EVENTID_E_THROTTLE` and `log_WARNING_HI_E_ThrottleClear()` | `ASSERT_EVENTS_E_SIZE(n)`, `ASSERT_EVENTS_E(idx, args...)` (only for events with arguments), `ASSERT_EVENTS_SIZE(n)` |
| telemetry `T` | | `tlmWrite_T(value, timeTag = ...)`, enum `CHANNELID_T` | `ASSERT_TLM_T_SIZE(n)`, `ASSERT_TLM_T(idx, value)`, `ASSERT_TLM_SIZE(n)` |
| param `P` | optional `parameterUpdated(FwPrmIdType id)`; `external param` needs `registerExternalParameters(Fw::ParamExternalDelegate*)` at init | `paramGet_P(Fw::ParamValid& valid)`, enum `PARAMID_P`. Each param also autocodes commands `P_SET`/`P_SAVE` (`OPCODE_P_SET`, `OPCODE_P_SAVE`; `P_PRM_SET` in the dictionary), numbered before the model's own commands | `paramSet_P(value, valid)`, `paramSend_P(instance, cmdSeq)`, `paramSave_P(instance, cmdSeq)` |
| product container `C`, record `R` | `dpRecv_C_handler(DpContainer&, Fw::Success::T)` only with a `product recv port` | `dpGet_C(size, container)`, `dpRequest_C(size)`, `dpSend(container)`, `container.serializeRecord_R(...)` | override `productGet_handler(FwDpIdType, FwSizeType, Fw::Buffer&)`; `ASSERT_PRODUCT_GET/REQUEST/SEND(...)` |
| internal port `ip` | `ip_internalInterfaceHandler(args...)`; `hook` adds `ip_overflowHook` | `ip_internalInterfaceInvoke(args...)` | |
| state machine instance `sm` of `SM` | `<Comp>_SM_action_<a>(SmId smId, <Ns>_SM::Signal signal, ...)` and the guards | `sm_sendSignal_<sig>(...)`, `sm_getState()` | |

Argument passing: primitives by value, enums/structs/arrays `const T&`,
`ref` arguments `T&`. Command string arguments are
`const Fw::CmdStringArg&`; event and port strings are
`const Fw::StringBase&` (`sendCmd_*` takes `const Fw::StringBase&`).
`FwIndexType portNum` is always first. Command handlers must end with
`cmdResponse_out`.

---

## 3. Where autocode lives, and when to read it

- The cache is `build-fprime-automatic-<toolchain>[-ut]/` at the
  project root named in `settings.ini` (`TestDeploymentsProject/` for
  `Ref`). Framework modules land under `F-Prime/<module path>/`;
  project and library modules under their path relative to the project
  root (`build-fprime-automatic-native/LedBlinker/Components/Led/`).
  `fprime-util generate` only writes `locs.fpp`; `*Ac.*` appear after
  `fprime-util build` (`--ut` for `TesterBase`/`GTestBase`). Never edit
  these files.
- `fprime-util impl` writes `<Comp>.template.{hpp,cpp}`; `--ut` writes
  `test/ut/<Comp>Tester.template.{hpp,cpp}` and
  `<Comp>TestMain.template.cpp`. Each needs the matching (ut or
  non-ut) cache, as does `fprime-util fpp-check`.
- Resolved opcodes, event/channel IDs and enum values are in
  `<Deployment>TopologyDictionary.json`, written by a normal deployment
  build under `build-artifacts/`; `fprime-util fpp-to-dict` from the
  `Top/` directory regenerates it. Use it for GDS and integration-test
  work rather than computing `base id` + local id by hand.
- `fprime-util generate` refuses to run when the installed `fpp` does
  not match `cmake/required.cmake`; build the venv from the
  framework's `requirements.txt`.
- `fpp-to-json` is **not** a shortcut: it emits 15–30× the size of the
  source file.

---

## 4. Codegen gotchas

- Command `string` arguments always autocode to `Fw::CmdStringArg`
  (`FW_CMD_STRING_MAX_SIZE`, default 40 in `default/config/FpConstants.fpp`)
  whatever `string size N` the model says; longer values fail
  deserialization with `FORMAT_ERROR`. Port and event string arguments
  honor their declared size.
- Pattern specifiers (`event connections instance X`, `telemetry
  connections instance X`, `command connections instance X`) include
  the instance `X` itself; a sink component's own events and telemetry
  are routed back into it.
- `log_*` and `tlmWrite_*` check `isConnected_*` before invoking, so an
  unconnected special port is silent; a plain `bar_out` on an
  unconnected port asserts.
- `event ... throttle N` stops emitting that event after `N` instances;
  a self-routed event under a pattern specifier recurses up to the
  throttle.
- Only `deployment topology X` autocodes `<X>TopologyAc.*`; a bare
  `topology` block (every subtopology) does not, and the dictionary
  also needs `register_fprime_deployment` in CMake. A scratch
  `fpp-to-cpp` run on a plain `topology` cannot check pattern wiring.
- Port numbers are compile-time constants: a port array of size `N`
  rejects `portNum >= N` with an assertion in the generated base class.
- Subtopology instances are addressed as `<Subtopology>.<instance>`
  in pattern specifiers (`CdhCore.cmdDisp`), but direct connections
  use the subtopology's exported ports,
  `<Subtopology>.Subtopology.<port>` (`CdhCore.Subtopology.tlmSendRun`),
  declared as aliases `port tlmSendRun = tlmSend.Run` in the
  subtopology. Subtopology instances are configured through
  `<Sub>Config/*.fpp` modules the project may override.
- A project `config/` directory shadows framework `.fpp`/`.hpp`
  configuration files by basename (`AcConstants.fpp`, `FpConstants.fpp`,
  `*Cfg.fpp`); check there before trusting the framework default.
