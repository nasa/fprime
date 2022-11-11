\page SvcComQueueComponent Svc::ComQueue Component
# Svc::ComQueue (Active Component)

## 1. Introduction

`Svc::ComQueue` is an  F´ active component that functions as a priority queue of buffer types. Messages are dequeued and
forwarded when a `Fw::Success::SUCCESS` signal is received in order of priority. `Fw::Success::FAILURE` signals result
in the queues being paused until a following `Fw::Success::SUCCESS` signal.

`Svc::ComQueue` is configured with a queue depth and queue priority for each incoming `Fw::Com` and `Fw::Buffer` port by
passing in a configuration table at initialization. Queued messages from the highest priority source port are serviced
first and a round-robin algorithm is used to balance between ports of shared priority.

`Svc::ComQueue` is designed to act alongside instances of the
[communication adapter interface](https://nasa.github.io/fprime/Design/communication-adapter-interface.html) and
implements the communication queue
[protocol](https://nasa.github.io/fprime/Design/communication-adapter-interface.html#communication-queue-protocol).

## 2. Assumptions

1. Incoming buffers to a given port are in priority order
2. Data is considered to be successfully sent when a `Fw::Success::SUCCESS` signal was received
3. The com adapter is responsible for any retransmission of failed data
4. The system includes downstream components implementing the
 [communications adapter](https://nasa.github.io/fprime/Design/communication-adapter-interface.html)


## 3. Requirements


| Requirement      | Description                                                                                                                             | Rationale                                                               | Verification Method |
|------------------|-----------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------|---------------------|
| SVC-COMQUEUE-001 | `Svc::ComQueue` shall queue `Fw::Buffer` and `Fw::ComBuffer` received on incoming ports.                                                | The purpose of the queue is to store messages.                          | Unit Test           |
| SVC-COMQUEUE-002 | `Svc::ComQueue` shall output exactly one `Fw::Buffer` or `Fw::ComBuffer` message on a received `Fw::Success::SUCCESS` signal.           | `Svc::ComQueue` obeys the communication adapter interface protocol.     | Unit Test           |
| SVC-COMQUEUE-003 | `Svc::ComQueue` shall pause sending  on the  `Fw::Success::FAILURE` and restart on the next `Fw::Success::SUCCESS` signal.              | `Svc::ComQueue` should not sent to a failing communication adapter.     | Unit Test           |
| SVC-COMQUEUE-004 | `Svc::ComQueue` shall have a configurable number of `Fw::Com` and `Fw::Buffer` input ports.                                             | `Svc::ComQueue` should be adaptable for a number of projects.           | Inspection          |
| SVC-COMQUEUE-005 | `Svc::ComQueue` shall select and send the next priority `Fw::Buffer` and `Fw::ComBuffer` message in response to `Fw::Success::SUCCESS`. | `Svc::ComQueue` obeys the communication adapter interface protocol.     | Unit test           |
| SVC-COMQUEUE-006 | `Svc::ComQueue` shall periodically telemeter the number of queued messages per-port in response to a `run` port invocation.             | `Svc::ComQueue` should provide useful telemetry.                        | Unit Test           | 
| SVC-COMQUEUE-007 | `Svc::ComQueue` shall emit a queue overflow event for a given port when the configured depth is exceeded. Messages shall be discarded.  | `Svc::ComQueue` needs to indicate off-nominal events.                   | Unit Test           | 
| SVC-COMQUEUE-008 | `Svc::ComQueue` shall implement a round robin approach to balance between ports of the same priority.                                   | Allows projects to balance between a set of queues of similar priority. | Unit Test           |
| SVC-COMQUEUE-009 | `Svc::ComQueue` shall keep track and throttle queue overflow events per port.                                                           | Prevents a flood of queue overflow events.                              | Unit test           | 

## 4. Design
The diagram below shows the `Svc::ComQueue` component.

![Svc::ComQueue](./img/ComQueue.png)

### 4.1. Ports
`Svc::ComQueue` has the following ports:

| Kind          | Name              | Port Type                             | Usage                                                  |
|---------------|-------------------|---------------------------------------|--------------------------------------------------------|
| `output`      | `comQueueSend`    | `Fw.Com`                              | Fw::ComBuffer output port                              |
| `output`      | `buffQueueSend`   | `Fw.BufferSend`                       | Fw::Buffer output port                                 |
| `async input` | `comStatusIn`     | `Fw.SuccessCondition`                 | Port for receiving the status signal                   |
| `async input` | `comQueueIn`      | `[ComQueueComPorts] Fw.Com`           | Port array for receiving Fw::ComBuffers                |
| `async input` | `buffQueueIn`     | `[ComQueueBufferPorts] Fw.BufferSend` | Port array for receiving Fw::Buffers                   |
| `async input` | `run`             | `Svc.Sched`                           | Port for scheduling telemetry output                   |
| `event`       | `Log`             | `Fw.Log`                              | Port for emitting events                               |
| `text event`  | `LogText`         | `Fw.LogText`                          | Port for emitting text events                          |
| `time get`    | `Time`            | `Fw.Time`                             | Port for getting the time                              |
| `telemetry`   | `Tlm`             | `Fw.Tlm`                              | Port for emitting telemetry                            |


### 4.2. State
`Svc::ComQueue` maintains the following state:
1. `m_queues`: An array of `Types::Queue` used to queue per-port messages.
2. `m_prioritizedList`: An instance of `Svc::ComQueue::QueueMetadata` storing the priority-order queue metadata.
3. `m_state`: Instance of `Svc::ComQueue::SendState` representing the state of the component. See: 4.3.1 State Machine
4. `m_throttle`: An array of flags that throttle the per-port queue overflow messages.

### 4.2.1 State Machine

The `Svc::ComQueue` component runs the following state machine. It has two states:

| State   | Description                                                                                   |
|---------|-----------------------------------------------------------------------------------------------|
| WAITING | `Svc::ComQueue` is waiting on `SUCCESS` before attempting to send an available buffer         |
| READY   | `Svc::ComQueue` had no queued buffers and will send the next buffer immediately when received |

The state machine will transition between states when a status is received and will transition from `READY` when a new
buffer is received. `FAILURE` statuses keep the `Svc::ComQueue` in `WAITING` state whereas a `SUCCESS` status will
either send a buffer and transition to `WAITING` or will have no buffers to send and will transition into `READY` state.
Buffers are queued when in `WAITING` state.

![`Svc::ComQueue` Functional State Machine](./img/state-machine.png)

### 4.3 Model Configuration
`Svc::ComQueue` has the following constants, that are configured in `AcConstants.fpp`:
1. `ComQueueComPorts`: number of ports of `Fw.Com` type in the `comQueueIn` port array.
2. `ComQueueBufferPorts`: number of ports of `Fw.BufferSend` type in the `buffQueueIn` port array.

### 4.4 Runtime Setup
To set up an instance of `ComQueue`, the following needs to be done: 
1. Call the constructor and the init method in the usual way for an F Prime active component. 
2. Call the `configure` method, passing in an array of `QueueConfiguration` type, the size of the array, 
and an allocator of `Fw::MemAllocator`. The `configure` method foes the following:

   1. Ensures that the total size and config size are the same value
   2. Ensures that priority values range from 0 to the total size value
   3. Ensures that every entry in the queue containing the prioritized order of the com buffer and buffer data have been 
   initialized. 
   4. Ensures that there is enough memory for the com buffer and buffer data we want to process

### 4.5 Port Handlers

#### 4.5.1 buffQueueIn
The `buffQueueIn` port handler receives an `Fw::Buffer` data type and a port number. 
It does the following:
1. Ensures that the port number is between zero and the value of the buffer size 
2. Enqueue the buffer onto the `m_queues` instance 
3. Returns a warning if `m_queues` is full

In the case where the component is already in `READY` state, this will process the queue immediately after the buffer
is added to the queue.

#### 4.5.2 comQueueIn
The `comQueueIn` port handler receives an `Fw::ComBuffer` data type and a port number. 
It does the following:
1. Ensures that the port number is between zero and the value of the com buffer size
2. Enqueue the com buffer onto the `m_queues` instance
3. Returns a warning if `m_queues` is full

In the case where the component is already in `READY` state, this will process the
queue immediately after the buffer is added to the queue.

#### 4.5.3 comStatusIn
The `comStatusIn` port handler receives a `Fw::Success` status. This triggers the component's state machine to change
state. For a full description see [4.2.1 State Machine](#4.2.1-State-Machine).
 
#### 4.5.4 run
The `run` port handler does the following: 
1. Report the high-water mark for each queue since last `run` invocation via telemetry
2. Clear each queue's high-water mark

### 4.6 Telemetry

| Name           | Type               | Description                                               |
|----------------|--------------------|-----------------------------------------------------------|
| comQueueDepth  | Svc.ComQueueDepth  | High-water mark depths of queues handling `Fw::ComBuffer` |
| buffQueueDepth | Svc.BuffQueueDepth | High-water mark depths of queues handling `Fw::Buffer`    |

### 4.7 Events

| Name           | Description                                                                     |
|----------------|---------------------------------------------------------------------------------|
| QueueOverflow  | WARNING_HI event triggered when a queue can no longer hold the incoming message |

### 4.8 Helper Functions

#### 4.8.1 sendComBuffer
Stores the com buffer message, sends the com buffer message on the output port, and then sets the send state to waiting.

#### 4.8.2 sendBuffer
Stores the buffer message, sends the buffer message on the output port, and then sets the send state to waiting.

#### 4.8.3 processQueue
In a bounded loop that is constrained by the total size of the queue that contains both 
buffer and com buffer data, do:

   1. Check if there are any items on the queue, and continue with the loop if there are none. 
   2. Store the entry point of the queue based on the index of the array that contains the prioritized data.
   3. Compare the entry index with the value of the size of the queue that contains com buffer data.
      1. If it is less than the size value, then invoke the sendComBuffer function.
      2. If it is greater than the size value, then invoke the sendBuffer function. 
   4. Break out of the loop, but enter a new loop that starts at the next entry and linearly swap the remaining items in
the prioritized list.
