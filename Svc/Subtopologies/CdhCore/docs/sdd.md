# CdhCore Subtopology — Software Design Document (SDD)

The **CdhCore subtopology** provides a reusable bundle of the core flight-software services that nearly every F´ deployment requires. It collects standard components for **command dispatching**, **event handling**, **health monitoring**, **version/configuration reporting**, **logging**, and **assert/fatal adaptation** into a single subtopology that can be instantiated in a larger system topology. By doing so, it reduces boilerplate wiring and ensures consistent configuration of these essential services. The subtopology also defines **configurable instances** for telemetry sending and fatal-event handling, allowing integration engineers to connect these core services to project-specific implementations while reusing the common backbone of CDH (Command and Data Handling) functions.

## 1. Requirements

| ID              | Description                                                                                                                   | Validation |
| --------------- | ----------------------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-CDHCORE-001 | The subtopology shall provide **command dispatching functionality** to route incoming commands to components.                 | Inspection |
| SVC-CDHCORE-002 | The subtopology shall provide **event management functionality** to collect events.                                           | Inspection |
| SVC-CDHCORE-003 | The subtopology shall provide **health monitoring functionality** including for active components.                            | Inspection |
| SVC-CDHCORE-004 | The subtopology shall provide **version and configuration reporting functionality** at system startup.                        | Inspection |
| SVC-CDHCORE-005 | The subtopology shall provide **text logging functionality** for console logged event messages.                               | Inspection |
| SVC-CDHCORE-006 | The subtopology shall provide **assert to fatal handling functionality** to convert `FW_ASSERT`s into `FATAL` events.         | Inspection |
| SVC-CDHCORE-007 | The subtopology shall provide **fatal-event routing functionality** to forward fatal announcements to a configurable handler. | Inspection |
| SVC-CDHCORE-008 | The subtopology shall provide **telemetry sending functionality** through a configurable telemetry interface.                 | Inspection |
| SVC-CDHCORE-009 | The subtopology shall support **configurable instance properties** for IDs, queue sizes, stack sizes, and priorities.         | Inspection |
| SVC-CDHCORE-010 | The subtopology shall expose rate-group connection points for any rate-drive components it contains.                          | Inspection |

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name  | Type (Svc)           | Kind    | Purpose (core function)                                                                | Handles                |
| -------------- | -------------------- | ------- | -------------------------------------------------------------------------------------- |------------------------|
| `cmdDisp`      | `CommandDispatcher`  | Active  | Receives uplinked/issued commands and dispatches them.                                 | command connections    |
| `events`       | `EventManager`       | Active  | Aggregates and distributes events; `FatalAnnounce` is wired to `fatalHandler`.         | event connections      |
| `$health`      | `Health`             | Queued  | Performs health pings; configured with ping table.                                     | health connections     |
| `version`      | `Version`            | Passive | Provides version reporting; configured for startup reporting.                          |                        |
| `textLogger`   | `PassiveTextLogger`  | Passive | Text event logging sink.                                                               | text event connections |
| `fatalAdapter` | `AssertFatalAdapter` | Passive | Adapts framework `FW_ASSERT`s into `FATAL`s behavior.                                  |                        |
| `tlmSend`      | *(configurable)*     | —       | Telemetry send instance provided via configuration.                                    | telemetry connections  |
| `fatalHandler` | *(configurable)*     | —       | Fatal handler instance provided via configuration.                                     |                        |

### 2.2 Configuration Hooks inside the Subtopology

* **Health**: Configures `$health` with a ping table (`ConfigObjects.CdhCore_health::pingEntries`).
* **Version**: Calls `version.config(true)` so version/config reporting is enabled at startup.

### 2.3 Internal Wiring

* **Fault protection path**: `events.FatalAnnounce -> fatalHandler.FatalReceive` is wired.

All other services are registered in the calling topology.

### 2.4 Required Inputs for Operation

The `CdhCore` subtopology is not a stand-alone application. It requires specific connections from the including deployment topology to operate correctly:

* **Rate Groups**

  * The active components instances (`$health` and possibly `tlmSend` depending on configuration) require scheduling via rate group ports.
  * The including topology must connect them to rate group driver outputs or equivalent schedulers.

* **Configurable Instances**

  * `tlmSend`: A project-specific telemetry sender instance that handles downlinking channelized telemetry.
  * `fatalHandler`: A project-specific handler that processes fatal events (for example, shutting down the system or resetting hardware).

Without these inputs, the subtopology cannot schedule, report, or properly propagate telemetry and fatal events.

### 2.5 Limitations

The `CdhCore` subtopology is focused on core CDH services and does **not** include the following functions:

* **Uplink handling** (command reception from ground)
* **Downlink handling** (telemetry transport to ground)
* **Framing or deframing** of communications data (e.g., CCSDS, F´ framing)
* **File management services** (uplink, downlink, or storage)

These capabilities must be provided by other subtopologies or components (e.g., `ComCcsds`, `FramingFprime`, file services) and wired alongside `CdhCore` in the deployment topology.

## 3. Usage

The **CdhCore subtopology** is included in a deployment to provide core CDH services—command dispatching, event handling, health monitoring, version/configuration reporting, logging, and fatal handling—through clearly defined **connection specifiers**. These specifiers expose the necessary ports so the deployment topology can attach external infrastructure such as radios or storage. In practice, `CdhCore` is typically wired into a **communication or framing stack** (e.g., `ComCcsds`, `ComFprime`, `FramingFprime`, or `FramingCcsds`) to complete the end-to-end command and telemetry paths.

### 3.1 Example Usage

Example of integrating `CdhCore` in a deployment topology:

```fpp
topology Flight {
  import CdhCore.Subtopology
  import ComCcsds.Subtopology  # Used as an example communication subtopology

  # Use CdhCore handler components
  command connections instance CdhCore.cmdDisp
  event connections instance CdhCore.events
  telemetry connections instance CdhCore.tlmSend
  text event connections instance CdhCore.textLogger
  health connections instance CdhCore.$health

  connections RateGroups {
    # Connect rate groups to active components inside CdhCore
    rg.RateGroupMemberOut[0] -> CdhCore.tlmSend.Run
    rg.RateGroupMemberOut[1] -> CdhCore.$health.Run
  }

  connections ComCcsds_CdhCore{
      # events and telemetry to comQueue
      CdhCore.events.PktSend        -> ComCcsds.comQueue.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.EVENTS]
      CdhCore.tlmSend.PktSend       -> ComCcsds.comQueue.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.TELEMETRY]

      # Router <-> CmdDispatcher
      ComCcsds.fprimeRouter.commandOut  -> CdhCore.cmdDisp.seqCmdBuff
      CdhCore.cmdDisp.seqCmdStatus     -> ComCcsds.fprimeRouter.cmdResponseIn
      cmdSeq.comCmdOut -> CdhCore.cmdDisp.seqCmdBuff
      CdhCore.cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
  }
}
```

> [!TIP]
> `ComCcsds` was used in the example wiring, but may be replace by another subtopology meeting the same function.

### 3.2 Integration Notes

* The **rate group driver** is project-specific. You may use a single driver with multiple cycle outputs (as above), or separate drivers for each component depending on timing needs.
* The **ping table** used by the health component must be configured to include the active components you want monitored in your system.
* The **fatal handler** should implement logic appropriate for your platform (e.g., log to NVM, trigger a reset).
* The **telemetry sender** should be wired to your communication link, downlink storage, or other system-level data channel.

## 4. Configuration

> This section summarizes the **knobs defined in the CdhCore configuration module**.

### 4.1 Where it lives

* Module: `Svc/Subtopologies/CdhCore/CdhCoreConfig/`
* Files (focus of this section):

  1. `CdhCoreConfig.fpp` — component properties
  2. `CdhCoreFatalHandlerConfig.fpp` — fatal-event handling instance
  3. `CdhCoreTlmConfig.fpp` — telemetry instance

### 4.2 Component properties (`CdhCoreConfig.fpp`)

* **Base ID** — base id for the subtopology, component will be offset from this.
* **Queue sizes** — component queue depths (for active and queued components).
* **Stack sizes** — task stack allocation (for active).
* **Priorities** — RTOS scheduling priority (for active).

> These govern footprint and responsiveness of: command dispatching, event management, health monitoring, text logging, version reporting, and assert→fatal adaptation. Override to meet platform timing and memory constraints.

### 4.3 Fatal-event handling (`CdhCoreFatalHandlerConfig.fpp`)

Defines a component instance called `fatalHandler` to handle `FATAL` events produced by the `eventManager`.

### 4.4 Telemetry path (`CdhCoreTlmConfig.fpp`)

Defines a component instance called `tlmSend` to handle telemetry.

## 5. Traceability Matrix

| Requirement ID  | Satisfied by                                                       |
| --------------- | ------------------------------------------------------------------ |
| SVC-CDHCORE-001 | `cmdDisp` — `Svc.CommandDispatcher`                                |
| SVC-CDHCORE-002 | `events` — `Svc.EventManager`                                      |
| SVC-CDHCORE-003 | `$health` — `Svc.Health`                                           |
| SVC-CDHCORE-004 | `version` — `Svc.Version`                                          |
| SVC-CDHCORE-005 | `textLogger` — `Svc.PassiveTextLogger`                             |
| SVC-CDHCORE-006 | `fatalAdapter` — `Svc.AssertFatalAdapter`                          |
| SVC-CDHCORE-007 | `fatalHandler` — configurable instance                             |
| SVC-CDHCORE-008 | `tlmSend` — configurable instance                                  |
| SVC-CDHCORE-009 | `CdhCoreConfig` / `CdhCoreFatalHandlerConfig` / `CdhCoreTlmConfig` |
| SVC-CDHCORE-010 | $health.Run |
