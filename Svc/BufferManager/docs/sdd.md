\page SvcBufferManagerComponent Svc::BufferManager Component
# Svc::BufferManager

## 1 Introduction

`BufferManager` is a passive ISF component.
It allocates and deallocates variable-sized buffers
from a fixed-size store.

## 2 Requirements

Requirement | Description | Rationale | Verification Method
---- | ---- | ---- | ----
ISF-BM-001 | `BufferManager` shall maintain a fixed-size store and shall provide a callee port on which another component may request and receive variable-size buffers allocated from the store. | This requirement provides variable-sized buffers that may be passed between components by reference. Such buffers are useful for transferring large data items of varying length, such as file packets and images. For such data items, a fixed-size buffer such as `Fw::ComBuffer` is not practical. | Test
ISF-BM-002 | `BufferManager` shall provide an input port on which a component that has been given a buffer may return the buffer for deallocation. | Deallocation prevents the fixed-size store from becoming exhausted. Note that the component returning the buffer is generally the receiver, while the component requesting the buffer is generally the sender. See the [sequence diagram](#SequenceDiagram) below. | Test

## 3 Design

### 3.1 Assumptions

The design of `BufferManager` assumes the following:

1. `BufferManager` has some maximum number of outstanding allocations, 
set at component initialization, that is never exceeded.

2. The store maintained by `BufferManager` has a fixed size, 
set at component initialization.
This fixed size is never exceeded by the outstanding allocations.

3. Buffers are freed in the same order that they were allocated.

### 3.2 Block Description Diagram (BDD)

![`BufferManager` BDD](img/BufferManagerBDD.jpg "BufferManager")

### 3.3 Ports

#### 3.3.1 Role Ports

Name | Type | Role
-----| ---- | ----
`timeCaller` | `Fw::Time` | TimeGet
`tlmOut` | [`Fw::Tlm`](../../../Fw/Tlm/docs/sdd.html) | Telemetry
`eventOut` | [`Fw::LogEvent`](../../../Fw/Log/docs/sdd.html) | LogEvent

#### 3.3.2 Component-Specific Ports

Name | Type | Kind | Purpose
---- | ---- | ---- | ----
<a name="bufferSendIn">`bufferSendIn`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.html) | guarded input | Receives buffers for deallocation
<a name="bufferGetCallee">`bufferGetCallee`</a> | [`Fw::BufferGet`](../../../Fw/Buffer/docs/sdd.html) | guarded input (callee) | Receives requests for allocated buffers and returns the buffers


### 3.4 Constants

`BufferManager` maintains the following constants, initialized
when the component is instantiated:

* <a name="storeSize">*storeSize*</a>:
The size of the store used to allocate buffers.

* <a name="allocationQueueDepth">*allocationQueueDepth*</a>:
The maximum number of buffers that may be allocated at any time.

### 3.5 State

`BufferManager` maintains the following state:

* <a name="store">*store*</a>:
A fixed allocation of [*storeSize*](#storeSize) bytes.

* <a name="allocationQueue">*allocationQueue*</a>:
A doubly-ended queue of up to [*allocationQueueDepth*](#allocationQueueDepth)
entries that maintains, for each outstanding allocation,
an entry *E = (I, s)* consisting of a unique identifier *I*
and a size *s*.

* <a name="freeIndex">*freeIndex*</a>:
An index pointing to the first free byte of the store.
The initial value is zero.

The identifiers *I* are 32-bit unsigned integers.

### 3.6 Port Behavior

#### 3.6.1 bufferGetCallee

When `BufferManager` receives a request for a buffer of size *s* on
[*bufferGetCallee*](#bufferGetCallee), it carries out the following steps:

1. If *freeIndex + s > storeSize*, then issue a
*StoreSizeExceeded* event and return an invalid buffer.

2. Otherwise 

    a. Assert that the size *s'* of [*allocationQueue*](#allocationQueue)
is less than or equal to [*allocationQueueDepth*](#allocationQueueDepth).

    b. If *s' = allocationQueueDepth*, then issue an *AllocationQueueFull*
event and return an invalid buffer.

    c. Otherwise

      1. Create a fresh identifier *I*.

      2. Compute the pointer *P* that points to byte [*freeIndex*](#freeIndex) of [*store*](#store).

      3. Create an allocation queue entry *E = (I, s)*.

      4. Push *E* onto the front of [*allocationQueue*](#allocationQueue).

      5. Increase [*freeIndex*](#freeIndex) by *s*.

      6. Create and return a valid
[`Fw::Buffer`](../../../Fw/Buffer/docs/sdd.html) with `managerID` equal to the
instance number of this component, `bufferID` equal to *I*, `data` equal to
*P*, and `size` equal to *s*.

#### 3.6.2 bufferSendIn

When `BufferManager` receives notification of a free buffer on
[*bufferSendIn*](#bufferSendIn), it carries out the following steps:

1. If [*allocationQueue*](#allocationQueue) is empty, then issue an
*AllocationQueueEmpty* event.

2. Otherwise

    a. Pull an entry *(I, s)* off the back of the queue.

    b. If *I* matches the identifier provided in the free notification, then
decrease [*freeIndex*](#freeIndex) by *s*.

    c. Otherwise issue an *IDMismatch* event.

 <a name="SequenceDiagram"></a>
### 3.7 Sequence Diagram

The following sequence diagram shows the procedure for sending a buffer
from one component to another:

1. The sending component requests a buffer *B* on the
[`bufferGetCallee`](#bufferGetCallee) port of `BufferManager`.

2. The sending component fills *B* with data and sends it to the receiving
component.

3. The receiving component uses the data in *B*. When done, it sends *B* back
to the [`bufferSendIn`](#bufferSendIn) port of `BufferManager` for deallocation.

![`BufferManager` Sending a Buffer](img/SendingABuffer.jpg "SequenceDiagram")

## 4 Dictionary

Dictionaries: [HTML](BufferManager.html) [MD](BufferManager.md)

## 5 Checklists

Document | Link
-------- | ----
Design | [Link](Checklist/design.xlsx)
Code | [Link](Checklist/code.xlsx)
Unit Test | [Link](Checklist/unit_test.xls)

## 6 Unit Testing

TODO
