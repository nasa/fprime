\page SvcComQueueComponent Svc::ComQueue Component
# Svc::ComQueue (Active Component)

## 1. Introduction

`Svc::ComQueue` is an active component. 
It accepts three types of inputs: Fw.Buffer, Fw.Com, and ComStatus. 
Type Fw.Buffer is and Fw.Com are both an array of ports, and ComStatus is an enum
that indicates the status of the ComQueue. 
From the two input buffers, ComQueue will put the different downlink data types 
into their respective queues (ie. data from Fw.Com will be put on to a queue that 
consists of Fw.Com types). 
If ComStatus is equal to the enum state Ready, and depending the priority of the queue, 
ComQueue will output the head of the queue that is prioritized (ie. 
if the queue that contains Fw.Com data has higher priority, then the head of that
queue will be outputted first).
If the enum state is equal to Fail then the head does not get outputted, and stays on 
the queue. 


## 2. Assumptions

## 3. Requirements

Requirement | Description | Rationale | Verification Method

## 4. Design
The diagram below shows the `ComQueue` component.

<div>
<img src="img/ComQueue.png" width=700/>
</div>

### 4.2. Ports
`ComQueue` has the following ports:

| Kind            | Name            | Port Type       | Usage                                               |
|-----------------|-----------------|-----------------|-----------------------------------------------------|
| `output`        | `comQueueSend`  | `Fw.Com`        | Port that produces the head of the com buffer queue |
| `output`        | `buffQueueSend` | `Fw.BufferSend` | Port that produces the head of the buffer queue     |
| `async input`   | `comStatusIn`   | `ComStatus`     | Port that takes in status signal of Queue           |
| `async input`   | `run`           | `Fw.Com`        | Schedule in port, driven by a rate group.           |
| `guarded input` | `comQueueIn`    | `Fw.BufferSend` | Ports that receives buffer data types               |
| `guarded input` | `buffQueueIn`   | `Svc.Sched`     | Port that receives com buffer data types            |

## 5. Ground Interface

None.

## 6. Example Uses