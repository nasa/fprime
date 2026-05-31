
# CCSDS Protocol Functionality

## References

- [F Prime SpacePacketFramer SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/SpacePacketFramer/docs/sdd.md)
- [F Prime SpacePacketDeframer SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/SpacePacketDeframer/docs/sdd.md)
- [F Prime ApidManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/ApidManager/docs/sdd.md)
- [F Prime TmFramer SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/TmFramer/docs/sdd.md)
- [F Prime TcDeframer SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/TcDeframer/docs/sdd.md)
- [F Prime AosFramer SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ccsds/AosFramer/docs/sdd.md)
- [CCSDS Space Packet Protocol (133.0-B-2)](https://ccsds.org/Pubs/133x0b2e2.pdf)
- [CCSDS TM Space Data Link Protocol (132.0-B-3)](https://ccsds.org/Pubs/132x0b3.pdf)
- [CCSDS TC Space Data Link Protocol (232.0-B-4)](https://ccsds.org/Pubs/232x0b4e1c1.pdf)
- [CCSDS AOS Space Data Link Protocol (732.0-B-5)](https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/10/732x0b5ec1.pdf)

## Overview

F Prime provides components implementing the Consultative Committee for Space Data Systems (CCSDS) protocols for missions requiring standards-compliant space communication. These components plug into the communication stack in place of (or alongside) the default F Prime protocol components, providing CCSDS-standard framing at multiple protocol layers. The implementation covers the Space Packet Protocol, TM and TC Space Data Link protocols, and the AOS Space Data Link protocol.

### Space Packet Protocol

The Space Packet layer provides application-level packet framing per CCSDS 133.0-B-2:

- **Space Packet Framer** — Constructs CCSDS Space Packets from user data. Each packet is assigned an Application Process Identifier (APID) and a sequence count for tracking.
- **Space Packet Deframer** — Extracts and validates received Space Packets, verifying the packet structure and sequence counts to detect dropped or out-of-order packets.

### APID Management

The APID Manager tracks per-APID sequence counts for both outgoing and incoming Space Packets. It provides incrementing sequence counts to the Space Packet Framer for each APID and validates received sequence counts in the Space Packet Deframer to detect packet loss.

By default, APIDs are assigned based on the F Prime data descriptor type (commands, telemetry, events, files, packetized telemetry). Missions requiring custom APID assignments can replace the default APID Manager component with a project-specific implementation.

### TM Space Data Link Protocol

The TM Framer implements the CCSDS Telemetry (TM) Space Data Link Protocol (132.0-B-3) for downlink. It wraps payload data (such as Space Packets) into TM Transfer Frames for transmission over the space link. The current implementation supports a single Virtual Channel Identifier (VCID).

### TC Space Data Link Protocol

The TC Deframer implements the CCSDS Telecommand (TC) Space Data Link Protocol (232.0-B-4) for uplink. It extracts payload data from received TC Transfer Frames.

### AOS Space Data Link Protocol

The AOS Framer implements the CCSDS Advanced Orbiting Systems (AOS) Space Data Link Protocol (732.0-B-5). AOS provides an alternative to TM for missions requiring more flexible virtual channel management.

### Protocol Layering

The CCSDS components can be stacked to provide multiple protocol layers. A typical downlink path might be: data source → Space Packet Framer → TM Framer → byte stream driver. A typical uplink path: byte stream driver → Frame Accumulator → TC Deframer → Space Packet Deframer → Router. The modular design allows missions to select the specific protocol layers they require.

### Unsupported Features

The current CCSDS implementation does not support:

- Multiple Virtual Channel Identifiers (VCIDs) — only a single VCID is available per TM Framer or TC Deframer instance.

### Off Nominal

- Malformed frames or packets are rejected with diagnostic events.
- Sequence count mismatches indicate dropped or reordered packets and are reported via events. The onboard sequence count is synchronized to the received value and processing continues.
- CRC or checksum failures cause the affected frame to be dropped.
