\page SvcComQueueComponent Svc::ComQueue Component
# Svc::ComQueue (Active Component)

## 1. Introduction

`Svc::ComQueue` is an  F' active component that ensures that data continues to be collected
when the communication interface is not ready. As the communication interface is being set up, 
the component takes incoming Fw.Buffer, and Fw.Com data types and combines the two into an 
array implementation queue. When a ready signal of type ComStatus is received, the component will dequeue 
an item on their respective outport type based on the highest priority to be sent down. Priority
values are configured by the user. If items on the queue share the same priority value, items 
will be dispatched in a round-robin manner. Items will not be dispatched again until another ready signal is received. 
`Svc::ComQueue` has three different types of states: FAIL, RETRY and READY. In the context of a FAIL state where data was 
not successfully sent out on the output port, the FAIL state automatically transitions into a 
RETRY state where when a ready signal is received again, the data that failed to be received earlier
gets sent out again until a ready signal is received. 


## 2. Assumptions

1. On the first run of the component, ComStatus is set to READY by default. 
2. Incoming data being received by the component is already sorted based on high to low priority level.
3. Data is considered to be successfully sent until a FAIL signal is received. 
4. A status signal will always be provided from a communication interface. 

## 3. Requirements


| Requirement      | Description                                                                                                                                                                      | Rationale                                                                                                                                                  | Verification Method |
|------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| SVC-COMQUEUE-001 | SVC::COMQUEUE shall queue up `Fw:Buffer` and `Fw:ComBuffer` types and output                                                                                                     | The purpose of the component is to ensure that no data gets dropped whenever the communication interface is not ready                                      | Unit Test           |
| SVC-COMQUEUE-002 | SVC::COMQUEUE shall output exactly one `Fw:Buffer` or `Fw:ComBuffer` messages on a received READY signal                                                                         | Purpose of this component is to act as a radio throttle                                                                                                    | Unit Test           |
| SVC-COMQUEUE-003 | SVC::COMQUEUE shall retry sending the current message if a FAIL signal was received.                                          TD                                                 | Ensures that no messages get dropped                                                                                                                       | Unit Test           |
| SVC-COMQUEUE-004 | SVC::COMQUEUE shall accept data with the type `Fw::ComBuffer` and `Fw::Buffer` on multiple ports. The number of ports per type can be an arbitrary value of n and m respectively | These are the data types used for downlink.                                                                                                                | Inspection          |
| SVC-COMQUEUE-005 | SVC::COMQUEUE shall have a separate queue for each buffer and com buffer port                                                                                                    | The seperate queue enables the interface to differentiate priortiy levels between buffer and com buffer                                                    | Unit Test           | 
| SVC-COMQUEUE-006 | SVC::COMQUEUE shall send `Fw:Buffer` and `Fw:ComBuffer` data on separate ports                                                                                                   | `Fw:Buffer` and `Fw:ComBuffer` data are typically handled by different components                                                                          | Unit test           |
| SVC-COMQUEUE-007 | SVC::COMQUEUE shall keep track of the queue depth of all queue messages for `Fw:Buffer` and `Fw:ComBuffer` queues                 TD                                             | Helps keep track on how close the queue is to being full                                                                                                   | Unit Test           | 
| SVC-COMQUEUE-008 | SVC::COMQUEUE shall stop accepting `Fw:Buffer` and `Fw:ComBuffer` messages when the associated queue is full                      TD                                             | Allows the processing of backlog that may have build up during the downlink process                                                                        | Unit Test           | 
| SVC-COMQUEUE-009 | SVC::COMQUEUE shall process `Fw:Buffer` and `Fw:ComBuffer` messages on the queue based on priority levels                         TD                                             | Allows flexibility on how the data should be collected ahd prioritized                                                                                     | Unit Test           | 
| SVC-COMQUEUE-010 | SVC::COMQUEUE shall implement a round robin logic if the queue share the same priority level                                       TD                                            | Ensure that data that share priority levels will get sent out at a equal rate                                                                              | Unit Test           |
| SVC-COMQUEUE-011 | SVC::COMQUEUE shall keep track and throttle QUEUE_FULL events per queue                                                            TD                                            | Ensures that a throttle occurs per queue and that if one queue overflows it does not prevent another queue from indicating that it has an overflow as well | Unit test           | 

## 4. Design
The diagram below shows the `ComQueue` component.

<div>
<img src="img/ComQueue.png" width=700/>
</div>

### 4.2. Ports
`ComQueue` has the following ports:

| Kind            | Name            | Port Type       | Usage                                                          |
|-----------------|-----------------|-----------------|----------------------------------------------------------------|
| `output`        | `comQueueSend`  | `Fw.Com`        | Port that sends out com buffers on READY status.               |
| `output`        | `buffQueueSend` | `Fw.BufferSend` | Port that sends out buffers on READY status.                   |
| `async input`   | `comStatusIn`   | `ComStatus`     | Port that takes in status signal from communication interface. |
| `async input`   | `run`           | `Fw.Com`        | Schedule in port, driven by a rate group.                      |
| `guarded input` | `comQueueIn`    | `Fw.BufferSend` | Ports that receives buffer data types.                         |
| `guarded input` | `buffQueueIn`   | `Svc.Sched`     | Port that receives com buffer data types.                      |

### 4.3. State
`ComQueue` maintains the following state:
1. `m_queues`: An instance of `Types::Queue` for storing com Buffers and buffers based on port numbers.
2. `m_comBufferMessage`: An instance of `Fw::ComBuffer` for storing com buffer data to be dispatched
3. `m_bufferMessage`: An instance of `Fw::Buffer` for storing buffer data to be dispatched
4. `m_prioritizedList`: An instance of `QueueData` that stores the priority ordered com buffers and buffers.
5. `m_lastEntry`: An instance of `QueueData` that stores the last entry point of prioritized list before the data was sent. 
6. `m_state`: An instance of `SendState` that indicates the state of the send status 
7. `m_throttle`: An array of flags that indicates if a message has been throttled or not

### 4.4. Model Configuration
`ComQueue` has the following constants, initialized at component instantiation time:
1. `Svc::ComQueue::ComQueueComSize`: Size of queue that contains com buffer data. 
2. `Svc::ComQueue::ComQueueBuffSize` Size of queue that contains buffer data.
3. `Svc::ComQueue::totalSize`: Combined size of `Svc::ComQueue::ComQueueComSize` and `Svc::ComQueue::ComQueueBuffSize`.

### 4.5. Runtime Setup
To set up an instance of `ComQueue`, the following needs to be done: 
1. Call the constructor and the init method in the usual way for an F Prime active component. 
2. Call the `configure` method, passing in an array of `QueueConfiguration` type, the size of the array, 
and an allocator of `Fw::MemAllocator`. The `configure` method foes the following:

   1. Ensures that the total size and config size are the same value
   2. Ensures that priority values range from 0 to the total size value
   3. Ensures that every entry in the queue containing the prioritized order of the com buffer and buffer data have been 
   initialized. 
   4. Ensures that there is enough memory for the com buffer and buffer data we want to process
   
### 4.6. Port Handlers

#### 4.6.1 buffQueueIn
The `buffQueueIn` port handler receives an `Fw::Buffer` data type and a port number. 
It does the following:
1. Ensures that the port number is between zero and the value of the buffer size 
2. Enqueue the buffer onto the `m_queues` instance 
3. Returns a warning if `m_queues` is full 

#### 4.6.2 comQueueIn
The `comQueueIn` port handler receives an `Fw::ComBuffer` data type and a port number. 
It does the following:
1. Ensures that the port number is between zero and the value of the com buffer size
2. Enqueue the com buffer onto the `m_queues` instance
3. Returns a warning if `m_queues` is full

#### 4.6.3 comStatusIn
The `comStatusIn` port handler receives an `Svc::ComSendStatus` and it does the following:
1. If the status = `READY`, the function `retryQueue` or `processQueue` is called depending on if
`m_needRetry` has been set to true or false. If `m_needRetry` is equal to true, the function `retryQueue`
is invoked and `m_needRetry` is set to false, otherwise `processQueue` gets called. 
2. If the status = `FAIL`, `m_needRetry` is set to true. This step enables data to be resent. 
 
#### 4.6.4 run
The `run` port handler does the following: 
1. Construct an `ComQueueDepth` variable and `BuffQueueDepth` variable. 
The two variables are used to keep track of the largest tracked allocated size of
their respective type. 
2. Reports the largest tracked allocated size as telemetry. 

### 4.7. Helper Functions

#### 4.7.1 sendComBuffer
Stores the com buffer message, sends the com buffer message on the output port, and then sets the send state to waiting.

#### 4.7.2 sendBuffer
Stores the buffer message, sends the buffer message on the output port, and then sets the send state to waiting.

#### 4.7.3 retryQueue
Depending on the size of the last entry that is a type `QueueData`, com buffer data
or buffer data will be sent out again. 

#### 4.7.4 processQueue
In a bounded loop that is constrained by the total size of the queue that contains both 
buffer amd com buffer data, do:

   1. Check if there are any items on the queue, and continue with the loop if there are none. 
   2. Store the entry point of the queue based on the index of the array that contains the prioritized data.
   3. Compare the entry index with the value of the size of the queue that contains com buffer data.
      1. If it is less than the size value, then invoke the sendComBuffer function.
      2. If it is greater than the size value, then invoke the sendBuffer function. 
   4. Break out of the loop, but enter a new loop that starts at the next entry and linearly swap the remaining 
items in the prioritized list. 
## 5. Change Log

| Date       | Description |
|------------|---|
| 2022-07-12 | Initial Draft |
