\page SvcFileDownlinkComponent Svc::FileDownlink Component
# Svc::FileDownlink Component

## 1 Introduction

`FileDownlink` is an active ISF component.
It manages downlink of files from the spacecraft.

## 2 Requirements

Requirement | Description | Rationale | Verification Method
---- | ---- | ---- | ----
FD-001 | Upon command, `FileDownlink` shall read a file from non-volatile storage, partition the file into packets, and send out the packets. | This requirement provides the capability to downlink files from the spacecraft. | Test

## 3 Design

### 3.1 Assumptions

The design of `FileDownlink` assumes the following:

1. File downlink occurs by dividing files into packets
of type [`Fw::FilePacket`](../../../Fw/FilePacket/docs/sdd.html).

2. One file downlink happens at a time.

### 3.2 Block Description Diagram (BDD)

![`FileDownlink` BDD](img/FileDownlinkBDD.jpg "FileDownlink")

### 3.3 Ports

#### 3.3.1 Role Ports

Name | Type | Role
-----| ---- | ----
`timeCaller` | `Fw::Time` | TimeGet
`cmdIn` | [`Fw::Cmd`](../../../Fw/Cmd/docs/sdd.html) | Cmd
`cmdRegOut` | [`Fw::CmdReg`](../../../Fw/Cmd/docs/sdd.html) | CmdReg
`cmdResponseOut` | [`Fw::CmdResponse`](../../../Fw/Cmd/docs/sdd.html) | CmdResponse
`tlmOut` | [`Fw::Tlm`](../../../Fw/Tlm/docs/sdd.html) | Telemetry
`eventOut` | [`Fw::LogEvent`](../../../Fw/Log/docs/sdd.html) | LogEvent

#### 3.3.2 Component-Specific Ports

Name | Type | Kind | Purpose
---- | ---- | ---- | ----
<a name="bufferGet">`bufferGet`</a> | [`Fw::BufferGet`](../../../Fw/Buffer/docs/sdd.html) | output (caller) | Requests buffers for sending file packets.
<a name="bufferSendOut">`bufferSendOut`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.html) | output | Sends buffers containing file packets.

### 3.4 Constants

`FileDownlink` has the following constants, initialized
at component instantiation time:

* *downlinkPacketSize*:
The size of the packets to use on downlink.

### 3.5 State

`FileDownlink` maintains a *mode* equal to
one of the following values:

* IDLE (0): `FileDownlink` is idle.

* DOWNLINK (1): `FileDownlink` is performing a file downlink.

* CANCEL (2): `FileDownlink` is canceling a file downlink.

The initial value is IDLE.

### 3.6 Commands

`FileDownlink` recognizes the commands described in the following sections.

#### 3.6.1 SendFile

SendFile is an asynchronous command.
It has two arguments:

1. *sourceFileName*:
The name of the on-board file to send.

2. *destFileName*:
The name of the destination file on the ground.

When `File Downlink` receives this command, it carries
out the following steps:

1. If *mode* = CANCEL, then 

    a. Issue a *DownlinkCanceled* event.

    b. set *mode* = IDLE and return.

2. Set *mode* = DOWNLINK.

3. Open the file *sourceFileName* for reading with file descriptor *d*.
If there is any problem opening the file, then issue a
*FileOpenError* warning and abort the command execution.

4. Invoke *bufferGetCaller*
to request a buffer whose size is the size of a START packet.
Fill the buffer and send it out on *bufferSendOut*.

5. Set a remainder *r* to the size of the file with descriptor *d*.

6. While *r > 0* and *mode = DOWNLINK* do:

    a. Let *n* be the smaller of *downlinkPacketSize* and *r*.

    b. Invoke *bufferGetCaller* to request a buffer *B* whose size is 
the size of a DATA packet with a data payload of *n* bytes.

    c. Read the next *n* bytes out of the file with descriptor *d*.
If there is any problem reading the file, then issue a
*FileReadError* warning, close the file, and abort the command execution.

    d. Fill *B* with (i) the data read the previous step and (ii) the appropriate
metadata. Send *B* out on *bufferSendOut*.

    e. Reduce *r* by *n*.

7. Close the file with descriptor *d*.

8. If *mode* = CANCEL, then

    a. Invoke *bufferGetCaller* to request a buffer whose size 
is the size of a CANCEL packet.
Fill the packet and send it out on *bufferSendOut*.

    b. Issue a *DownlinkCanceled* event.

9. Otherwise invoke *bufferGetCaller* to request a buffer whose 
size is the size of an END packet.
Fill the buffer and send it out on *bufferSendOut*.

10. Set *mode = IDLE*.

#### 3.6.2 Cancel

Cancel is a synchronous command.
If *mode* = DOWNLINK, it sets *mode* to CANCEL.
Otherwise it does nothing.

## 4 Dictionary

Dictionaries: [HTML](FileDownlink.html) [MD](FileDownlink.md)

## 5 Checklists

Document | Link
-------- | ----
Design | [Link](Checklist/design.xlsx)
Code | [Link](Checklist/code.xlsx)
Unit Test | [Link](Checklist/unit_test.xls)

## 6 Unit Testing

TODO
