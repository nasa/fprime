\page SvcFileDownlinkComponent Svc::FileDownlink Component
# Svc::FileDownlink Component

## 1 Introduction

`FileDownlink` is an active F´ component that manages spacecraft file downlink. Both operators and
components on the spacecraft can add files to the file queue, which `FileDownlink` will downlink
from. Operators can enqueue files using the `SendFile` and `SendPartial` commands, and components
can enqueue files using the `SendFile` port. The `FileComplete` port broadcasts when a file downlink
initiated by a port completes, allowing components to detect when a previous enqueued file downlink
has completed. To prevent a continuous stream of file downlink traffic from saturating the
communication link, a cooldown can be configured to add a delay between the completion of a file
downlink and starting on the next file in the queue.

**Note:** file downlink is limited to processing files with a maximum file size of 4GiB. Larger files will result in a
bad size error.

## 2 Requirements

Requirement | Description | Rationale | Verification Method
---- | ---- | ---- | ----
FD-001 | `FileDownlink` shall queue up a list of files to downlink | The requirement provides the ability to simultaneously queue up multiple files for downlink from different sources | Test
FD-002 | `FileDownlink` shall read a file from non-volatile storage, partition the file into packets, and send out the packets. | This requirement provides the capability to downlink files from the spacecraft. | Test
FD-003 | `FileDownlink` shall wait for a cooldown after completing a file downlink before starting another file downlink | Allows a saturated link to process a backlog that may have built up during a file downlink | Test

## 3 Design

### 3.1 Assumptions

The design of `FileDownlink` assumes the following:

1. File downlink occurs by dividing files into packets
of type [`Fw::FilePacket`](../../../Fw/FilePacket/docs/sdd.md).

2. One file downlink happens at a time.

3. Both components and operators must be able to enqueue files, necessitating both a `SendFile`
   command and port.

### 3.3 Ports

#### 3.3.1 Role Ports

Name | Type | Role
-----| ---- | ----
`timeCaller` | `Fw::Time` | TimeGet
`cmdIn` | [`Fw::Cmd`](../../../Fw/Cmd/docs/sdd.md) | Cmd
`cmdRegOut` | [`Fw::CmdReg`](../../../Fw/Cmd/docs/sdd.md) | CmdReg
`cmdResponseOut` | [`Fw::CmdResponse`](../../../Fw/Cmd/docs/sdd.md) | CmdResponse
`tlmOut` | [`Fw::Tlm`](../../../Fw/Tlm/docs/sdd.md) | Telemetry
`eventOut` | [`Fw::LogEvent`](../../../Fw/Log/docs/sdd.md) | LogEvent

#### 3.3.2 Component-Specific Ports

Name | Type | Kind | Purpose
---- | ---- | ---- | ----
`sendFile` | `Svc::SendFileRequest` | guarded_input | Enqueues file for downlink
`fileComplete` | `Svc::SendFileComplete` | output | Emits notifications when a file downlink initiated by a port completes
`Run` | `Svc::Sched` | async_input | Periodic clock input used to trigger internal state machine
<a name="bufferGet">`bufferGet`</a> | [`Fw::BufferGet`](../../../Fw/Buffer/docs/sdd.md) | output (caller) | Requests buffers for sending file packets.
<a name="bufferSendOut">`bufferSendOut`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.md) | output | Sends buffers containing file packets.

### 3.4 Constants

`FileDownlink` has the following constants, initialized
at component instantiation time:

* *downlinkPacketSize*: The size of the packets to use on downlink.
* *timeout*: Max amount of time in ms to wait for a buffer return before aborting downlink
* *cooldown*: The amount of time in ms to wait in a cooldown state before starting next downlink.
* *cycle time*: Frequency in ms of clock pulses sent to `Run` port, used for timing timeouts and
  cooldown.
* *file queue depth*: The maximum number of files that can be held in the internal file downlink
  queue. Attempting to dispatch a SendFile command or port call while the queue is full will result
  in a busy error response.

### 3.5 State

`FileDownlink` maintains a *mode* equal to
one of the following values:

* IDLE (0): `FileDownlink` is idle.

* DOWNLINK (1): `FileDownlink` is performing a file downlink.

* CANCEL (2): `FileDownlink` is canceling a file downlink.

* WAIT (3): `FileDownlink` is waiting for a buffer to be returned before sending another packet.

* COOLDOWN (4): `FileDownlink` is waiting in a cooldown period before downlinking the next file.

The initial value is IDLE.

### 3.6 Commands

`FileDownlink` recognizes the commands described in the following sections.

#### 3.6.1 SendFile/SendPartial

SendFile is an asynchronous command that adds a file to the file downlink queue.
It has two arguments:

1. *sourceFileName*: The name of the on-board file to send.
2. *destFileName*: The name of the destination file on the ground.

SendPartial also includes the following fields:

3. *offset*: Position in file to start reading from.
4. *length*: Amount of data to read. A length of 0 reads until the end of file.

When the downlink completes or fails, a CmdResponse packet will be sent indicating success or
failure.

#### 3.6.2 Cancel

Cancel is a synchronous command.
If *mode* = DOWNLINK, it sets *mode* to CANCEL.
Otherwise it does nothing.

## 4 Checklists

Document | Link
-------- | ----
Design | [Link](Checklist/design.xlsx)
Code | [Link](Checklist/code.xlsx)
Unit Test | [Link](Checklist/unit_test.xls)

## 6 Unit Testing

TODO
