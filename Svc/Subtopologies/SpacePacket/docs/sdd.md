# SpacePacket Subtopology — Software Design Document (SDD)

The **SpacePacket subtopologies** implement the F´ **CCSDS Space Packet** layer of the communications stack: router, ComQueue, buffer manager, Space Packet framer/deframer, APID manager, and aggregator — **without** any transfer-frame (TM/TC) layer. There are **two variants** in the same module:

1. A variant that **supplies a `Svc::ComStub`** implementation of `Svc.ComInterface` and expects to be wired to a **`Drv::ByteStreamDriverModel`** (TCP/UDP/UART, etc.), and
2. A variant that **expects a downstream layer** — either a transfer-frame subtopology (e.g., `CcsdsFraming`) or an external implementation of [`Svc.ComInterface`](https://fprime.jpl.nasa.gov/latest/docs/reference/communication-adapter-interface/) provided by the deployment.

> [!IMPORTANT]
> `SpacePacket` provides framing/deframing of F´ data into CCSDS Space Packets only. Compose it with a transfer-frame subtopology (e.g., `CcsdsFraming`, as done by `ComCcsds`) for a full CCSDS TM/TC stack.

---

## 1. Requirements

| ID                  | Description                                                                                                       | Validation |
| ------------------- | ----------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-SPACEPACKET-001 | Provide a **CCSDS Space Packet framer** to convert COM buffers into Space Packets for downlink.                   | Inspection |
| SVC-SPACEPACKET-002 | Provide a **CCSDS Space Packet deframing path** to parse incoming Space Packets into COM buffers for uplink.      | Inspection |
| SVC-SPACEPACKET-003 | Provide an F´ **router** to route deframed packets (e.g., commands/files) into the flight software.               | Inspection |
| SVC-SPACEPACKET-004 | Provide a **subtopology variant that supplies `Svc::ComStub`** designed to connect to a ByteStream driver.        | Inspection |
| SVC-SPACEPACKET-005 | Provide a **subtopology variant that expects a downstream layer** (framing subtopology or `Svc::ComInterface`).   | Inspection |
| SVC-SPACEPACKET-006 | Support **configurable instance properties** (IDs, queue sizes, stack sizes, priorities) via `SpacePacketConfig`. | Inspection |

---

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name         | Type (Svc/Drv)                  | Kind    | Purpose (core function)                                                                         |
| --------------------- | ------------------------------- | ------- | ----------------------------------------------------------------------------------------------- |
| `fprimeRouter`        | `Svc.FprimeRouter`              | Passive | Routes deframed packets (e.g., commands/files) into the flight software.                        |
| `comQueue`            | `Svc.ComQueue`                  | Active  | Queues categorized COM data for framing (telemetry, events, file, etc.); exposes `run`.         |
| `commsBufferManager`  | `Svc.BufferManager`             | Passive | Provides buffer allocation for the comms stack.                                                 |
| `spacePacketFramer`   | `Svc.Ccsds.SpacePacketFramer`   | Passive | Builds **CCSDS Space Packets** from COM buffers (downlink).                                     |
| `spacePacketDeframer` | `Svc.Ccsds.SpacePacketDeframer` | Passive | Deframes F´ data from **CCSDS Space Packets** (uplink).                                         |
| `apidManager`         | `Svc.Ccsds.ApidManager`         | Passive | Manages APID sequence counts for framing and validates them on deframing.                       |
| `aggregator`          | `Svc.ComAggregator`             | Active  | Aggregates space packets into larger buffers before handoff to the downstream layer.            |
| `comStub`             | `Svc.ComStub`                   | Passive | (Variant A only) Implementation of `Svc.ComInterface`, adapting a `Drv::ByteStreamDriverModel`. |

> **Two variants:**
> **A. “With ComStub” (`Subtopology`):** includes `Svc::ComStub` and exposes **ByteStream** ports to your driver.
> **B. “Framing only” (`FramingSubtopology`):** leaves the aggregator/deframer boundary ports **unconnected** for a downstream layer to attach to.

### 2.2 External Boundary (Variant B)

When importing `SpacePacket.FramingSubtopology`, the composer shall connect:

* Downlink: `SpacePacket.aggregator.dataOut -> [downstream].dataIn`, `[downstream].dataReturnOut -> SpacePacket.aggregator.dataReturnIn`, `[downstream].comStatusOut -> SpacePacket.aggregator.comStatusIn`
* Uplink: `[downstream].dataOut -> SpacePacket.spacePacketDeframer.dataIn`, `SpacePacket.spacePacketDeframer.dataReturnOut -> [downstream].dataReturnIn`

### 2.3 Required Inputs for Operation

* **Rate Groups:** Connect a rate group to **`comQueue.run`** (telemetry send rate), **`aggregator.timeout`** (flush rate), and **`commsBufferManager.schedIn`** (buffer manager telemetry).
* **Flight-side hookups:** Wire the **router** outputs (commands/files) into your CDH stack, and feed **packet sources** (telemetry/events/file downlink) into **`comQueue`**.

---

## 3. Configuration

> Configure **only the instance properties** owned by the SpacePacket subtopologies. All knobs live under:
> `Svc/Subtopologies/SpacePacket/SpacePacketConfig/SpacePacketConfig.fpp`

* **Base ID** — Base identifier for the subtopologies; instance IDs are offset from this base.
* **Queue sizes / Stack sizes / Priorities** — Properties of the active components (`comQueue`, `aggregator`).
* **Queue depths / priorities** — `ComQueue` per-channel configuration.
* **BuffMgr** — Bin configuration for `commsBufferManager`.

---

## 4. Traceability Matrix

| Requirement ID      | Satisfied by (instance/type)                                          |
| ------------------- | ---------------------------------------------------------------------- |
| SVC-SPACEPACKET-001 | `spacePacketFramer` — `Svc.Ccsds.SpacePacketFramer`                    |
| SVC-SPACEPACKET-002 | `spacePacketDeframer` — `Svc.Ccsds.SpacePacketDeframer`                |
| SVC-SPACEPACKET-003 | `fprimeRouter` — `Svc.FprimeRouter`                                    |
| SVC-SPACEPACKET-004 | `Subtopology` (variant including `Svc.ComStub`)                        |
| SVC-SPACEPACKET-005 | `FramingSubtopology` (variant expecting a downstream layer)            |
| SVC-SPACEPACKET-006 | `SpacePacketConfig` module                                             |
