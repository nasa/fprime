---
name: fprime-topology-development
description: >-
  Develop and evolve an F Prime deployment topology: FPP instance
  definitions, base IDs, queue/stack/priority, connection graphs, rate
  group scheduling, phased initialization, subtopologies, and wiring a
  new or changed component into an existing deployment. Use whenever
  a task touches a `Top/` directory, `instances.fpp`, `topology.fpp`,
  a `*TopologyDefs.hpp`, or a subtopology. Keywords: F Prime, topology,
  deployment, instance, base id, connections, rate group, subtopology,
  phases, ping entries.
---

# Skill: F Prime Topology Development

Covers everything between "a component exists" and "a deployment runs".
Most work here extends an existing topology rather than creating one
from scratch. Component-level work belongs to
`fprime-component-development`; CMake and build-cache mechanics belong
to `fprime-cmake-build-system`.

Canonical references:
[building the topology](../../../docs/user-manual/framework/building-topology.md),
[port/component/topology overview](../../../docs/user-manual/overview/03-port-comp-top.md),
[autocoded functions](../../../docs/user-manual/framework/autocoded-functions.md).
Working exemplar: [`TestDeploymentsProject/Ref/Top/`](../../../TestDeploymentsProject/Ref/Top).

---

## 1 — Anatomy of a `Top/` directory

| File | Holds |
|---|---|
| `instances.fpp` | Instance *definitions*: type, base id, queue/stack/priority/cpu |
| `topology.fpp` | Topology membership, pattern graphs, connection graphs, packet include |
| `system.fpp` | `system <Name>: <Topology>` — the deployment entry point |
| `<Dep>Packets.fppi` | `telemetry packets` + `omit { ... }` list |
| `<Dep>TopologyDefs.hpp` | `TopologyState` struct, `PingEntries` constants |
| `<Dep>Topology.cpp` | `setupTopology` / `teardownTopology`, project-specific `configureTopology` |

Definition and membership are separate: an instance defined in
`instances.fpp` only joins the topology when named by `instance <name>`
inside `deployment topology <Name> { ... }`. Defined-but-unused
instances are legal and are neither constructed nor started.

## 2 — Instance definitions

- Passive: `instance posixTime: Svc.PosixTime base id 0x10020000`
- Queued: add `queue size ...`
- Active: add `queue size ...`, `stack size ...`, `priority ...`, and
  `cpu ...` where affinity matters.

Base IDs must be unique and non-overlapping across the whole
deployment, including subtopologies. `Ref` uses `0xDSSCCxxx`
(deployment / subtopology / component / reserved low 12 bits for the
instance's own events, commands, channels) — see the header comment in
[`instances.fpp`](../../../TestDeploymentsProject/Ref/Top/instances.fpp).
Subtopologies offset from a configurable `BASE_ID` constant.

Thread priorities, stack sizes and affinities are project- and
platform-specific; take them from the project's own conventions rather
than from a generic default.

## 3 — Connections

**Pattern graphs** — one specifier per service wires the whole
deployment's command, event, telemetry, text-event, parameter, time,
and health ports, each naming the serving instance, e.g.
`telemetry connections instance CdhCore.tlmSend`. Prefer these over
hand-wiring service ports.

**Direct graphs** — named blocks `connections <Name> { a.out -> b.in }`.
Group by subsystem or by instance pair; port arrays are indexed, and
indices should come from an FPP `enum` (e.g. `Ports_RateGroups`) rather
than bare integers.

**Matched ports** — `match <a> with <b>` in the component model forces
parallel arrays to be connected at identical indices; FPP errors when
they are not. See
[common port patterns](../../../docs/user-manual/design-patterns/common-port-patterns.md).

**Rate group scheduling** — a timer drives
`rateGroupDriverComp.CycleIn`; each `CycleOut[<group>]` drives one
`ActiveRateGroup.CycleIn`, whose `RateGroupMemberOut[<slot>]` ports call
each member's `schedIn`-style port synchronously, in slot order. Rates
come from the `DividerSet` passed to `RateGroupDriver::configure` in the
topology `.cpp`. See
[rate groups](../../../docs/user-manual/design-patterns/rate-group.md).

**Unconnected output ports assert when invoked.** A port that is
optional by design can be guarded with
`isConnected_<port>_OutputPort(portNum)` in the C++ implementation.

## 4 — Phased initialization

`setupTopology` calls autocoded functions in a fixed order:
`initComponents` → `setBaseIds` → `connectComponents` →
`configComponents` → project `configureTopology` → `regCommands` →
`loadParameters` → `startTasks`. Teardown reverses it:
`stopTasks` → `freeThreads` → `tearDownComponents` → `deinitComponents`.
Never start tasks or register commands before connections exist.

Per-instance C++ can be attached in FPP instead of the `.cpp`, via init
specifiers on an instance body:

```
instance $health: Svc.Health base id CdhCoreConfig.BASE_ID + 0x002000 \
  queue size CdhCoreConfig.QueueSizes.$health \
{
  phase Fpp.ToCpp.Phases.configComponents """
  CdhCore::health.setPingEntries(...);
  """
}
```

Phases are mandatory inside subtopologies; deployments may use either.
Phase list and semantics:
[subtopology how-to](../../../docs/how-to/develop/develop-subtopologies.md).

## 5 — Subtopologies

Use one when a group of instances is only meaningful together and is
reused across deployments (comms stack, C&DH core, data products).
Do not create one for a single component.

Consume an F Prime core subtopology by instantiating it and wiring its
declared topology ports (`CdhCore.Subtopology.tlmSendRun`), not the
inner instances; a locally developed subtopology may instead be
`import`ed and its instances referenced qualified
(`MySubtopology.rateGroup.CycleIn`). Its `PingEntries` live in
`GlobalDefs::PingEntries` and its `SubtopologyTopologyDefs.hpp` must be
included by the deployment's `TopologyDefs.hpp`. Configuration is
overridden with a config module.
See [subtopologies](../../../docs/user-manual/design-patterns/subtopologies.md)
and [develop a subtopology](../../../docs/how-to/develop/develop-subtopologies.md).

## 6 — Wiring a new component into an existing deployment

1. Define the instance in `instances.fpp` with a fresh base ID.
2. Add `instance <name>` to the topology.
3. Connect its functional ports in a `connections` block; attach
   `schedIn`-style ports to the appropriate rate group slot.
4. Health: for an active instance with `Svc.Ping` ports, add
   `PingEntries::<Module>_<instance> { WARN, FATAL }` to
   `<Dep>TopologyDefs.hpp`. See
   [health checking](../../../docs/user-manual/design-patterns/health-checking.md).
5. Telemetry: list every new channel in a packet in `*Packets.fppi` or
   in the trailing `omit { ... }` block.
6. Project-specific setup (allocators, `configure()` calls) in
   `configureTopology` or a `configComponents` phase.
7. Register the new module and regenerate — see
   `fprime-cmake-build-system`. Adding an FPP file to `Top/` also
   requires it in the topology module's `AUTOCODER_INPUTS`.

## 7 — Failure modes

| Symptom | Cause |
|---|---|
| Overlapping dictionary IDs, GDS shows wrong item | Colliding/overlapping base IDs, often a subtopology `BASE_ID` reused |
| Assert on first port invocation | Required port never connected |
| Instance silently absent at runtime | Defined in `instances.fpp` but never added to the topology |
| Active instance has no thread / FPP error | Active instance missing `queue size`, `stack size`, or `priority` |
| Stale dictionary or "unknown instance" build error | Topology not regenerated after FPP edits |
| `PingLateWarnings`, spurious FATAL | Missing `PingEntries` entry, or ping ports not covered by `health connections` |
| Rate group slot index out of range | More members than the `ActiveRateGroup` output port array, or a `DividerSet` entry missing for a rate group |
| Rate group slip `WARNING_HI` | Too much synchronous work in one cycle — move members to a slower group or make them active |
