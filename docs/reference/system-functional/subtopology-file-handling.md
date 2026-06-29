
# File Handling Subtopology

## References

- [FileHandling Subtopology SDD](https://github.com/nasa/fprime/blob/devel/Svc/Subtopologies/FileHandling/docs/sdd.md)
- [F Prime FileUplink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileUplink/docs/sdd.md)
- [F Prime FileDownlink SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileDownlink/docs/sdd.md)
- [F Prime FileManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/FileManager/docs/sdd.md)
- [F Prime PrmDb SDD](https://github.com/nasa/fprime/blob/devel/Svc/PrmDb/docs/sdd.md)

## Overview

The File Handling subtopology packages the core file-transfer and file-management services commonly needed in F Prime deployments into a single reusable unit. It provides file uplink (ground to flight), file downlink (flight to ground), on-board file management operations, and filesystem-based parameter management.

### Included Components

- **File Uplink** (active) — Receives file packets from the communication stack and reconstructs files on the flight file system.
- **File Downlink** (active) — Reads files from the file system, segments them into packets, and sends them through the communication stack for transmission to the ground.
- **File Manager** (active) — Provides ground commands for common file operations (list, remove, create directory, move, copy, concatenate, hash).
- **Parameter Database** (active) — Manages persistent parameter storage via the file system. Loads parameters at startup and saves them on command.

### Required Inputs

- Rate group connections to drive File Downlink's send cycle.
- Communication stack connections for file packet exchange (wire to a ComFprime or ComCcsds subtopology's file ports).
- Parameter connections from all components that use parameters.

### Configuration

- Base IDs, queue sizes, stack sizes, and priorities via FileHandlingConfig.
- The File Downlink send rate is determined by the connected rate group frequency.
- The Parameter Database file path is configured at deployment time.

### Limitations

This subtopology focuses on file transfer, file operations, and parameter management. It does not provide framing, deframing, or CDH services — those are provided by the communication and CDH subtopologies.
