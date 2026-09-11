
# File Management Functionality

## References

- [F Prime FileManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileManager/docs/sdd.md)
- [F Prime FileDownlink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDownlink/docs/sdd.md)
- [F Prime FileUplink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileUplink/docs/sdd.md)
- [F Prime FilePacket SDD](https://github.com/nasa/fprime/blob/devel/Fw/FilePacket/docs/sdd.md) (F Prime file packet format)
- [F Prime CfdpManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/CfdpManager/docs/sdd.md)
- [F Prime FileDispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDispatcher/docs/sdd.md)
- [F Prime FileWorker SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileWorker/docs/sdd.md)
- [FileHandling Subtopology SDD](https://github.com/nasa/fprime/blob/devel/Svc/Subtopologies/FileHandling/docs/sdd.md)
- [FileHandlingCfdp Subtopology SDD](https://github.com/nasa/fprime/blob/devel/Svc/Subtopologies/FileHandlingCfdp/docs/sdd.md)
- [CCSDS 727.0-B-5, CCSDS File Delivery Protocol (CFDP)](https://ccsds.org/Pubs/727x0b5e1.pdf)

## Overview

File management provides the capability to manipulate files on the spacecraft file system, transfer files between the ground and the spacecraft in both directions, route delivered files to appropriate handlers, and perform large file I/O operations without blocking time-critical components. These capabilities are delivered by a set of collaborating components that together support the full lifecycle of file operations.

File transfer is available in two interchangeable implementations: the F Prime file protocol (`FileUplink` and `FileDownlink`) and the CCSDS File Delivery Protocol (`CfdpManager`). Both are described under [File Transfer](#file-transfer) below, along with where each is supported. This page is an overview; the SDDs linked above are the authoritative descriptions of ports, commands, events, telemetry, and configuration.

### File System Operations

The File Manager provides ground commands for common file and directory operations:

- Create, remove, and rename files and directories
- Move and copy files
- Concatenate files
- Query file sizes
- List directory contents

Each operation reports its result via events. File system operations execute in the File Manager's own thread to avoid blocking the commanding path.

### File Transfer

File transfer moves whole files between the ground and the flight software: uplink (ground to flight) delivers sequences, parameter files, and software updates; downlink (flight to ground) returns data products, logs, and diagnostic files. F Prime ships two implementations of this capability:

- **F Prime file protocol**, provided by the `FileUplink` and `FileDownlink` components. A lightweight, F Prime-specific protocol that is supported natively by the F Prime GDS.
- **CCSDS File Delivery Protocol (CFDP)**, provided by the `CfdpManager` component. A standards-based protocol with optional reliable delivery, intended for lossy or intermittent links and for interoperability with CFDP-capable ground systems.

Both present the same interfaces to the rest of the flight software and to the communication stack, so a deployment can swap one for the other without changing the components that produce or consume files.

#### F Prime File Protocol (FileUplink / FileDownlink)

Files are carried as a series of F Prime file packets: a START packet announcing the file name and size, DATA packets carrying offset-tagged chunks of the file, an END packet carrying a checksum, and a CANCEL packet to abort a transfer. The checksum is the 32-bit CFDP checksum, so integrity checking is shared with the CFDP mechanism.

**Downlink.** `FileDownlink` maintains a queue of files to send. Operators enqueue files by command (whole files, or a partial file given an offset and length) and on-board components enqueue files through a port; a completion port notifies the requesting component when its file has been sent. Files are sent one at a time, and a configurable cooldown between files keeps sustained file traffic from starving other downlink data. In-progress downlinks can be cancelled by command. File size is limited to 4 GiB.

**Uplink.** `FileUplink` receives file packets from the communication stack, reassembles them in order, verifies the checksum, and writes the file to non-volatile storage. Successfully received files are announced on a port so that they can be routed to a consumer by the [File Dispatcher](#file-dispatch). Files are received one at a time, and packets within a file are expected in order.

**Delivery guarantees.** The protocol has no retransmission. Lost or out-of-order packets are reported by events and telemetry counters, and recovery is an operator action: re-send the file, or re-send the missing range using a partial downlink. This is appropriate for links where the framing layer already provides reliability or where loss is rare and operator-driven recovery is acceptable.

**Sandboxing.** Both components confine file access to a directory chosen at initialization; paths that resolve outside it are rejected. The sandbox is fail-closed: a deployment must configure it or no file can be read or written. The stock `FileHandling` subtopology configures it to the file system root for backward compatibility, which deployments should restrict.

#### CCSDS File Delivery Protocol (CfdpManager)

`CfdpManager` is a single component that handles both uplink and downlink using CFDP protocol data units (PDUs). It is a port of the flight-proven cFS CF application, adapted to F Prime ports, commands, events, telemetry, and parameters.

**Transfer classes.** Class 1 (unacknowledged) sends a file with no feedback from the receiver, comparable to the F Prime file protocol. Class 2 (acknowledged) adds end-of-file and finished acknowledgments, gap detection, negative acknowledgments, and retransmission, providing complete and verified delivery over lossy or intermittent links without operator intervention.

**Channels and transactions.** Transfers are organized into independently configured channels, each with its own pool of concurrent transactions, entity identifiers, timers, retry limits, and outgoing-PDU throttle. Multiple files can be in flight at once within the configured limits.

**Operations.** Operators can send a single file, play back every file in a directory, or poll a directory on a fixed interval so that new files are sent automatically. Individual transactions can be suspended, resumed, cancelled with protocol close-out, or abandoned, and a channel's transmission can be frozen and resumed. On-board components can request a transfer through a port and are notified on completion, using the same port interface as `FileDownlink`.

**Timing.** Protocol timers and transaction processing are driven by a scheduler port that must be invoked at 1 Hz.

**Current limitations.** Partial-file transfers (non-zero offset or length) are not supported, files are limited to 4 GiB, received files are not announced to the File Dispatcher, and CFDP transfers are not confined by a file-system sandbox — the destination path in an incoming metadata PDU is honored as received. CFDP assumes authentication is provided by lower layers; deployments should pair it with link-layer security such as SDLS. See the CfdpManager SDD for the full list of assumptions and security considerations.

#### Interchangeability

The two mechanisms are designed as drop-in alternatives at three levels:

1. **Component interface.** Components that produce files for downlink (for example, the data product catalog) request a transfer through the `SendFileRequest` port and are notified through the `SendFileComplete` port. `FileDownlink` and `CfdpManager` both implement these ports, so file producers do not depend on which mechanism is deployed.
2. **Communication stack interface.** Both mechanisms exchange file traffic with the communication stack as F Prime file-type packets, so they connect to the same file uplink and file downlink ports exposed by the `ComFprime`, `ComCcsds`, and `ComCcsdsSdls` communication subtopologies. `CfdpManager` wraps each PDU in the file packet type so that the router delivers incoming PDUs to it and the framer transmits outgoing PDUs, exactly as it does for `FileUplink` and `FileDownlink`.
3. **Subtopology.** F Prime provides two pre-wired subtopologies that differ only in the transfer mechanism. `FileHandling` bundles `FileUplink`, `FileDownlink`, the File Manager, and the Parameter Database; `FileHandlingCfdp` bundles `CfdpManager`, the File Manager, and the Parameter Database. A deployment selects one of the two and connects it to a communication subtopology and a rate group.

Choosing a mechanism therefore comes down to the delivery guarantees needed and the ground system in use, not to on-board architecture.

#### Where Each Mechanism Is Supported

| | F Prime file protocol (`FileUplink` / `FileDownlink`) | CFDP (`CfdpManager`) |
|---|---|---|
| F Prime GDS (`fprime-gds`) | Supported: uplink and downlink from the GDS user interface, the `fprime-cli file-uplink` command, and the integration test API. | Not supported. The GDS does not implement CFDP. |
| YAMCS | Supported through the `fprime-yamcs` plugin; exercised by the `fprime-yamcs-reference` project. | Requires the YAMCS CFDP service; the flight side transports PDUs as CCSDS space packets. No F Prime reference project demonstrates this configuration yet. |
| Communication subtopologies | `ComFprime`, `ComCcsds`, `ComCcsdsSdls`. | `ComCcsds`, `ComCcsdsSdls`. Wiring to `ComFprime` is possible, but no F Prime-protocol ground system speaks CFDP. |
| Reliable delivery | No; operator-driven recovery. | Class 2 provides acknowledged delivery with retransmission; Class 1 does not. |
| Concurrent transfers | One file at a time in each direction. | Multiple transactions per channel, multiple channels. |
| Partial-file transfer | Downlink by offset and length. | Not supported. |
| Reference deployment | `Ref` (`TestDeploymentsProject/Ref`) and `fprime-yamcs-reference`. | `FileHandlingCfdp` subtopology; no shipped deployment instantiates it. |

Use the F Prime file protocol when operating with the F Prime GDS or when the link is reliable and simplicity matters. Use CFDP when the mission requires standards-based, interoperable, or autonomously reliable file delivery and a CFDP-capable ground system is available.

### File Dispatch

The File Dispatcher routes delivered files to different processing components based on file extension. A configuration table maps file extensions to numbered output ports. When a file is delivered (typically after uplink), the dispatcher examines its extension and forwards it to the appropriate handler. This allows different file types (e.g., sequences, parameter files, software updates) to be automatically routed to their respective processing components.

### File Worker

The File Worker provides an off-thread file I/O service for components that need to perform large or slow file operations without missing their timing deadlines. Components send file read or write requests to the File Worker, which executes them in its own thread and returns the results. This decouples file I/O latency from rate group execution.

### Off Nominal

- File system operations report errors via events when operations fail (e.g., file not found, permission denied, disk full).
- **F Prime file protocol.** Packet loss, out-of-order packets, checksum mismatches, and file-system errors are reported by events and counted in telemetry; the affected transfer fails and must be re-commanded. A downlink of a zero-length or unreadable file, or a path outside the sandbox, is rejected with an event.
- **CFDP Class 1.** Behaves like the F Prime file protocol: the receiver detects a missing or corrupt file at end-of-file and reports it, but no retransmission occurs.
- **CFDP Class 2.** Missing segments are requested by negative acknowledgment and retransmitted until either the file is complete or the configured retry limits or inactivity timers expire, at which point the transaction is faulted and reported. Operators can cancel or abandon stuck transactions by command.
- **Malformed input.** `CfdpManager` treats received PDUs as untrusted and discards malformed PDUs rather than asserting, so a hostile or corrupt link cannot take the deployment out of service through the file transfer path.
- File dispatch logs a warning if a file extension does not match any entry in the routing table.
