# Svc::ModeManager Component

## 1. Introduction

`Svc::ModeManager` is an active component that holds the spacecraft's current mode,
applies a configurable policy to transition requests, and notifies subscribing
components when the mode changes.

The component is deliberately mission-agnostic. The set of modes, the graph of
transitions between them, and the policy governing which transitions are permitted
all live in a configuration module that a project is expected to override. The
component itself supplies only the mechanism: request handling, provenance
stamping, policy dispatch, state tracking, and notification.

`Svc::ModeManager` performs **no fault management**. It accepts a request to enter
a mode; it never decides that a mode is warranted. Fault detection, isolation, and
response belong to other components, which request a mode change through the
`requestMode` port like any other requester.

The component also asserts **no default restrictions**. As shipped, every
transition is permitted. Supplying a policy — by overriding the configuration or
by connecting a policy component — is a required integration step. See
[§4.3](#43-default-policy).

## 2. Requirements

Requirement | Description | Verification Method
----------- | ----------- | -------------------
MM-001 | The `Svc::ModeManager` component shall maintain exactly one current mode at all times | Unit Test
MM-002 | The `Svc::ModeManager` component shall draw its set of modes from build-time project configuration | Inspection
MM-003 | The `Svc::ModeManager` component shall initialize to the initial state declared by the configured mode machine | Unit Test
MM-004 | The `Svc::ModeManager` component shall not invoke any output port, emit any event, or write any telemetry during its initial transition | Inspection
MM-005 | The `Svc::ModeManager` component shall apply a transition policy to every request, determining whether the requested transition is permitted | Unit Test
MM-006 | The `Svc::ModeManager` component shall obtain the policy from the `checkTransition` port when that port is connected, and from the configured default policy otherwise | Unit Test
MM-007 | The `Svc::ModeManager` component shall change mode only in response to a request, never autonomously | Inspection, Unit Test
MM-008 | The `Svc::ModeManager` component shall send a transition signal to the mode machine only after the active policy has permitted the transition | Unit Test
MM-009 | The `Svc::ModeManager` component shall accept transition requests from ground via the `REQUEST_MODE` command | Unit Test
MM-010 | The `Svc::ModeManager` component shall accept transition requests from other components via the `requestMode` port array | Unit Test
MM-011 | The `Svc::ModeManager` component shall determine requester provenance from the entry point used, and shall not accept provenance supplied by the caller | Inspection, Unit Test
MM-012 | The `Svc::ModeManager` component shall identify a component requester by the index of the `requestMode` port on which the request arrived | Unit Test
MM-013 | The `Svc::ModeManager` component shall leave the current mode unchanged when the active policy denies a request | Unit Test
MM-014 | The `Svc::ModeManager` component shall emit a `TransitionRejected` event on every denied request, identifying the current mode, the requested mode, the requester, and which policy decided | Unit Test
MM-015 | The `Svc::ModeManager` component shall notify every connected `modeChanged` subscriber on each mode change, reporting the previous and new mode | Unit Test
MM-016 | The `Svc::ModeManager` component shall emit a `ModeTransitioned` event on each mode change, identifying the previous mode, the new mode, and the requester | Unit Test
MM-017 | The `Svc::ModeManager` component shall report the current mode as telemetry on each mode change | Unit Test
MM-018 | The `Svc::ModeManager` component shall return the current mode on the `getMode` port | Unit Test
MM-019 | The `Svc::ModeManager` component shall perform no fault detection, fault isolation, or fault response | Inspection
MM-020 | The `Svc::ModeManager` component shall respond to health pings | Unit Test
MM-021 | The `Svc::ModeManager` component shall reject a request arriving on `requestMode` port index 0 without constructing a `ModeRequest` and without stamping it as originating from ground | Unit Test

## 3. Design

### 3.1 Context

#### 3.1.1 Ports

Port Data Type | Name | Direction | Kind | Usage
-------------- | ---- | --------- | ---- | -----
`Svc::ModeRequestPort` | requestMode | Input | Asynchronous | Transition request from another component. The port index identifies the requester. Index 0 is reserved and always rejected (MM-021); real requesters occupy indices 1 through `NUM_REQUESTERS - 1`.
`Svc::GetMode` | getMode | Input | Guarded | Return the current mode to the caller. Guarded because it executes in the caller's thread.
[`Svc::Ping`](../../Ping/docs/sdd.md) | pingIn | Input | Asynchronous | Health check request
`Svc::ModeChanged` | modeChanged | Output | n/a | Notify a subscriber that the mode changed
`Svc::CheckTransition` | checkTransition | Output | n/a | Optional external policy query. When unconnected, the default policy decides.
[`Svc::Ping`](../../Ping/docs/sdd.md) | pingOut | Output | n/a | Health check response

#### 3.1.2 Commands

Command | Description
------- | -----------
`START` | Leave the initial state and begin operations
`REQUEST_MODE` | Request a transition to the given mode

#### 3.1.3 Telemetry

Channel | Type | Description
------- | ---- | -----------
`CurrentMode` | `Svc::Mode` | The current spacecraft mode, written on each change

> The channel is written only when the mode changes. `Svc::TlmPacketizer`
> re-sends stored packet values on its own cadence, so no periodic rewrite is
> needed. Under `Svc::TlmChan`, which downlinks only channels updated since the
> last cycle, the mode would be seen once per transition — deployments using
> TlmChan should account for that.

#### 3.1.4 Events

Event | Severity | Description
----- | -------- | -----------
`ModeTransitioned` | ACTIVITY_HI | A mode transition completed
`TransitionRejected` | WARNING_LO | A transition request was denied by the active policy, or arrived on the reserved `requestMode` port index 0

### 3.2 Functional Description

A transition request arrives either as a `REQUEST_MODE` command or as a call on
`requestMode[n]`. In both cases the component constructs the `Svc::ModeRequest`
itself: the command path stamps `GROUND`, the port path stamps `COMPONENT` and
the requester corresponding to port index `n`. A caller cannot supply or forge
its own provenance (MM-011).

Port index 0 is the one exception: since `Requester` value 0 (`NONE`) means
"this came from ground," stamping a component request with it would produce
an event that misrepresents its own source. A call on `requestMode[0]` is
rejected immediately, before any `ModeRequest` is constructed, via the same
`TransitionRejected` path any other denial uses (MM-021). It is deliberately
*not* relabeled as `GROUND`: doing so would let a component wired to port 0
bypass a project's ground-only policy rules, defeating the purpose MM-011
exists for. The resulting event label (`COMPONENT`/`NONE`) is imperfect, but
the request is never granted trust, which is what matters.

The component then asks the active policy whether the transition is permitted.
If `checkTransition` is connected, the external policy decides; otherwise the
default policy in the configuration module decides (MM-006).

On denial, the mode is unchanged and `TransitionRejected` is emitted naming the
deciding policy, so an operator can tell which of the two paths ruled (MM-013,
MM-014).

On approval, the component records the outgoing mode, sends the corresponding
signal to the mode machine, and the machine's `announce` action updates the
current mode, writes telemetry, emits `ModeTransitioned`, and calls every
connected `modeChanged` port (MM-015 through MM-017).

### 3.3 State

The current mode is held both by the generated state machine and by a component
member. The member is the value reported on `getMode`, telemetry, and events; it
is updated inside the `announce` action. This avoids any dependence on when the
generated state variable is updated relative to transition actions.

The mode machine enforces no policy of its own — every state accepts every
signal. All legality decisions are made before a signal is ever sent. A missing
clause in the machine would silently drop an approved transition rather than
reject it, which is why the shipped machine is fully permissive and why an
override must remain so.

## 4. Configuration

The configuration module is `config/ModeManagerConfig`. A project overrides it by
creating files of the same names and registering them with
`register_fprime_config(... CONFIGURATION_OVERRIDES ...)`.

File | Contents
---- | --------
`ModeManagerCfg.fpp` | `Mode`, `ModeRequestSource`, `Requester`, `ModeRequest`, `ModePolicySource`, sizing constants
`ModeMachine.fpp` | The mode machine: states, signals, and the `announce` action
`ModeManagerCfg.hpp` | The default transition policy
`ModeList.inc` | The mode-to-signal dispatch list

### 4.1 Override contract

The shipped C++ is compiled against names declared in the configuration module.
None of the following are checked by the FPP compiler; violating one produces a
link error rather than a diagnostic.

Requirement on the override | Reason
--------------------------- | ------
A state machine named exactly `ModeMachine` | `ModeManager.fpp` instantiates that name
An action named exactly `announce`, taking `Svc.ModeRequest` | The shipped implementation defines `Svc_ModeMachine_action_announce`
One state per member of `Svc.Mode` | The enum and the graph must agree
One `MODE_ENTRY` line in `ModeList.inc` per requestable mode | Drives the mode-to-signal dispatch
Each `Requester` enum value equal to that component's `requestMode` port index | Provenance is derived from the port index
`Requester::NONE` equal to 0, with no other member equal to 0 | Port index 0 is unconditionally rejected (MM-021); a project that assigns a real requester the value 0 makes that requester permanently unreachable
Every state accepting every signal | See §3.3

### 4.2 Sizing

Constant | Default | Meaning
-------- | ------- | -------
`ModeManagerCfg.NUM_REQUESTERS` | 4 | Size of the `requestMode` port array
`ModeManagerCfg.NUM_SUBSCRIBERS` | 8 | Size of the `modeChanged` port array

### 4.3 Default policy

**The shipped default permits every transition.** With nothing connected to
`checkTransition` and no configuration override in place, `Svc::ModeManager` will
move between any two modes on request from any requester. That includes:

- entering SCIENCE directly from SAFE
- a component taking the vehicle out of SAFE with no ground involvement
- leaving the initial state without a `START` command

This is deliberate. The component supplies mechanism; it asserts no policy of its
own and makes no safety guarantees about which transitions are reachable. Every
restriction is a project responsibility.

A deployment imposes restrictions in one of two ways:

1. Override `ModeManagerCfg.hpp` with a policy function of its own — no additional
   component required
2. Connect a policy component to `checkTransition`, which takes precedence over
   the default whenever it is connected

> **Integration note.** Supplying a policy is a required integration step, not an
> optional one. The shipped default exists so the component builds and runs; it is
> not a safe operating configuration. A deployment that connects nothing and
> overrides nothing has an unrestricted mode manager.

Because the default denies nothing, `TransitionRejected` cannot fire in the shipped
configuration. It becomes reachable only once a project supplies a restricting
policy.

## 5. Unit Testing

Rule-based testing (`fprime-unit-testing.skill.md` section 3), chosen over
simple `TEST()` cases because the component is stateful (current mode gates
what is legal next) and has several interacting ports and commands.

The tester's `connectPorts()` is hand-written rather than autocoder-generated
so that `checkTransition` can be left unconnected on purpose, exercising the
shipped default policy (permit everything). A separate constructor argument
connects it for tests of an external policy.

Rule groups:

Group | Exercises | Requirements
----- | --------- | ------------
`Start` | `START` command | MM-003, MM-004, MM-008, MM-009
`RequestModeCmd` | `REQUEST_MODE` command, every reachable mode | MM-001, MM-005, MM-007, MM-009, MM-015 to MM-017
`RequestModeCmd.UnsupportedTarget` | `REQUEST_MODE(STARTUP)` -- rejected before any policy is consulted | MM-005
`RequestModePort` | `requestMode[n]`, every reachable mode and requester | MM-001, MM-005, MM-007, MM-010 to MM-012, MM-015 to MM-017
`RequestModePort.ReservedPort` | `requestMode[0]` -- rejected via `TransitionRejected` before a `ModeRequest` is constructed, never stamped as ground | MM-021
`GetMode` | `getMode` query | MM-018
`Ping` | Health ping round trip | MM-020
`ExternalPolicy` | `checkTransition` connected: permits, denies, and denies a transition the default would always allow (proving precedence, not mere consultation) | MM-005, MM-006, MM-013, MM-014

A random scenario (`ModeManager.RandomizedTesting`) applies every mechanism
rule above (excluding `ExternalPolicy`, which requires a differently-wired
tester) in random order for 10,000 steps. This is where sequence-dependent
bugs surface -- a stale `fromMode` in `ModeTransitioned`, a `modeChanged`
broadcast that skips a subscriber, provenance bleeding between the ground and
component request paths -- none of which depend on the policy ever denying
anything, since the shipped default permits everything.

**Dispatch counts are exact, not drained.** The generated `doDispatch()`
calls `Os::Queue::receive()` with `BLOCKING`, so there is no way to "dispatch
until empty" from a test -- the call after the last real message hangs
forever rather than returning an empty-queue status. Each rule instead calls
`dispatchExactly(n)`, where `n` is 2 for an approved transition (the
command/port call, then the signal it sends to the mode machine) and 1 for a
denied one or an unrelated call like `pingIn` (see the header comment on
`dispatchExactly()`).

Coverage: 97.6% lines, 100% functions. The two uncovered lines are the
`FW_ASSERT` in `sendTransitionSignal()` -- unreachable by design, since
`isRequestableMode()` filters every caller before it, and reaching it would
require bypassing that guard rather than exercising real behavior.

## 6. Change Log

Date | Description
---- | -----------
2026-07-29 | Initial draft
2026-08-03 | Reserved `requestMode` port index 0 (MM-021): rejected via the existing `TransitionRejected` event, never stamped as ground
