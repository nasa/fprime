# Drv::ByteStreamBufferAdapter

## 1. Introduction

The `Drv::ByteStreamBufferAdapter` is a passive component that serves as an adapter between two different driver interfaces in F´:

1. The `ByteStreamDriver` interface, which is used for sending and receiving streams of bytes
2. The `PassiveBufferDriver` interface, which is used for sending and receiving `Fw::Buffer` objects

This adapter component allows components that expect a `PassiveBufferDriver` interface to communicate with components that provide a `ByteStreamDriver` interface, bridging the gap between these two different models of data transfer.


## 2. Requirements

| Name | Description | Validation |
|------|-------------|------------|
| DRV-BSB-ADAPTER-001 | The component shall adapt the ByteStreamDriver interface to the PassiveBufferDriver interface | Unit Test |
| DRV-BSB-ADAPTER-002 | The component shall handle buffer ownership transfers between the two interfaces | Unit Test |
| DRV-BSB-ADAPTER-003 | The component shall monitor the ByteStreamDriver readiness state | Unit Test |
| DRV-BSB-ADAPTER-004 | The component shall emit events on error conditions | Unit Test |

## 3. Design

### 3.1 Assumptions

The design of `ByteStreamBufferAdapter` assumes the following:

1. The byte stream driver will properly signal ready status before accepting data.
2. Buffer ownership follows the standard Return-To-Sender pattern: buffers are owned by the receiver until returned.
3. The byte stream driver operates correctly and returns buffers sent to it when it is done processing.

### 3.2 Ports

| Kind | Name | Type | Description |
|------|------|------|-------------|
| sync input | bufferIn | Fw.BufferSend | Receives buffers from the client to send to the byte stream driver |
| output | bufferInReturn | Fw.BufferSend | Returns ownership of buffers received on `bufferIn` |
| output | bufferOut | Fw.BufferSend | Sends buffers received from the byte stream driver to the client |
| sync input | bufferOutReturn | Fw.BufferSend | Receives back ownership of buffers sent on `bufferOut` |
| sync input | byteStreamDriverReady | Drv.ByteStreamReady | Receives ready signals from the byte stream driver |
| sync input | fromByteStreamDriver | Drv.ByteStreamData | Receives data from the byte stream driver |
| output | fromByteStreamDriverReturn | Fw.BufferSend | Returns ownership of buffers received on `fromByteStreamDriver` |
| output | toByteStreamDriver | Drv.ByteStreamSend | Sends data to the byte stream driver |

### 3.3 Events

| Event | Severity | Description |
|-------|----------|-------------|
| DriverNotReady | warning low | A buffer arrived on `bufferIn` before the driver signaled ready |
| DataSendError | warning low | The driver reported an error sending data |
| DataReceiveError | warning low | The driver reported an error receiving data |

### 3.4 State

The component maintains a single state variable:

- `m_driverIsReady`: An atomic boolean that tracks whether the `ByteStreamDriver` is ready to receive data. Initialized to `false` and set to `true` when a ready signal is received from the driver.
