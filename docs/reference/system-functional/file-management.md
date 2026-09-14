
# File Management Functionality

## References

- [F Prime FileManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileManager/docs/sdd.md)
- [F Prime FileDownlink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDownlink/docs/sdd.md)
- [F Prime FileUplink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileUplink/docs/sdd.md)
- [F Prime FilePacket SDD](https://github.com/nasa/fprime/blob/devel/Fw/FilePacket/docs/sdd.md) (F Prime file packet format)
- [F Prime CfdpManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/CfdpManager/docs/sdd.md)
- [F Prime FileDispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDispatcher/docs/sdd.md)
- [F Prime FileWorker SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileWorker/docs/sdd.md)
- [CCSDS 727.0-B-5, CCSDS File Delivery Protocol (CFDP)](https://ccsds.org/Pubs/727x0b5e1.pdf)

## Overview

File management provides the capability to manipulate files on the spacecraft file system, transfer files between the ground and the spacecraft in both directions, route delivered files to appropriate handlers, and perform large file I/O operations without blocking time-critical components. These capabilities are delivered by a set of collaborating components that together support the full lifecycle of file operations.

File transfer is available in two implementations that share the same on-board interfaces: the F Prime file protocol (`FileUplink` and `FileDownlink`) and the CCSDS File Delivery Protocol (`CfdpManager`). Both are described under [File Transfer](#file-transfer) below, along with where each is supported. This page is an overview; the SDDs linked above are the authoritative descriptions of ports, commands, events, telemetry, and configuration.

### File System Operations

The File Manager provides ground commands for common file and directory operations:

- Create and remove directories; remove files
- Move (rename) files
- Concatenate files
- Query file sizes and compute file CRCs
- List directory contents

Each operation reports its result via events. File system operations execute in the File Manager's own thread to avoid blocking the commanding path.

### File Transfer

File transfer moves files between the ground and the flight software: uplink (ground to flight) delivers sequences, parameter files, and software updates; downlink (flight to ground) returns data products, logs, and diagnostic files. F Prime ships two implementations of this capability:

- **F Prime file protocol**, provided by the `FileUplink` and `FileDownlink` components. A lightweight, F Prime-specific protocol that is supported natively by the F Prime GDS.
- **CCSDS File Delivery Protocol (CFDP)**, provided by the `CfdpManager` component. A standards-based protocol with optional reliable delivery, intended for lossy or intermittent links and for interoperability with CFDP-capable ground systems.

#### F Prime File Protocol (FileUplink / FileDownlink)

Files are carried as a series of F Prime file packets: a START packet announcing the file name and size, DATA packets carrying offset-tagged chunks of the file, an END packet carrying a checksum, and a CANCEL packet to abort a transfer. The checksum is the 32-bit CFDP checksum, so integrity checking is shared with the CFDP mechanism.

**Downlink.** `FileDownlink` maintains a queue of files to send. Operators enqueue files by command (whole files, or a partial file given an offset and length) and on-board components enqueue files through a port; a completion port broadcasts a notification, carrying the transfer status and the request's context, when a port-requested transfer finishes (successfully or not). Files are sent one at a time, and a configurable cooldown between files keeps sustained file traffic from starving other downlink data. In-progress downlinks can be cancelled by command. File size is limited to 4 GiB.

**Uplink.** `FileUplink` receives file packets from the communication stack, writes each data chunk at its byte offset, verifies the checksum on the END packet, and leaves the completed file at the destination path named in the START packet. Successfully received files are announced on a port so that they can be routed to a consumer by the [File Dispatcher](#file-dispatch). Files are received one at a time, and packets within a file are expected in order.

**Delivery guarantees.** The protocol has no retransmission. Lost or out-of-order uplink packets are reported by `FileUplink` events and telemetry counters; loss on downlink is visible only to the ground system. Recovery is an operator action: re-send the file, or re-send the missing range using a partial downlink. This is appropriate for links where the framing layer already provides reliability or where loss is rare and operator-driven recovery is acceptable.

**Sandboxing.** `FileUplink`, `FileDownlink`, and the File Manager confine file access to a directory chosen at initialization; requested paths outside it are rejected. Validation is textual and does not resolve symbolic links, so containment assumes untrusted actors cannot create symlinks inside the sandbox directory. The sandbox is fail-closed: a deployment must configure it or no file can be read or written. The default configuration shipped with F Prime is the file system root, which deployments should restrict.

#### CCSDS File Delivery Protocol (CfdpManager)

`CfdpManager` is a single component that handles both uplink and downlink using CFDP protocol data units (PDUs). It is a port of the cFS CF application, adapted to F Prime ports, commands, events, telemetry, and parameters.

**Transfer classes.** Class 1 (unacknowledged) sends a file with no feedback from the receiver, comparable to the F Prime file protocol. Class 2 (acknowledged) adds end-of-file and finished acknowledgments, gap detection, negative acknowledgments, and retransmission, providing complete and verified delivery over lossy or intermittent links without operator intervention.

**Channels and transactions.** Transfers are organized into independently configured channels, each with its own pool of concurrent transactions, timers, retry limits, and outgoing-PDU throttle. The local entity identifier is a single component parameter; the destination entity identifier is given per commanded transaction. Multiple files can be in flight at once within the configured limits.

**Operations.** Operators can send a single file, play back every file in a directory, or poll a directory on a fixed interval so that new files are sent automatically. Individual transactions can be suspended, resumed, cancelled with protocol close-out, or abandoned, and a channel's transmission can be frozen and resumed. On-board components can request a transfer through the same `SendFileRequest`/`SendFileComplete` port types as `FileDownlink`; for these port-initiated transfers the channel, destination entity, class, retention policy, and priority come from component parameters, and the completion notification does not carry a per-request context.

**Timing.** Protocol timers and transaction processing are driven by a scheduler port that must be invoked at 1 Hz. Timer values (the acknowledgment and inactivity timeout parameters and the commanded directory poll interval) are counted in scheduler invocations, so driving the port at any other rate scales every CFDP timeout by the same factor.

**Current limitations.** Partial-file transfers (non-zero offset or length) are not supported, file sizes and offsets are 32-bit in the current implementation, received files are not announced to the File Dispatcher, and CFDP transfers are not confined by a file-system sandbox — the destination path in an incoming metadata PDU is honored as received. CFDP assumes authentication is provided by lower layers; deployments should pair it with link-layer security such as SDLS. See the CfdpManager SDD for the full list of assumptions and security considerations.

#### Interchangeability and Support

The two mechanisms are interchangeable for on-board file producers and for the communication stack: both implement the `SendFileRequest`/`SendFileComplete` ports and exchange file-type packets with the same communication-stack ports, so a deployment selects one or the other without changing those components. Two caveats: a producer that correlates completions by request context, or that needs partial-file requests, depends on `FileDownlink` behavior (see *Operations* and *Current limitations* above); and consumers of uplinked files are affected, because `CfdpManager` does not announce received files to the File Dispatcher. The choice comes down to the delivery guarantees needed and the ground system in use.

| | F Prime file protocol (`FileUplink` / `FileDownlink`) | CFDP (`CfdpManager`) |
|---|---|---|
| F Prime GDS (`fprime-gds`) | Supported: uplink and downlink from the GDS user interface, the `fprime-cli file-uplink` command, and the integration test API. | Not supported. The GDS does not implement CFDP. |
| YAMCS | Supported through the `fprime-yamcs` plugin; exercised by the `fprime-yamcs-reference` project. | Not supported by the `fprime-yamcs` plugin. YAMCS has a native CFDP service, but F Prime emits each PDU behind the 2-byte F Prime file packet-type descriptor inside the space packet data field, so an adapter that strips that descriptor is needed; no F Prime project demonstrates this yet. |
| Communication stack | F Prime framing or CCSDS space packets (with or without SDLS). | CCSDS space packets (with or without SDLS). F Prime framing is possible, but no F Prime-protocol ground system speaks CFDP. |
| Reliable delivery | No; operator-driven recovery. | Class 2 provides acknowledged delivery with retransmission; Class 1 does not. |
| Concurrent transfers | One file at a time in each direction. | Multiple transactions per channel, multiple channels. |
| Partial-file transfer | Downlink by offset and length. | Not supported. |
| Reference deployment | `Ref` (`TestDeploymentsProject/Ref`) and `fprime-yamcs-reference`. | None yet. |

Use the F Prime file protocol when operating with the F Prime GDS or when the link is reliable and simplicity matters. Use CFDP when the mission requires standards-based, interoperable, or autonomously reliable file delivery and a CFDP-capable ground system is available.

### File Dispatch

The File Dispatcher routes delivered files to different processing components based on file extension. A configuration table maps file extensions to numbered output ports. When a file is delivered (typically after uplink), the dispatcher examines its extension and forwards it to the appropriate handler. This allows different file types (e.g., sequences, parameter files, software updates) to be automatically routed to their respective processing components.

### File Worker

The File Worker provides an off-thread file I/O service for components that need to perform large or slow file operations without missing their timing deadlines. Components send file read or write requests to the File Worker, which executes them in its own thread and returns the results. This decouples file I/O latency from rate group execution.

### Off Nominal

- File system operations report errors via events when operations fail (e.g., file not found, permission denied, disk full).
- **F Prime file protocol.** Packet loss, checksum mismatches, and file-system errors are reported by events and counted in telemetry; out-of-order packets raise a warning but are written at their offset, so a file still completes if every packet arrives. A failed uplink is not announced to the File Dispatcher, but the partially written file remains at its destination path until removed or overwritten; recovery is to re-send the file. A downlink of a zero-length or unreadable file, or a path outside the sandbox, is rejected with an event.
- **CFDP Class 1.** Like the F Prime file protocol, there is no retransmission: a corrupt or short file is detected when the end-of-file PDU arrives (checksum or size mismatch) and reported by event. Unlike the F Prime protocol, a transfer whose PDUs stop arriving is timed out by the channel's inactivity timer and reported rather than left open.
- **CFDP Class 2.** Missing segments are requested by negative acknowledgment and retransmitted until either the file is complete or the configured retry limits or inactivity timers expire, at which point the transaction is faulted and reported. Operators can cancel or abandon stuck transactions by command.
- **Malformed input.** `CfdpManager` treats received PDUs as untrusted input and rejects or ignores malformed PDUs rather than asserting on them, so that a corrupt link does not take the deployment out of service through the file transfer path; this is an availability-hardening measure, not a substitute for authenticated lower layers. This does not protect against well-formed traffic from an unauthenticated sender, which can write any path the process can reach; see the authentication note under *Current limitations* above.
- File dispatch emits no event when a file extension matches no entry in the routing table; the file is left in place and not routed. A matched entry whose output port is not connected is reported with a warning.
