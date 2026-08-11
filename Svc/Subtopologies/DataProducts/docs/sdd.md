# DataProducts Subtopology — Software Design Document (SDD)

The **DataProducts subtopology** packages the standard F´ data product services — **product management**, **product writing**, **product cataloging**, and the buffer infrastructure that connects them — into a single pre-wired subtopology. Client components that produce data products connect to the exposed topology ports; the subtopology handles buffer allocation, accumulation, writing products to the filesystem, and cataloging written products for prioritized downlink.

## 1. Requirements

| ID                   | Description                                                                                                          | Validation |
| -------------------- | -------------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-DATAPRODUCTS-001 | The subtopology shall provide **data product buffer management** for client components requesting product buffers.   | Inspection |
| SVC-DATAPRODUCTS-002 | The subtopology shall provide **data product writing functionality** to store filled products on the filesystem.     | Inspection |
| SVC-DATAPRODUCTS-003 | The subtopology shall provide **data product cataloging functionality** for prioritized downlink of stored products. | Inspection |
| SVC-DATAPRODUCTS-004 | The subtopology shall provide **buffer accumulation** between product management and product writing.                | Inspection |
| SVC-DATAPRODUCTS-005 | The subtopology shall support **configurable instance properties** (IDs, queue sizes, stack sizes, priorities).      | Inspection |
| SVC-DATAPRODUCTS-006 | The subtopology shall expose **rate-group connection points** for components requiring scheduling.                   | Inspection |

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name         | Type (Svc)          | Kind    | Purpose (core function)                                              |
| --------------------- | ------------------- | ------- | -------------------------------------------------------------------- |
| `dpMgr`               | `DpManager`         | Active  | Services product get/request/send ports from client components.      |
| `dpWriter`            | `DpWriter`          | Active  | Writes filled product buffers to files in the data product directory.|
| `dpCat`               | `DpCatalog`         | Active  | Catalogs written products; commands prioritized downlink.            |
| `dpBufferAccumulator` | `BufferAccumulator` | Active  | Buffers filled products between `dpMgr` and `dpWriter`.              |
| `dpBufferManager`     | `BufferManager`     | Passive | Allocates data product buffers.                                      |

### 2.2 Internal Wiring

* `dpMgr.bufferGetOut -> dpBufferManager.bufferGetCallee` — product buffer allocation.
* `dpMgr.productSendOut -> dpBufferAccumulator.bufferSendInFill` — filled products enter the accumulator.
* `dpBufferAccumulator.bufferSendOutDrain -> dpWriter.bufferSendIn` — accumulated products drain to the writer.
* `dpWriter.deallocBufferSendOut -> dpBufferAccumulator.bufferSendInReturn` and `dpBufferAccumulator.bufferSendOutReturn -> dpBufferManager.bufferSendIn` — written buffers return to the buffer manager.
* `dpWriter.dpWrittenOut -> dpCat.addToCat` — written products are added to the catalog.

### 2.3 Configuration Hooks inside the Subtopology

* **dpCat**: The data product directory (`DataProductsConfig::Paths::dpDir`) is created at startup and `dpCat.configure()` is called with the directory, state file (`DataProductsConfig::Paths::dpState`), and the subtopology memory allocator.
* **dpWriter**: `dpWriter.configure()` is called with the data product directory.
* **dpBufferAccumulator**: The queue is allocated at startup (drain mode `DRAIN`) and deallocated at teardown, sized by `DataProductsConfig::BufferAccumulator::maxNumBuffers`.
* **dpBufferManager**: A single buffer bin is set up from `DataProductsConfig::BuffMgr` (buffer size and count); the manager is cleaned up (and `dpCat.shutdown()` called) at teardown.

### 2.4 Required Inputs for Operation

The `DataProducts` subtopology is not a stand-alone application. It requires connections from the including deployment topology:

* **Client components**: Connect data-product-producing components to `productGetIn` / `productRequestIn` / `productSendIn` / `productResponseOut`.
* **Rate groups**: Connect scheduler outputs to `dpMgrSchedIn`, `dpWriterSchedIn`, and `dpBufferManagerSchedIn` for telemetry output.
* **File downlink**: Connect `dpCatFileOut` / `dpCatFileDone` to a file downlink component (e.g., via the `FileHandling` subtopology) so cataloged products can be downlinked.
* **Optional processing**: `dpWriterProcOut` provides a hook for processing product buffers before they are written (e.g., compression via the [`DpCompression`](../../DpCompression/docs/sdd.md) subtopology).

## 3. Usage

### 3.1 Exposed Topology Ports

| Port | Direction | Underlying port | Purpose |
| --- | --- | --- | --- |
| `productGetIn` | input | `dpMgr.productGetIn` | Synchronous product buffer get requests from clients |
| `productRequestIn` | input | `dpMgr.productRequestIn` | Asynchronous product buffer requests from clients |
| `productSendIn` | input | `dpMgr.productSendIn` | Filled product buffers from clients |
| `productResponseOut` | output | `dpMgr.productResponseOut` | Requested product buffers back to clients |
| `dpCatFileOut` | output | `dpCat.fileOut` | File downlink requests |
| `dpCatFileDone` | input | `dpCat.fileDone` | File downlink completion notifications |
| `dpWriterProcOut` | output | `dpWriter.procBufferSendOut` | Product processing hook |
| `dpMgrSchedIn` / `dpWriterSchedIn` / `dpBufferManagerSchedIn` | input | respective `schedIn` ports | Rate-group scheduling for telemetry |

### 3.2 Example Usage

```fpp
topology Flight {
  instance DataProducts.Subtopology

  connections RateGroups {
    rg1Hz.RateGroupMemberOut[0] -> DataProducts.Subtopology.dpMgrSchedIn
    rg1Hz.RateGroupMemberOut[1] -> DataProducts.Subtopology.dpWriterSchedIn
    rg1Hz.RateGroupMemberOut[2] -> DataProducts.Subtopology.dpBufferManagerSchedIn
  }

  connections Producers {
    myProducer.productGetOut -> DataProducts.Subtopology.productGetIn
    myProducer.productSendOut -> DataProducts.Subtopology.productSendIn
  }

  connections Downlink {
    DataProducts.Subtopology.dpCatFileOut -> FileHandling.Subtopology.fileDownlinkSendIn
    FileHandling.Subtopology.fileDownlinkCompleteOut -> DataProducts.Subtopology.dpCatFileDone
  }
}
```

## 4. Configuration

Configure the instance properties for the static instances owned by the subtopology. All knobs live under `Svc/Subtopologies/DataProducts/DataProductsConfig/DataProductsConfig.fpp`:

* **Base ID** — Base identifier for the subtopology; component IDs are offset from this base.
* **Queue sizes / stack sizes / priorities** — For the active instances (`dpCat`, `dpMgr`, `dpWriter`, `dpBufferAccumulator`).
* **BufferAccumulator** — Allocator ID and maximum number of accumulated buffers.
* **BuffMgr** — Data product buffer store size, count, and buffer manager ID.
* **Paths** — Data product directory and catalog state file path.

The memory allocator used by the subtopology is provided by `DataProductsSubtopologyConfig.cpp` (`DataProducts::Allocation::memAllocator`).

## 5. Traceability Matrix

| Requirement ID       | Satisfied by                                    |
| -------------------- | ----------------------------------------------- |
| SVC-DATAPRODUCTS-001 | `dpMgr` — `Svc.DpManager`, `dpBufferManager`    |
| SVC-DATAPRODUCTS-002 | `dpWriter` — `Svc.DpWriter`                     |
| SVC-DATAPRODUCTS-003 | `dpCat` — `Svc.DpCatalog`                       |
| SVC-DATAPRODUCTS-004 | `dpBufferAccumulator` — `Svc.BufferAccumulator` |
| SVC-DATAPRODUCTS-005 | `DataProductsConfig` (instance properties)      |
| SVC-DATAPRODUCTS-006 | Exposed `*SchedIn` topology ports               |
