# CcsdsFraming Subtopology — Software Design Document (SDD)

The **CcsdsFraming subtopology** boxes the **CCSDS TM/TC transfer frame layer** of the communications stack: the TM framer (downlink), and the frame accumulator + TC deframer (uplink). It carries no packet-layer components and no `Svc.ComInterface` implementation — it is intended to be composed between a packet layer (e.g., the `SpacePacket` subtopology) and a com interface, as done by the `ComCcsds` subtopology.

> [!IMPORTANT]
> `CcsdsFraming` provides only the TM/TC transfer frame layer. Compose it with the `SpacePacket` subtopology (see `ComCcsds`) for a full CCSDS communications stack.

---

## 1. Requirements

| ID                   | Description                                                                                         | Validation |
| -------------------- | ----------------------------------------------------------------------------------------------------- | ---------- |
| SVC-CCSDSFRAMING-001 | Provide a **CCSDS TM framer** to convert buffers into TM transfer frames for downlink.              | Inspection |
| SVC-CCSDSFRAMING-002 | Provide a **frame accumulator** to collect bytes from the link and emit complete TC frames.         | Inspection |
| SVC-CCSDSFRAMING-003 | Provide a **CCSDS TC deframer** to parse accumulated TC transfer frames for uplink.                 | Inspection |
| SVC-CCSDSFRAMING-004 | Support **configurable instance properties** (IDs, accumulator buffer size) via `CcsdsFramingConfig`. | Inspection |

---

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name      | Type (Svc)             | Kind    | Purpose (core function)                                                              |
| ------------------ | ---------------------- | ------- | ------------------------------------------------------------------------------------- |
| `framer`           | `Svc.Ccsds.TmFramer`   | Passive | Builds **CCSDS TM Transfer Frames** and sends them to the link (downlink).            |
| `frameAccumulator` | `Svc.FrameAccumulator` | Passive | Collects bytes from the link and emits complete TC frames (uplink step 1).            |
| `tcDeframer`       | `Svc.Ccsds.TcDeframer` | Passive | Deframes **CCSDS TC Transfer Frames** into their payloads (uplink step 2).            |

### 2.2 External Boundary

Only the `frameAccumulator -> tcDeframer` uplink hop is internal. The composer shall connect:

* **Upstream (packet layer):** `[upstream].dataOut -> framer.dataIn`, `framer.dataReturnOut -> [upstream].dataReturnIn`, `framer.comStatusOut -> [upstream].comStatusIn`, `tcDeframer.dataOut -> [upstream].dataIn`, `[upstream].dataReturnOut -> tcDeframer.dataReturnIn`
* **Downstream (`Svc.ComInterface`):** `framer.dataOut -> [Svc.Com].dataIn`, `frameAccumulator.dataReturnOut -> [Svc.Com].dataReturnIn`, `[Svc.Com].dataReturnOut -> framer.dataReturnIn`, `[Svc.Com].comStatusOut -> framer.comStatusIn`, `[Svc.Com].dataOut -> frameAccumulator.dataIn`
* **Buffer management:** `frameAccumulator.bufferAllocate -> [BufferManager].bufferGetCallee`, `frameAccumulator.bufferDeallocate -> [BufferManager].bufferSendIn`

---

## 3. Configuration

> All knobs live under: `Svc/Subtopologies/CcsdsFraming/CcsdsFramingConfig/CcsdsFramingConfig.fpp`

* **Base ID** — Base identifier for the subtopology; instance IDs are offset from this base.
* **BuffMgr.frameAccumulatorSize** — Size of the frame accumulation buffer.

---

## 4. Traceability Matrix

| Requirement ID       | Satisfied by (instance/type)                 |
| -------------------- | ---------------------------------------------- |
| SVC-CCSDSFRAMING-001 | `framer` — `Svc.Ccsds.TmFramer`                |
| SVC-CCSDSFRAMING-002 | `frameAccumulator` — `Svc.FrameAccumulator`    |
| SVC-CCSDSFRAMING-003 | `tcDeframer` — `Svc.Ccsds.TcDeframer`          |
| SVC-CCSDSFRAMING-004 | `CcsdsFramingConfig` module                    |
