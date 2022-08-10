\page SvcFileUplinkComponent Svc::FileUplink Component
# Svc::FileUplink Component

## 1 Introduction

`FileUplink` is an active ISF component.
It manages uplink of files to the spacecraft.

## 2 Requirements

Requirement | Description | Rationale | Verification Method
---- | ---- | ---- | ----
ISF-FU-001 | `FileUplink` shall receive file packets, assemble them into files, and store the files in the on-board non-volatile storage. | This requirement provides the capability to uplink files to the spacecraft. | Test

## 3 Design

### 3.1 Assumptions

The design of `FileUplink` assumes the following:

1. File uplink occurs by dividing files into packets
of type [`Fw::FilePacket`](../../../Fw/FilePacket/docs/sdd.html).

2. In the nominal case of file uplink

    a. Files are received one at a time.
All packets of one file are received before receiving any
packets of the next file.

    b. Within a file, packets are received in order.

### 3.2 Block Description Diagram (BDD)

![`FileUplink` BDD](img/FileUplinkBDD.jpg "FileUplink")

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
<a name="bufferSendIn">`bufferSendIn`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.html) | async input | Receives buffers containing file packets.
<a name="bufferSendOut">`bufferSendOut`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.html) | output | Returns buffers for deallocation.

### 3.4 State

`FileUplink` maintains the following state:

* <a name="receiveMode">*receiveMode*</a>:
One of START or DATA, recording the type of the next packet that
`FileUplink` expects to receive.
The initial value is START.

* <a name="lastSequenceIndex">*lastSequenceIndex*</a>:
An integer recording the sequence index of the last packet received.
The initial value is zero.

* <a name="writeFileDescriptor">*writeFileDescriptor*</a>:
The file descriptor of the file, if any, that is currently open
for writing.

### 3.5 The bufferSendIn Port

`FileUplink` asynchronously receives buffers on
[`bufferSendIn`](#bufferSendIn).
Each buffer contains a file packet.
When `FileUplink` receives a buffer, it (a) determines the type
of the file packet in the buffer; (b) takes action as
specified in section below corresponding to the packet type; and (c) 
invokes [`bufferSendOut`](#bufferSendOut)
to return the buffer for deallocation.

#### 3.5.1 START Packets

Upon receipt of a START packet, `FileUplink` does the following:

1. If [*receiveMode*](#receiveMode) is not START,
then close the file at
[*writeFileDescriptor*](#writeFileDescriptor)
and issue an *InvalidReceiveMode* warning.

2. Open the file for writing and set
[*writeFileDescriptor*](#writeFileDescriptor).

3. If step 2 succeeded, then set
[*lastSequenceIndex*](#lastSequenceIndex)
to zero and go to DATA mode; otherwise issue a
*FileOpenError* warning and go to START mode.

#### 3.5.2 DATA Packets

Upon receipt of a DATA packet *P*, `FileUplink` does the following,
where *I* is the 
[sequence index](../../../Fw/FilePacket/docs/sdd.html) 
of *P*:

1. If 
[*receiveMode*](#receiveMode)
is not DATA, then issue an *InvalidReceiveMode* warning 
and go to START mode.

2. Otherwise

    a. If *I* is not equal to *lastSequenceIndex + 1*, then issue a 
*PacketOutOfOrder*
warning reporting *lastSequenceIndex* and *I*.

    b. If the packet offset and size are in bounds for the current file, then

    1. Using *writeFileDescriptor*, write the file data in the 
packet at offset specified in the packet.

    2. If there was an error writing the file, then issue a
*FileWriteError* warning.

    c. Otherwise issue a *PacketOutOfBounds* warning.

#### 3.5.3 END Packets

Upon receipt of an END packet *P*, `FileUplink` does the following:

1. If [*receiveMode*](#receiveMode) is *DATA*,
then do the following, where *I* is the sequence index of *P*:

    a. If *I* is not equal to *lastSequenceIndex + 1*, 
then issue a *PacketOutOfOrder* warning reporting 
*lastSequenceIndex* and *I*.

    b. Use *writeFileDescriptor* to do the following:

    1. Use the method described in &sect; 4.1.2 of the
[CCSDS File Delivery Protocol (CFDP) Recommended Standard](https://public.ccsds.org/Pubs/727x0b4s.pdf)
to compute the checksum value for the file.

    2. Compare the value computed in the previous step against the
checksum value in the packet.
If the two values are different, then issue a *BadChecksum* warning.

    c. Close the file.

2. Otherwise issue an *InvalidReceiveMode* warning.

3. Set *lastSequenceIndex* to zero and go to START mode.

#### 3.5.4 CANCEL Packets

Upon receipt of a cancel packet *P*, `FileUplink` does the following:

1. Set *lastSequenceIndex* to zero.

2. If *receiveMode* is not START, then close the file at
*writeFileDescriptor*.

3. Issue an *UplinkCanceled* event.

4. Go to START mode.

## 4 Dictionary

TBD

## 5 Checklists

Document | Link
-------- | ----
Design | [Link](Checklist/design.xlsx)
Code | [Link](Checklist/code.xlsx)
Unit Test | [Link](Checklist/unit_test.xls)

## 6 Unit Testing

TODO
