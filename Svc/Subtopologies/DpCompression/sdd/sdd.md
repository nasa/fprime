# DpCompression Subtopology — Software Design Document (SDD)

The **DpCompression subtopology** packages the F´ data product compression services — a **compression processor**, a **ZLib compressor**, and a dedicated **buffer manager** — into a single pre-wired subtopology. It plugs into a data product pipeline (typically the [`DataProducts`](../../DataProducts/docs/sdd.md) subtopology's `dpWriterProcOut` processing hook) to compress data products before they are stored and downlinked.

## 1. Requirements

| ID                    | Description                                                                                                  | Validation |
| --------------------- | ------------------------------------------------------------------------------------------------------------ | ---------- |
| SVC-DPCOMPRESSION-001 | The subtopology shall provide **data product compression processing** for product buffers.                   | Inspection |
| SVC-DPCOMPRESSION-002 | The subtopology shall provide **ZLib-based compression** of product data.                                    | Inspection |
| SVC-DPCOMPRESSION-003 | The subtopology shall provide **buffer management** for compression working and output buffers.               | Inspection |
| SVC-DPCOMPRESSION-004 | The subtopology shall support **configurable buffer store properties** (sizes, counts, allocator ID).         | Inspection |
| SVC-DPCOMPRESSION-005 | The subtopology shall expose a **rate-group connection point** for buffer manager telemetry.                  | Inspection |

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name                   | Type (Svc)         | Kind    | Purpose (core function)                                       |
| ------------------------------- | ------------------ | ------- | -------------------------------------------------------------- |
| `dpCompressProc`                | `DpCompressProc`   | Passive | Receives product processing requests; chunks data for compression. |
| `dpZLibCompressor`              | `DpZLibCompressor` | Passive | Compresses chunks using ZLib.                                   |
| `dpZLibCompressorBufferManager` | `BufferManager`    | Passive | Allocates compression working and output buffers.               |

### 2.2 Internal Wiring

* `dpCompressProc.compressChunk -> dpZLibCompressor.compressChunk` — chunks flow to the compressor.
* `dpZLibCompressor.bufferCompressionGet / bufferCompressionReturn -> dpZLibCompressorBufferManager` — compression output buffer allocation and return.
* `dpZLibCompressor.bufferZLibGet / bufferZLibReturn -> dpZLibCompressorBufferManager` — ZLib working buffer allocation and return.

### 2.3 Configuration Hooks inside the Subtopology

* **dpZLibCompressorBufferManager**: Two buffer bins are set up at startup from `DpCompressionConfig::ZLibBuffMgr` — one for compression output buffers and one for ZLib allocation (working) buffers — and cleaned up at teardown.

### 2.4 Required Inputs for Operation

The `DpCompression` subtopology is not a stand-alone application. It requires connections from the including deployment topology:

* **Processing requests**: Connect a data product processing output (e.g., `DataProducts.Subtopology.dpWriterProcOut`) to `dpCompressProcIn`.
* **Rate group**: Connect a scheduler output to `dpZLibBufferManagerSchedIn` for buffer manager telemetry.

## 3. Usage

### 3.1 Exposed Topology Ports

| Port | Direction | Underlying port | Purpose |
| --- | --- | --- | --- |
| `dpCompressProcIn` | input | `dpCompressProc.procRequest` | Data product compression processing requests |
| `dpZLibBufferManagerSchedIn` | input | `dpZLibCompressorBufferManager.schedIn` | Rate-group scheduling for buffer manager telemetry |

### 3.2 Example Usage

```fpp
topology Flight {
  instance DataProducts.Subtopology
  instance DpCompression.Subtopology

  connections Compression {
    DataProducts.Subtopology.dpWriterProcOut -> DpCompression.Subtopology.dpCompressProcIn
  }

  connections RateGroups {
    rg1Hz.RateGroupMemberOut[3] -> DpCompression.Subtopology.dpZLibBufferManagerSchedIn
  }
}
```

## 4. Configuration

Configure the buffer store properties in `Svc/Subtopologies/DpCompression/DpCompressionConfig/DpCompressionConfig.fpp`:

* **Base ID** — Base identifier for the subtopology; component IDs are offset from this base.
* **ZLibBuffMgr** — Compression buffer store size/count, ZLib allocation buffer store size/count, and the buffer manager ID.

The memory allocator used by the subtopology is provided by `DpCompressionSubtopologyConfig.cpp` (`DpCompression::Allocation::memAllocator`).

## 5. Traceability Matrix

| Requirement ID        | Satisfied by                                        |
| --------------------- | --------------------------------------------------- |
| SVC-DPCOMPRESSION-001 | `dpCompressProc` — `Svc.DpCompressProc`             |
| SVC-DPCOMPRESSION-002 | `dpZLibCompressor` — `Svc.DpZLibCompressor`         |
| SVC-DPCOMPRESSION-003 | `dpZLibCompressorBufferManager` — `Svc.BufferManager` |
| SVC-DPCOMPRESSION-004 | `DpCompressionConfig::ZLibBuffMgr` constants        |
| SVC-DPCOMPRESSION-005 | `dpZLibBufferManagerSchedIn` topology port          |
