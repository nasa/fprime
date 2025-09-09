# FileHandling Subtopology — Software Design Document (SDD)

The **FileHandling subtopology** packages the core file-transfer services commonly needed in F´ deployments: **file uplink** (ground → flight), **file downlink** (flight → ground), **on-board file management**, and parameter management via filesystem. By providing these as a pre-wired subgraph, integration engineers avoid repetitive wiring and get a consistent, reusable baseline for file operations.

## 1. Requirements

| ID                   | Description                                                                                                     | Validation |
| -------------------- | --------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-FILEHANDLING-001 | The subtopology shall provide **file uplink functionality** to receive and reconstruct files from ground.       | Inspection |
| SVC-FILEHANDLING-002 | The subtopology shall provide **file downlink functionality** to segment and transmit files to ground.          | Inspection |
| SVC-FILEHANDLING-003 | The subtopology shall provide **on-board file management functionality** (e.g., list, remove, hash, mkdir).     | Inspection |
| SVC-FILEHANDLING-004 | The subtopology shall provide **parameter management** via the filesystem.                                      | Inspection |
| SVC-FILEHANDLING-005 | The subtopology shall support **configurable instance properties** (IDs, queue sizes, stack sizes, priorities). | Inspection |
| SVC-FILEHANDLING-006 | The subtopology shall expose **rate-group connection points** for any rate-drive components it contains.        | Inspection |


## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name  | Type (Svc)     | Kind   | Purpose (core function)                                 |
| -------------- | -------------- | ------ | ------------------------------------------------------- |
| `fileUplink`   | `FileUplink`   | Active | Ingest deframed file packets; reconstruct files.        |
| `fileDownlink` | `FileDownlink` | Active | Read files; segment into packets for downlink.          |
| `fileManager`  | `FileManager`  | Active | Local file operations.                                  |
| `prmDb`        | `PrmDb`        | Active | Filesystem based parameter management.                  |

### 2.2 Configuration Hooks inside the Subtopology

* Uses **instance properties** (IDs, queue sizes, stack sizes, priorities) defined in `FileHandlingConfig` for these static instances (see §4).

### 2.3 Required Inputs for Operation

* **Rate Groups**: Connect scheduler outputs to the **Run** (scheduling) ports of `fileDownlink`.
* **Communication/Framing Stack**: Wire file-packet ports between FileHandling and your COM/framing subtopology (e.g., `ComCcsds`, `ComFprime`, `FramingFprime`, `FramingCcsds`) to complete uplink/downlink paths.

### 2.4 Limitations

Focused on **file transfer and on-board file ops** only. It does **not** provide general uplink/downlink routing for non-file traffic, framing/deframing for non-file data, or broader CDH services.

## 3. Usage

### 3.1 Example Usage

```fpp
topology Flight {
  import FileHandling.Subtopology

  param connections instance FileHandling.prmDb

  # Schedule the active/queued file components (example)
  connections RateGroups {
    rg.RateGroupMemberOut[0] -> FileHandling.fileDownlink.Run
  }

  connections ComCcsds_FileHandling {
    # File Downlink <-> ComQueue
    FileHandling.fileDownlink.bufferSendOut -> ComCcsds.comQueue.bufferQueueIn[ComCcsds.Ports_ComBufferQueue.FILE]
    ComCcsds.comQueue.bufferReturnOut[ComCcsds.Ports_ComBufferQueue.FILE] -> FileHandling.fileDownlink.bufferReturn
    
    # Router <-> FileUplink
    ComCcsds.fprimeRouter.fileOut     -> FileHandling.fileUplink.bufferSendIn
    FileHandling.fileUplink.bufferSendOut -> ComCcsds.fprimeRouter.fileBufferReturnIn
  }
```

## 4. Configuration

> Configure **only the instance properties** for the static instances owned by the subtopology. All knobs live under:
> `Svc/Subtopologies/FileHandling/FileHandlingConfig/FileHandlingConfig.fpp`. The generated constants header for this module (e.g., `FppConstantsAc.hpp`) reflects these settings. ([FPrime][2])

### 4.1 Component properties (`FileHandlingConfig.fpp`)

* **Base ID** — Base identifier for the subtopology; component IDs are offset from this base.
* **Queue sizes** — Queue depths for `fileUplink`, `fileDownlink`, `fileManager`.
* **Stack sizes** — Task stacks for active components (`fileUplink`, `fileDownlink`).
* **Priorities** — RTOS priorities for the active/queued components as applicable.

> These knobs tailor runtime footprint and scheduling without modifying the subtopology wiring.

---

## 5. Traceability Matrix

| Requirement ID       | Satisfied by                               |
| -------------------- | ------------------------------------------ |
| SVC-FILEHANDLING-001 | `fileUplink` — `Svc.FileUplink`            |
| SVC-FILEHANDLING-002 | `fileDownlink` — `Svc.FileDownlink`        |
| SVC-FILEHANDLING-003 | `fileManager` — `Svc.FileManager`          |
| SVC-FILEHANDLING-004 | `prmDb` — `Svc.PrmDb`                      |
| SVC-FILEHANDLING-005 | `FileHandlingConfig` (instance properties) |
| SVC-FILEHANDLING-006 | Run/scheduling connection specifiers       |
