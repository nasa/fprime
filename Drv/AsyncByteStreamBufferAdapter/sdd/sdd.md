# Drv::AsyncByteStreamBufferAdapter

## 1 Introduction

`AsyncByteStreamBufferAdapter` is a passive F´ component that mediates between the `AsyncByteStreamDriver` interface and the `PassiveBufferDriver` interface. It enables asynchronous byte stream drivers to be used as passive buffer drivers by adapting the interfaces and managing buffer ownership transitions.

The component allows an asynchronous byte stream driver implementation to be transparently used by components that expect a passive buffer driver interface, providing flexibility in topology design.

## 2 Requirements


| Name | Description | Validation |
|------|-------------|------------|
| DRV-ASYNC-BSB-ADAPTER-001 | The component shall adapt the AsyncByteStreamDriver interface to the PassiveBufferDriver interface | Unit Test |
| DRV-ASYNC-BSB-ADAPTER-002 | The component shall handle buffer ownership transfers between the two interfaces | Unit Test |
| DRV-ASYNC-BSB-ADAPTER-003 | The component shall monitor the AsyncByteStreamDriver readiness state | Unit Test |
| DRV-ASYNC-BSB-ADAPTER-004 | The component shall emit events on error conditions | Unit Test |

## 3 Design

### 3.1 Assumptions

The design of `AsyncByteStreamBufferAdapter` assumes the following:

1. The async byte stream driver will properly signal ready status before accepting data.
2. Buffer ownership follows the standard Return-To-Sender pattern: buffers are owned by the receiver until returned.
3. The async byte stream driver operates correctly and returns buffers sent to it when it is done processing.

### 3.2 State

`AsyncByteStreamBufferAdapter` maintains a single state variable:

* **m_driverIsReady**: An atomic boolean flag indicating whether the async byte stream driver is ready to accept data. Initialized to `false` and set to `true` when a ready signal is received from the driver.
