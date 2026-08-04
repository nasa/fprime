# FileHandlingCfdp Subtopology — Software Design Document (SDD)

The **FileHandlingCfdp subtopology** packages CFDP-based file-transfer services and on-board file management commonly needed in F´ CCSDS deployments: CFDP file transfer (bidirectional reliable file delivery), on-board file management, and parameter management via filesystem. By providing these as a pre-wired subgraph, integration engineers avoid repetitive wiring and get a consistent, reusable baseline for CFDP operations.

## 1. Requirements

| ID                       | Description                                                                                                     | Validation |
| ------------------------ | --------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-FILEHANDLINGCFDP-001 | The subtopology shall provide **CFDP file transfer functionality** for reliable bidirectional file delivery.    | Inspection |
| SVC-FILEHANDLINGCFDP-002 | The subtopology shall provide **on-board file management functionality** (e.g., list, remove, hash, mkdir).     | Inspection |
| SVC-FILEHANDLINGCFDP-003 | The subtopology shall provide **parameter management** via the filesystem.                                      | Inspection |
| SVC-FILEHANDLINGCFDP-004 | The subtopology shall support **configurable instance properties** (IDs, queue sizes, stack sizes, priorities). | Inspection |
| SVC-FILEHANDLINGCFDP-005 | The subtopology shall expose **rate-group connection points** for the CFDP manager component.                   | Inspection |


## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name  | Type (Svc)                 | Kind   | Purpose (core function)                                              |
| -------------- | -------------------------- | ------ | -------------------------------------------------------------------- |
| `cfdpManager`  | `Ccsds.Cfdp.CfdpManager`   | Active | CFDP protocol engine; handles reliable file transfer (TX/RX).        |
| `fileManager`  | `FileManager`              | Active | Local file operations (list, remove, hash, mkdir).                   |
| `prmDb`        | `PrmDb`                    | Active | Filesystem based parameter management.                               |

### 2.2 Configuration Hooks inside the Subtopology

* Uses **instance properties** (IDs, queue sizes, stack sizes, priorities) defined in `FileHandlingCfdpConfig` for these static instances (see §4).
* **CfdpManager configuration**: The `cfdpManager` instance is configured with a memory allocator in the `configComponents` phase to allocate the internal CFDP engine.
* **PrmDb configuration**: The `prmDb` instance is configured with a parameter file name (`PrmDb.dat`) and reads parameters during the `readParameters` phase.

### 2.3 Required Inputs for Operation

* **Rate Groups**: Connect scheduler outputs to the **run1Hz** (1 Hz scheduling) port of `cfdpManager` to drive CFDP protocol timers and transaction processing.
* **Communication/Framing Stack**: Wire CFDP PDU ports between FileHandlingCfdp and your COM/framing subtopology (e.g., `ComCcsds`, `FprimeRouter`) to complete uplink/downlink paths for CFDP PDUs.
* **Buffer Management**: Connect buffer allocation/deallocation ports to downstream buffer managers (typically part of your COM subtopology).

### 2.4 Differences from FileHandling Subtopology

The **FileHandlingCfdp** subtopology replaces the traditional `FileUplink` and `FileDownlink` components with the CFDP-based `CfdpManager` component:

* **CFDP Protocol**: Uses the CCSDS File Delivery Protocol (CFDP) for reliable, autonomous file transfer with automatic retry, gap detection, and acknowledgments.
* **Bidirectional Transfer**: Single component handles both uplink (ground → flight) and downlink (flight → ground) file transfers.
* **Class 1 & Class 2**: Supports both unacknowledged (Class 1) and acknowledged (Class 2) transfer modes.

### 2.5 Limitations

Focused on CFDP-based file transfer and on-board file ops only. It does **not** provide:
* General uplink/downlink routing for non-CFDP traffic
* Framing/deframing services (must be provided by separate framing subtopology)
* Generic command and data handling services (Commands, events, telemetry, etc.)

## 3. Usage

### 3.1 Example Usage

```fpp
topology Flight {
  instance FileHandlingCfdp.Subtopology

  param connections instance FileHandlingCfdp.prmDb

  # Schedule the CFDP manager at 1 Hz (required for protocol timers)
  connections RateGroups {
    rg.RateGroupMemberOut[0] -> FileHandlingCfdp.Subtopology.cfdpManager.run1Hz
  }

  # Wire CFDP PDU data flow to/from communication stack
  connections ComCcsds_FileHandlingCfdp {
    # CFDP Downlink -> ComQueue (PDU egress)
    FileHandlingCfdp.Subtopology.cfdpManager.dataOut[0] -> ComCcsds.Subtopology.bufferQueueIn[ComCcsds.Ports_ComBufferQueue.CFDP]
    ComCcsds.Subtopology.bufferReturnOut[ComCcsds.Ports_ComBufferQueue.CFDP] -> FileHandlingCfdp.Subtopology.cfdpManager.dataReturnIn[0]
    
    # FprimeRouter -> CFDP Uplink (PDU ingress)
    ComCcsds.Subtopology.cfdpUplinkOut -> FileHandlingCfdp.Subtopology.cfdpManager.dataIn[0]
    FileHandlingCfdp.Subtopology.cfdpManager.dataInReturn[0] -> ComCcsds.Subtopology.cfdpUplinkReturnIn
    
    # Buffer allocation for CFDP PDU construction
    FileHandlingCfdp.Subtopology.cfdpManager.bufferAllocate[0] -> ComCcsds.Subtopology.bufferGetIn[ComCcsds.Ports_ComBufferGet.CFDP]
    ComCcsds.Subtopology.bufferDeallocateOut[ComCcsds.Ports_ComBufferGet.CFDP] -> FileHandlingCfdp.Subtopology.cfdpManager.bufferDeallocate[0]
  }
}
```

### 3.2 CFDP Configuration

The CFDP manager is configured at runtime via **parameters** defined in `CfdpCfg.fpp` (compile-time configuration) and component parameters (runtime configuration):

#### Compile-time Configuration (`CfdpCfg.fpp`)
* **Number of channels**: `Cfdp::NumChannels` - Number of independent CFDP communication channels (default: 2)
* **Number of transactions**: `Cfdp::NumTransactions` - Maximum concurrent CFDP transactions per channel
* **Maximum PDU size**: `Cfdp::MaxPduSize` - Maximum size of a single CFDP PDU in bytes
* **File path size**: `Cfdp::MaxFilePathSize` - Maximum length of file paths

#### Runtime Parameters (per channel)
* **Local Entity ID**: Unique identifier for this CFDP entity
* **ACK/NAK limits**: Maximum retry attempts for acknowledged transfers
* **Timers**: ACK timeout and inactivity timeout durations
* **Directories**: Temporary, failure, and move directories for file operations
* **Throttling**: Maximum outgoing PDUs per cycle for bandwidth control

See the [CfdpManager SDD](../../../Ccsds/CfdpManager/docs/sdd.md) for detailed configuration options.

## 4. Configuration

> Configure **only the instance properties** for the static instances owned by the subtopology. All knobs live under:
> `Svc/Subtopologies/FileHandlingCfdp/FileHandlingCfdpConfig/FileHandlingCfdpConfig.fpp`. The generated constants header for this module (e.g., `FppConstantsAc.hpp`) reflects these settings.

### 4.1 Component properties (`FileHandlingCfdpConfig.fpp`)

* **Base ID** — Base identifier for the subtopology; component IDs are offset from this base.
* **Queue sizes** — Queue depths for `cfdpManager`, `fileManager`, `prmDb`.
* **Stack sizes** — Task stacks for active components.
* **Priorities** — RTOS priorities for the active/queued components.

> These knobs tailor runtime footprint and scheduling without modifying the subtopology wiring.

### 4.2 Memory Allocation

The `cfdpManager` component requires a memory allocator to be provided during configuration. The subtopology includes a `MallocAllocator` instance (`Allocation::memAllocator`) that is passed to `cfdpManager.configure()` in the `configComponents` phase. The allocator is used to dynamically allocate the CFDP engine structure.

---

## 5. Traceability Matrix

| Requirement ID           | Satisfied by                                   |
| ------------------------ | ---------------------------------------------- |
| SVC-FILEHANDLINGCFDP-001 | `cfdpManager` — `Svc.Ccsds.Cfdp.CfdpManager`   |
| SVC-FILEHANDLINGCFDP-002 | `fileManager` — `Svc.FileManager`              |
| SVC-FILEHANDLINGCFDP-003 | `prmDb` — `Svc.PrmDb`                          |
| SVC-FILEHANDLINGCFDP-004 | `FileHandlingCfdpConfig` (instance properties) |
| SVC-FILEHANDLINGCFDP-005 | run1Hz connection specifier for `cfdpManager`  |
