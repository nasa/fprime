\page FwBufferSerializableBufferGetBufferSend Fw::Buffer Serializable / Fw::BufferGet Port / Fw::BufferSend Port
# Fw::Buffer Serializable / Fw::BufferGet Port / Fw::BufferSend Port

## 1 Introduction

This module provides the following elements:

* A type `Fw::Buffer` representing a variable-size buffer allocated from a fixed-size store.

* A port `Fw::BufferGet` for requesting a buffer of type `Fw::Buffer` from
a [`BufferManager`](../../../Svc/BufferManager/docs/sdd.html) component.

* A port `Fw::BufferSend` for sending a buffer of type `Fw::Buffer` from one
component to another.

## 2 Design

 <a name="FwBuffer"></a>
### 2.1 The Type Fw::Buffer

`Fw::Buffer` is a serializable class. It contains the following members:

Name | Type | Purpose
---- | ---- | -----
<a name="managerID">`managerID`</a> | `U32` | The instance number of the [`BufferManager`](../../../Svc/BufferManager/docs/sdd.html) that allocated this buffer.
<a name="bufferID">`bufferID`</a> | `U32` | A `U32` value that uniquely (modulo 2^32) identifies this buffer among all buffers allocated by the buffer manager with ID [`managerID`](#managerID). Each allocation receives a different identifier, even if the same memory is re-used for different allocations.
<a name="data">`data`</a> | `U64` | Either zero or a pointer to the initial byte of the memory area reserved for the buffer, cast to a `U64` value so it can be serialized.
<a size="size">`size`</a> | `U32` | The size of the buffer in bytes.

A value of type `Fw::Buffer` is *valid* if its `data` member is non-zero; otherwise it is
*invalid*.

### 2.2 The Port Fw::BufferGet

As shown in the following diagram, `Fw::BufferGet` has one argument `size` of type `U32`. It returns a
value of type [`Fw::Buffer`](#FwBuffer).

![`Fw::BufferGet` Diagram](img/BufferGetBDD.jpg "Fw::BufferGet Port")

### 2.3 The Port Fw::BufferSend

As shown in the following diagram, `Fw::BufferSend` has one argument `fwBuffer`
of type `Fw::Buffer`.

![`Fw::BufferSend` Diagram](img/BufferSendBDD.jpg "Fw::BufferSend Port")

