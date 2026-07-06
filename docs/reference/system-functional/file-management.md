
# File Management Functionality

## References

- [F Prime FileManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileManager/docs/sdd.md)
- [F Prime FileDownlink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDownlink/docs/sdd.md)
- [F Prime FileUplink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileUplink/docs/sdd.md)
- [F Prime FileDispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDispatcher/docs/sdd.md)
- [F Prime FileWorker SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileWorker/docs/sdd.md)

## Overview

File management provides the capability to manipulate files on the spacecraft file system, transfer files between the ground and the spacecraft in both directions, route delivered files to appropriate handlers, and perform large file I/O operations without blocking time-critical components. These capabilities are delivered by a set of collaborating components that together support the full lifecycle of file operations.

### File System Operations

The File Manager provides ground commands for common file and directory operations:

- Create, remove, and rename files and directories
- Move and copy files
- Concatenate files
- Query file sizes
- List directory contents

Each operation reports its result via events. File system operations execute in the File Manager's own thread to avoid blocking the commanding path.

### File Downlink

File downlink transfers files from the spacecraft to the ground. The File Downlink component reads a file from the file system, breaks it into packets, and sends them through the communication stack. Both operators and other components can initiate downlinks.

Downlink operations support:

- Sending individual files by command
- Canceling an in-progress downlink
- Notification of downlink completion to other components via a completion port

File data is broken into fixed-size packets, with each packet containing a sequence number and offset for reassembly on the ground.

### File Uplink

File uplink receives files from the ground and writes them to the spacecraft file system. The File Uplink component receives file packets from the communication stack, reassembles them, and writes the complete file to the specified path.

File uplink handles:

- Receiving start, data, and end packets in sequence
- Writing the reassembled file to disk
- Reporting completion or errors via events

### File Dispatch

The File Dispatcher routes delivered files to different processing components based on file extension. A configuration table maps file extensions to numbered output ports. When a file is delivered (typically after uplink), the dispatcher examines its extension and forwards it to the appropriate handler. This allows different file types (e.g., sequences, parameter files, software updates) to be automatically routed to their respective processing components.

### File Worker

The File Worker provides an off-thread file I/O service for components that need to perform large or slow file operations without missing their timing deadlines. Components send file read or write requests to the File Worker, which executes them in its own thread and returns the results. This decouples file I/O latency from rate group execution.

### Off Nominal

- File system operations report errors via events when operations fail (e.g., file not found, permission denied, disk full).
- File downlink reports errors if the file cannot be read or if the downlink is interrupted.
- File uplink handles out-of-order packets and reports warnings for unexpected sequence numbers.
- File dispatch logs a warning if a file extension does not match any entry in the routing table.
