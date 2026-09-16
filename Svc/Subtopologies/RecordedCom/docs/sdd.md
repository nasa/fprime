# RecordedCom Subtopology — Software Design Document (SDD)

The **RecordedCom subtopology** provides COM buffer recording functionality as data products. It captures incoming COM buffers (typically events or telemetry packets), packages them into data product containers, and writes them to the filesystem for later analysis or downlink. The subtopology integrates **ComLoggerDp** for recording, **DpManager** for buffer management, **DpWriter** for filesystem persistence, and **BufferManager** for buffer pool allocation, providing a complete, self-contained COM recording solution.

## 1. Requirements

| ID                  | Description                                                                                                       | Validation |
| ------------------- | ----------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-RECORDEDCOM-001 | The subtopology shall provide **COM buffer recording functionality** to capture COM traffic as data products.     | Inspection |
| SVC-RECORDEDCOM-002 | The subtopology shall provide **data product buffer management** for allocating and recycling recording buffers. | Inspection |
| SVC-RECORDEDCOM-003 | The subtopology shall provide **data product writing functionality** to persist recorded COM data to disk.        | Inspection |
| SVC-RECORDEDCOM-004 | The subtopology shall support **configurable recording parameters** (packets per container, buffer pool size).    | Inspection |
| SVC-RECORDEDCOM-005 | The subtopology shall support **command-driven recording control** (start, stop, update priority).                | Inspection |
| SVC-RECORDEDCOM-006 | The subtopology shall expose **rate-group connection points** for components requiring scheduling.                | Inspection |

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name     | Type (Svc)      | Kind    | Purpose (core function)                                                |
| ----------------- | --------------- | ------- | ---------------------------------------------------------------------- |
| `comLogger`       | `ComLoggerDp`   | Active  | Records COM buffers into data product containers.                      |
| `dpMgr`           | `DpManager`     | Active  | Manages data product buffer allocation and forwarding.                 |
| `dpWriter`        | `DpWriter`      | Active  | Writes filled data product containers to files in the recording directory. |
| `dpBufferManager` | `BufferManager` | Passive | Allocates and manages the buffer pool for data product containers.     |

### 2.2 Internal Wiring

* `comLogger.productGetOut[0] -> dpMgr.productGetIn[0]` — synchronous buffer allocation requests.
* `comLogger.productSendOut[0] -> dpMgr.productSendIn[0]` — filled data product containers.
* `dpMgr.bufferGetOut[0] -> dpBufferManager.bufferGetCallee` — buffer pool allocation.
* `dpMgr.productSendOut[0] -> dpWriter.bufferSendIn` — filled containers sent to writer.
* `dpWriter.deallocBufferSendOut -> dpBufferManager.bufferSendIn` — written buffers returned to pool.

### 2.3 Configuration Hooks inside the Subtopology

* **comLogger**: Configured at startup with `configure(enabled, packetsPerContainer, priority)` using values from `RecordedComConfig::ComLogger` (default: disabled, 10 packets per container, priority 5).
* **dpWriter**: The recording directory (`RecordedComConfig::Paths::dpDir`) is created at startup and `dpWriter.configure()` is called with the directory path.
* **dpBufferManager**: A single buffer bin is configured from `RecordedComConfig::BuffMgr` (buffer size: 8192 bytes, count: 20 buffers, manager ID: 302); the manager is cleaned up at teardown.

### 2.4 Required Inputs for Operation

The `RecordedCom` subtopology is not a stand-alone application. It requires connections from the including deployment topology:

* **COM source**: Connect a COM output port (typically events or telemetry) to `comIn` to provide the data stream to record.
* **Rate groups**: Connect scheduler outputs to `comLoggerSchedIn`, `dpMgrSchedIn`, `dpWriterSchedIn`, and `dpBufferManagerSchedIn` for telemetry output (typically at 1Hz).
* **Optional control ports**: Connect `startRecordingIn` and `stopRecordingIn` for port-based recording control (in addition to commands).
* **Health monitoring**: If health monitoring is enabled in the deployment, connect `pingIn` / `pingOut` to the health component.

### 2.5 Operational Flow

1. **Recording Start**: Issue `comLogger.StartComDp(packetsPerContainer, priority)` command or invoke `startRecordingIn` port.
2. **COM Capture**: As COM buffers arrive at `comIn`, ComLoggerDp requests a container buffer from DpManager.
3. **Container Filling**: ComLoggerDp serializes incoming COM buffers into the container (prepended with sentry value 0xDEADBEEF).
4. **Container Send**: When the container reaches `packetsPerContainer` limit, it's sent to DpManager, which forwards to DpWriter.
5. **Filesystem Write**: DpWriter writes the container to `./RecordedCom/Dp_<containerID>_<timeSec>_<timeUsec>.fdp`.
6. **Buffer Return**: Written buffers are returned to BufferManager for reuse.
7. **Recording Stop**: Issue `comLogger.StopComDp()` command or invoke `stopRecordingIn` port; any partial container is flushed.

## 3. Usage

### 3.1 Exposed Topology Ports

| Port                       | Direction | Underlying port                | Purpose                                          |
| -------------------------- | --------- | ------------------------------ | ------------------------------------------------ |
| `comIn`                    | input     | `comLogger.comIn`              | COM buffers to record                            |
| `startRecordingIn`         | input     | `comLogger.startRecordingIn`   | Port-based recording start control               |
| `stopRecordingIn`          | input     | `comLogger.stopRecordingIn`    | Port-based recording stop control                |
| `pingIn` / `pingOut`       | input/out | `comLogger.pingIn/Out`         | Health monitoring                                |
| `comLoggerSchedIn`         | input     | `comLogger.schedIn`            | Rate-group scheduling for ComLoggerDp telemetry  |
| `dpMgrSchedIn`             | input     | `dpMgr.schedIn`                | Rate-group scheduling for DpManager telemetry    |
| `dpWriterSchedIn`          | input     | `dpWriter.schedIn`             | Rate-group scheduling for DpWriter telemetry     |
| `dpBufferManagerSchedIn`   | input     | `dpBufferManager.schedIn`      | Rate-group scheduling for BufferManager telemetry|

### 3.2 Example Usage

Example integrating `RecordedCom` into the Ref deployment to record events:

```fpp
topology Ref {
  instance RecordedCom.Subtopology
  instance comSplitter: Svc.ComSplitter  # Split COM stream for recording

  connections ComCcsds_CdhCore {
    # Split event stream: downlink + recording
    CdhCore.Subtopology.eventsPktSend -> comSplitter.comIn
    comSplitter.comOut[0] -> ComCcsds.Subtopology.comPacketQueueIn[EVENTS]
    comSplitter.comOut[1] -> RecordedCom.Subtopology.comIn
  }

  connections RateGroups {
    # Connect 1Hz rate group to RecordedCom components
    rateGroup2Comp.RateGroupMemberOut[6] -> RecordedCom.Subtopology.comLoggerSchedIn
    rateGroup2Comp.RateGroupMemberOut[7] -> RecordedCom.Subtopology.dpMgrSchedIn
    rateGroup2Comp.RateGroupMemberOut[8] -> RecordedCom.Subtopology.dpWriterSchedIn
    rateGroup2Comp.RateGroupMemberOut[9] -> RecordedCom.Subtopology.dpBufferManagerSchedIn
  }

  # Optional: health monitoring
  connections Health {
    health.pingOut[N] -> RecordedCom.Subtopology.pingIn
    RecordedCom.Subtopology.pingOut -> health.pingReturn[N]
  }
}
```

### 3.3 Runtime Commands

Control recording via ComLoggerDp commands:

* `comLogger.StartComDp(packetsPerContainer, priority)` — Start recording with specified container size and priority.
* `comLogger.StopComDp()` — Stop recording and flush any partial container.
* `comLogger.UpdatePriority(priority)` — Change data product priority while recording.
* `comLogger.CLEAR_COUNTERS` — Reset telemetry counters.

### 3.4 Telemetry Channels

Monitor recording status via telemetry:

* `comLogger.LoggingEnabled` (bool) — Current recording state.
* `comLogger.NumBuffersLogged` (U32) — Total COM buffers recorded since start or last counter clear.
* `comLogger.NumBuffersDropped` (U32) — COM buffers lost due to buffer allocation failure.
* `dpBufferManager.TotalBuffs`, `CurrBuffs`, `HiBuffs`, `NoBuffs`, `EmptyBuffs` — Buffer pool health.
* `dpWriter.NumSuccessfulWrites`, `NumFailedWrites`, `NumBytesWritten` — Write statistics.

### 3.5 Events

* `comLogger.ComDpStarted` (ACTIVITY HIGH) — Recording started.
* `comLogger.ComDpStopped` (ACTIVITY HIGH) — Recording stopped.
* `comLogger.DpBufferError` (WARNING HIGH, throttled) — Buffer allocation failed; data loss occurred.
* `dpWriter.FileWritten` (ACTIVITY LOW) — Data product file successfully written.
* `dpWriter.FileWriteError` (WARNING HIGH, throttled) — File write failure.

## 4. Configuration

Configure the subtopology instance properties via `Svc/Subtopologies/RecordedCom/RecordedComConfig/RecordedComConfig.fpp`:

### 4.1 Configuration Parameters

* **Base ID** — `0x08000000` — Base identifier for the subtopology; component IDs are offset by 0x0000, 0x1000, 0x2000, 0x3000.
* **Queue sizes** — `comLogger: 20, dpMgr: 15, dpWriter: 10` — Queue depths for active components.
* **Stack sizes** — `64 * 1024` (64KB) — Stack size for all active component threads.
* **Priorities** — `comLogger: 25, dpMgr: 24, dpWriter: 23` — Thread priorities (comLogger highest to capture data immediately).
* **CPU affinities** — `Os.TASK_DEFAULT` — CPU affinity for all components (deployment-configurable).
* **BuffMgr** — Buffer pool configuration:
  * `dpBufferStoreSize: 8192` (8KB) — Individual buffer size (sized for 10 packets per container using `ComLoggerDpBuffSize()` helper).
  * `dpBufferStoreCount: 20` — Number of buffers in pool (total: 160KB memory).
  * `dpBufferManagerId: 302` — Unique buffer manager ID.
* **ComLogger** — Recording defaults:
  * `packetsPerContainer: 10` — Default packets per container.
  * `enabled: false` — Recording disabled at startup (enable via command).
  * `defaultPriority: 5` — Default data product priority.
* **Paths** — `dpDir: "./RecordedCom"` — Directory for data product files.

### 4.2 Memory Allocator

The memory allocator used by the subtopology is provided by `RecordedComSubtopologyConfig.cpp` (`RecordedCom::Allocation::memAllocator`), which uses `Fw::MallocAllocator` by default. Projects can substitute a custom allocator by updating this file.

### 4.3 Buffer Sizing

The buffer size (8192 bytes) is calculated to hold 10 COM packets per container using the formula:

```cpp
ComLoggerDp::ComLoggerDpBuffSize(packetsPerContainer) = 
    DpContainer::MIN_PACKET_SIZE + 
    packetsPerContainer * SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE + sizeof(ComLoggerDpSentry))
```

Where:
* `FW_COM_BUFFER_MAX_SIZE = 512` bytes (COM buffer size)
* `ComLoggerDpSentry = 0xDEADBEEF` (4-byte marker)
* `DpContainer::MIN_PACKET_SIZE ≈ 100` bytes (header + hashes)

For 10 packets: ~6KB actual data, rounded to 8KB for margin.

## 5. Performance Tuning

### 5.1 Buffer Pool Sizing

Adjust buffer pool if experiencing drops:
* Monitor `comLogger.NumBuffersDropped` and `dpBufferManager.NoBuffs` telemetry.
* Increase `RecordedComConfig::BuffMgr::dpBufferStoreCount` for higher COM traffic rates.
* Consider memory constraints on embedded systems.

### 5.2 Container Size

Trade-off between file size and latency:
* **Smaller** (5 packets): More frequent writes, lower latency, higher I/O overhead.
* **Larger** (20 packets): Less frequent writes, higher latency, lower I/O overhead.
* Adjust `packetsPerContainer` in `StartComDp` command or config default.

### 5.3 Priority Assignment

* ComLoggerDp priority (25) should be higher than normal COM processing but lower than critical safety tasks.
* Adjust relative to deployment's priority scheme in `RecordedComConfig::Priorities`.

## 6. Data Product File Format

Recorded data products are written to `./RecordedCom/` with filename format:

```
Dp_<containerID>_<timeSec>_<timeUsec>.fdp
```

Each container contains:
* **DpContainer header** — Container ID, priority, time tag, processing types, state.
* **Header hash** — HASH_DIGEST_LENGTH bytes.
* **Record data** — `N` × `ComBufferRecord`:
  * 4-byte sentry (0xDEADBEEF)
  * COM buffer data (up to 512 bytes)
* **Data hash** — HASH_DIGEST_LENGTH bytes.

See [Svc/ComLoggerDp/docs/sdd.md](../../../ComLoggerDp/docs/sdd.md) for detailed data product format specification.

## 7. Traceability Matrix

| Requirement ID      | Satisfied by                                                          |
| ------------------- | --------------------------------------------------------------------- |
| SVC-RECORDEDCOM-001 | `comLogger` — `Svc.ComLoggerDp`                                       |
| SVC-RECORDEDCOM-002 | `dpMgr` — `Svc.DpManager`, `dpBufferManager` — `Svc.BufferManager`   |
| SVC-RECORDEDCOM-003 | `dpWriter` — `Svc.DpWriter`                                           |
| SVC-RECORDEDCOM-004 | `RecordedComConfig` (buffer pool, container size configuration)       |
| SVC-RECORDEDCOM-005 | ComLoggerDp commands (`StartComDp`, `StopComDp`, `UpdatePriority`)   |
| SVC-RECORDEDCOM-006 | Exposed `*SchedIn` topology ports                                     |

## 8. Limitations

* **No catalog management** — RecordedCom does not include DpCatalog; files accumulate in the directory without automatic cleanup or downlink management. Add DpCatalog integration if catalog functionality is needed.
* **No compression** — Data products are written uncompressed. Connect `dpWriter.procBufferSendOut` to the `DpCompression` subtopology if compression is desired.
* **No buffer accumulation** — Filled containers flow directly from DpManager to DpWriter without accumulation buffering. Add BufferAccumulator if flow control is needed for burst traffic.
* **Single COM source** — Only one COM input port is exposed. Use ComSplitter (as shown in example) if recording from multiple sources or splitting traffic.

## 9. Change Log

| Date       | Description                                        |
| ---------- | -------------------------------------------------- |
| 2026-09-15 | Initial implementation with ComLoggerDp, DpManager, DpWriter, BufferManager |