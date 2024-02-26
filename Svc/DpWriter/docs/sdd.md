\page SvcDpWriterComponent Svc::DpWriter Component
# Svc::DpWriter (Active Component)

## 1. Introduction

`Svc::DpWriter` is an active component for writing data products to disk.
It does the following:

1. Receive buffers containing filled data product containers.
The buffers typically come from one or more components that produce
data products.
They typically pass through an instance of
[`Svc::DpManager`](../../DpManager/docs/sdd.md), and possibly through
an instance of
[`Svc::BufferAccumulator`](../../BufferAccumulator/docs/BufferAccumulator.md),
before reaching `DpWriter`.

1. For each buffer _B_ received in step 1:

   1. Perform any requested processing, such as data compression, on _B_.

   1. Write _B_ to disk.

## 2. Requirements

Requirement | Description | Rationale | Verification Method
----------- | ----------- | ----------| -------------------
SVC-DPWRITER-001 | `Svc::DpWriter` shall provide a port for receiving `Fw::Buffer` objects pointing to filled data product containers. | The purpose of `DpWriter` is to write the data products to disk. | Unit Test
SVC-DPWRITER-002 | `Svc::DpWriter` shall provide an array of ports for sending `Fw::Buffer` objects for processing. | This requirement supports downstream processing of the data in the buffer. | Unit Test
SVC-DPWRITER-003 | On receiving a data product container _C_, `Svc::DpWriter` shall use the processing type field of the header of _C_ to select zero or more processing ports to invoke, in port order. | The processing type field is a bit mask. A one in bit `2^n` in the bit mask selects port index `n`. | Unit Test
SVC-DPWRITER-004 | On receiving an `Fw::Buffer` _B_, and after performing any requested processing on _B_, `Svc::DpWriter` shall write _B_ to disk. | The purpose of `DpWriter` is to write data products to the disk. | Unit Test
SVC-DPWRITER-005 | `Svc::DpManager` shall provide telemetry that reports the number of data products written and the number of bytes written. | This requirement establishes the telemetry interface for the component. | Unit test

## 3. Design

### 3.1. Component Diagram

The diagram below shows the `DpWriter` component.

<div>
<img src="img/DpWriter.png" width=700/>
</div>

### 3.2. Ports

`DpWriter` has the following ports:

| Kind | Name | Port Type | Usage |
|------|------|-----------|-------|
| `async input` | `schedIn` | `Svc.Sched` | Schedule in port |
| `async input` | `bufferSendIn` | `Fw.BufferSend` | Port for receiving data products to write to disk |
| `output` | `procBufferSendOut` | `[DpWriterNumProcPorts] Fw.BufferSend` | Port for processing data products |
| `output` | `deallocBufferSendOut` | `Fw.BufferSend` | Port for deallocating data product buffers |
| `time get` | `timeGetOut` | `Fw.Time` | Time get port |
| `telemetry` | `tlmOut` | `Fw.Tlm` | Telemetry port |
| `event` | `eventOut` | `Fw.Log` | Event port |
| `text event` | `textEventOut` | `Fw.LogText` | Text event port |

### 3.3. State

`DpWriter` maintains the following state:

1. `numDataProducts (U32)`: The number of data products written.

1. `numBytes (U64)`: The number of bytes written.

### 3.4. Compile-Time Setup

The configuration constant [`DpWriterNumProcPorts`](../../../config/AcConstants.fpp)
specifies the number of ports for connecting components that perform
processing.

### 3.5. Runtime Setup

The `config` function specifies the following constants:

1. `fileNamePrefix (string)`: The prefix to use for file names.

1. `fileNameSuffix (string)`: The suffix to use for file names.

### 3.6. Port Handlers

#### 3.6.1. schedIn

This handler sends out the state variables as telemetry.

#### 3.6.2. bufferSendIn

This handler receives a mutable reference to a buffer `B`.
It does the following:

1. Check that `B` is valid and that the first `sizeof(FwPacketDescriptorType)`
   bytes of the memory referred to by `B` hold the serialized value
   [`Fw_PACKET_DP`](../../../Fw/Com/ComPacket.hpp).
   If not, emit a warning event.

1. If step 1 succeeded, then

   1. Read the `ProcType` field out of the container header stored in the
      memory pointed to by `B`.
      If the value is a valid port number `N` for `procBufferSendOut`, then invoke
      `procBufferSendOut` at port number `N`, passing in `B`.
      This step updates the memory pointed to by `B` in place.

   1. Write `B` to a file, using the format described in the [**File
      Format**](#file_format) section. For the time stamp, use the time
      provided by `timeGetOut`.

1. Send `B` on `deallocBufferSendOut`.

<a name="file_format"></a>
## 4. File Format

### 4.1. Data Format

Each file stores a serialized data product record,
with the format described in the
[data products documentation](../../../Fw/Dp/docs/sdd.md#serial-format).

### 4.2. File Name

The name of each file consists of `fileNamePrefix` followed by an
ID, a time stamp, and `fileNameSuffix`.
The ID consists of an underscore character `_` followed by the container ID.
The time stamp consists of an underscore character `_` followed by a seconds 
value, an underscore character, and a microseconds value.

For example, suppose that the file name prefix is `container_data` and the
file name suffix is `.dat`.
Suppose that container ID is 100, the seconds value is 100000,
and the microseconds value is 1000.
Then the file name is `container_data_100_100000_1000.dat`.

<a name="ground_interface"></a>
## 5. Ground Interface

### 5.1. Telemetry

| Name | Type | Description |
|------|------|-------------|
| `NumDataProducts` | `U32` | The number of data products handled |
| `NumBytes` | `U64` | The number of bytes handled |

### 5.2. Events

| Name | Severity | Description |
|------|----------|-------------|
| `BufferTooSmall` | `warning high` | Incoming buffer is too small to hold a data product container |
| `InvalidPacketDescriptor` | `warning high` | Incoming buffer had an invalid packet descriptor |

## 6. Example Uses

<a name="top-diagrams"></a>
### 6.1. Topology Diagrams

The following topology diagram shows how to connect `Svc::DpWriter`
to a `DpManager` component and a processor component.
The diagrams use the following instances:

* `dpManager`: An instance of [`Svc::DpManager`](../../DpManager/docs/sdd.md).

* `dpProcessor`: A component that processes data product containers.

* `dpWriter`: An instance of `Svc::DpWriter`.

* `producer`: A component that produces data products.

<div>
<img src="img/top/product-write.png" width=800/>
</div>

### 6.2. Sequence Diagrams

The following diagram shows what happens when a buffer is sent to `DpWriter`,
is processed, and is written to disk.

```mermaid
sequenceDiagram
    activate producer
    activate dpManager
    activate dpWriter
    producer-)dpManager: Send buffer
    dpManager-)dpWriter: Send buffer [bufferSendIn]
    dpWriter->>dpProcessor: Process buffer B [procBufferSendOut]
    dpProcessor-->>dpWriter: Return
    dpWriter->>dpWriter: Write B to disk
    dpWriter->>bufferManager: Deallocate B [deallocBufferSendOut]
    bufferManager-->>dpWriter: Return
    deactivate dpWriter
    deactivate dpManager
    deactivate producer
```
