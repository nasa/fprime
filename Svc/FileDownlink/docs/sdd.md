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
FD-004 | `FileDownlink` shall issue a warning if a file with zero size is encountered | Ensures that operators are aware of invalid file sizes | Test
FD-005 | `FileDownlink` shall provide a command that force-completes the active downlink and drains the file queue without waiting on outstanding buffer returns | Allows operators to recover the component when a downstream component never returns a buffer | Test
FD-006 | `FileDownlink` shall emit a warning event when a downlink has been waiting longer than a configurable timeout for a buffer return | A downstream component that fails to return a buffer otherwise produces no fault indication, since ping responses continue | Test

## 3 Design

### 3.1 Assumptions

The design of `FileDownlink` assumes the following:

1. File downlink occurs by dividing files into packets
of type [`Fw::FilePacket`](../../../Fw/FilePacket/docs/sdd.md).

2. One file downlink happens at a time.

3. Both components and operators must be able to enqueue files, necessitating both a `SendFile`
   command and port.

4. File access is sandboxed to a directory configured via `configure(directory)`.
   All source paths supplied through `SendFile`/`SendPartial` commands or the
   `SendFile` port are validated against the sandbox directory before reading; rejected paths emit
   `SourceOutOfSandbox`. This mirrors `Svc::FileUplink` write-side sandboxing.

   > [!WARNING]
   > The sandbox is **fail-open**: until `configure(directory)` is called, the sandbox defaults to
   > `/`, which permits reading any absolute path accessible to the process via ground command.
   > This default is intentionally insecure for backwards compatibility. Security-conscious
   > deployments **must** call `configure(directory)` during topology setup to restrict file
   > access. Note that the stock `FileHandling` subtopology and reference topologies do **not**
   > configure a downlink sandbox: `FileHandling` only calls the
   > `configure(cooldown, cycleTime, fileQueueDepth, stallTimeout)` overload, which does not set a sandbox.

### 3.2 Ports

#### 3.2.1 Role Ports

Name | Type | Role
-----| ---- | ----
`timeCaller` | `Fw::Time` | TimeGet
`cmdIn` | [`Fw::Cmd`](../../../Fw/Cmd/docs/sdd.md) | Cmd
`cmdRegOut` | [`Fw::CmdReg`](../../../Fw/Cmd/docs/sdd.md) | CmdReg
`cmdResponseOut` | [`Fw::CmdResponse`](../../../Fw/Cmd/docs/sdd.md) | CmdResponse
`tlmOut` | [`Fw::Tlm`](../../../Fw/Tlm/docs/sdd.md) | Telemetry
`eventOut` | [`Fw::LogEvent`](../../../Fw/Log/docs/sdd.md) | LogEvent

#### 3.2.2 Component-Specific Ports

Name | Type | Kind | Purpose
---- | ---- | ---- | ----
`SendFile` | `Svc::SendFileRequest` | guarded_input | Enqueues file for downlink
`FileComplete` | `Svc::SendFileComplete` | output | Emits notifications when a file downlink initiated by a port completes
`Run` | `Svc::Sched` | async_input | Periodic clock input used to trigger internal state machine
<a name="bufferSendOut">`bufferSendOut`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.md) | output | Sends buffers containing file packets.
<a name="bufferReturn">`bufferReturn`</a> | [`Fw::BufferSend`](../../../Fw/Buffer/docs/sdd.md) | async_input | Receives back ownership of buffers sent on `bufferSendOut`.
`pingIn` | `Svc::Ping` | async_input | Ping input from health checker
`pingOut` | `Svc::Ping` | output | Ping response to health checker

### 3.3 Constants and Configuration

The downlink packet size is the compile-time configuration constant `FILEDOWNLINK_INTERNAL_BUFFER_SIZE`;
packets are stored in an internal memory store.

`FileDownlink` has the following values, set via `configure()` at initialization time:

* *cooldown*: The amount of time in ms to wait in a cooldown state before starting next downlink.
* *cycle time*: Frequency in ms of clock pulses sent to `Run` port, used for cooldown.
* *file queue depth*: The maximum number of files that can be held in the internal file downlink
  queue. Attempting to dispatch a SendFile command or port call while the queue is full will result
  in an error response (`STATUS_ERROR` on the port, `EXECUTION_ERROR` for the command).
* *stall timeout*: The time in ms a downlink may wait on a buffer return before a
  `DownlinkStalled` warning is emitted. The default of 0 disables the warning. The warning is
  observational only: it triggers no automatic action. Set it well above the worst-case buffer
  turnaround time of the downstream component: the warning fires once per wait, so a value below
  the nominal turnaround warns on every packet of a slow link. The warning fires at most once per
  wait, so its rate is bounded by the timeout itself; it is deliberately not count-throttled, so
  that a stall is always reported however long the transfer has been running.

### 3.4 State

`FileDownlink` maintains a *mode* equal to
one of the following values:

* IDLE (0): `FileDownlink` is idle.

* DOWNLINK (1): `FileDownlink` is performing a file downlink.

* CANCEL (2): `FileDownlink` is canceling a file downlink.

* WAIT (3): `FileDownlink` is waiting for a buffer to be returned before sending another packet.

* COOLDOWN (4): `FileDownlink` is waiting in a cooldown period before downlinking the next file.

The initial value is IDLE.

### 3.5 Commands

`FileDownlink` recognizes the commands described in the following sections.

#### 3.5.1 SendFile/SendPartial

SendFile is an asynchronous command that adds a file to the file downlink queue.
It has two arguments:

1. *sourceFileName*: The name of the on-board file to send.
2. *destFileName*: The name of the destination file on the ground.

SendPartial also includes the following fields:

3. *offset*: Position in file to start reading from.
4. *length*: Amount of data to read. A length of 0 reads until the end of file.

When the downlink completes or fails, a CmdResponse packet will be sent indicating success or
failure.

#### 3.5.2 Cancel

Cancel is an asynchronous command.
If *mode* = DOWNLINK or *mode* = WAIT, it sets *mode* to CANCEL.
Otherwise it does nothing.
On the return of the outstanding buffer, `FileDownlink` sends a cancel packet; when that cancel
packet's own buffer is in turn returned, it emits `DownlinkCanceled` and enters COOLDOWN. Cancel therefore follows
the normal buffer flow-control protocol and cannot complete if the downstream component never
returns the outstanding buffer. Use Reset to recover from that condition.

#### 3.5.3 Reset

Reset is an asynchronous command that recovers the component when the downstream component
never returns the outstanding buffer. It force-completes the active downlink, if any, without
waiting on the buffer return: `FileDownlink` sends a cancel packet, emits `DownlinkCanceled`,
responds to the originating command or port request, and enters COOLDOWN. It then drains the
file queue, responding to each queued request with an error, and emits `DownlinkReset` with the
number of requests dropped. A single Reset drops at most *file queue depth* requests; if
delivering a drop response causes a client to synchronously submit another request, that request
may be dropped by the same Reset. As with all downlink failure responses,
`FILEDOWNLINK_COMMAND_FAILURES_DISABLED` maps these error responses to OK; the `DownlinkReset`
count is then the only indication that queued requests were dropped. Port clients therefore see
`SendFileStatus::STATUS_OK` for requests that were dropped without a single packet being sent:
`Svc::DpCatalog`, for example, will mark the data product TRANSMITTED and remove it from the
catalog. Deployments that cannot tolerate that must set `FILEDOWNLINK_COMMAND_FAILURES_DISABLED`
to false, which makes both the drain and the pre-existing failure paths report `STATUS_ERROR`. Late returns of buffers that were outstanding at the time of the
Reset are ignored.

> [!WARNING]
> Reset breaks the buffer flow-control protocol: the internal packet buffers are eligible for
> reuse while a downstream component may still hold references to them. Issue Reset only when
> the downstream component has been written off. If that component later revives and reads a
> held buffer, it may emit stale packet data. In particular the file-packet store is re-wrapped
> by the very next `SendFile` after a Reset, so a downstream component that revives mid-transfer
> may read bytes `FileDownlink` is concurrently rewriting; nothing in the component delays a
> follow-on downlink.

## 4 Checklists

Checklist |
-------- |
[Design](Checklist/design.xlsx) |
[Code](Checklist/code.xlsx) |
[Unit Test](Checklist/unit_test.xls) |
