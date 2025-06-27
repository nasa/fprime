# Svc::Ccsds::ApidManager

The `Svc::Ccsds::ApidManager` component manages CCSDS Application Process Identifier (APID) sequence counts for the F Prime communications stack. It provides per-APID sequence count tracking and validation, supporting the construction and checking of CCSDS Space Packet headers by other components (such as `SpacePacketFramer` and `SpacePacketDeframer`).

The `ApidManager` is typically used in conjunction with the [`SpacePacketFramer`](../../SpacePacketFramer/docs/sdd.md) (to provide incrementing sequence counts for each APID) and the [`SpacePacketDeframer`](../../SpacePacketDeframer/docs/sdd.md) (to validate received sequence counts and detect dropped or out-of-order packets).

## Functionality

- Maintains a table of APIDs and their associated 14-bit sequence counts.
- Handles a fixed maximum number of tracked APIDs (as configured in the project).
- Provides a way to retrieve the current sequence count for a given APID through a port call.
- Provides a way to validate a received sequence counts for a given APID through a port call.

## Port Descriptions

| Kind            | Name                  | Port Type                | Description                                                                 |
|-----------------|-----------------------|--------------------------|-----------------------------------------------------------------------------|
| guarded input   | validateApidSeqCountIn| Ccsds.ApidSequenceCount  | Validates a received sequence count for a given APID.                       |
| guarded input   | getApidSeqCountIn     | Ccsds.ApidSequenceCount  | Returns and increments the sequence count for a given APID.                 |

## Events

| Name                   | Severity      | Description                                                                 |
|------------------------|---------------|-----------------------------------------------------------------------------|
| UnexpectedSequenceCount| warning low   | Received an unexpected sequence count for an APID.                          |
| ApidTableFull          | warning high  | APID table is full; cannot track additional APIDs.                          |

## Usage

- The `getApidSeqCountIn` port is called by a component (e.g., `SpacePacketFramer`) to obtain and increment the sequence count for a given APID when constructing a new Space Packet.
- The `validateApidSeqCountIn` port is called by a component (e.g., `SpacePacketDeframer`) to check the sequence count of a received Space Packet. If the count does not match the expected value, an event is logged and the onboard count is synchronized.

## Requirements

| Name                              | Description                                                                 | Validation           |
|-----------------------------------|-----------------------------------------------------------------------------|----------------------|
| SVC-Ccsds-APID-MANAGER-001         | The ApidManager shall track a 14-bit sequence count for each APID.          | Unit Test            |
| SVC-Ccsds-APID-MANAGER-002         | The ApidManager shall provide the current sequence count for a given APID.  | Unit Test            |
| SVC-Ccsds-APID-MANAGER-003         | The ApidManager shall increment the sequence count for each APID on request.| Unit Test            |
| SVC-Ccsds-APID-MANAGER-004         | The ApidManager shall provide validation of a received sequence counts for each APID.  | Unit Test |
| SVC-Ccsds-APID-MANAGER-005         | The ApidManager shall emit an event if an unexpected sequence count is received.| Unit Test        |
| SVC-Ccsds-APID-MANAGER-006         | The ApidManager shall emit an event if the APID table is full and an APID is not able to be added to the tracking. | Unit Test |
| SVC-Ccsds-APID-MANAGER-007         | The ApidManager shall synchronize the onboard sequence count if a mismatch is detected.| Unit Test   |

## See Also

- [`Svc::Ccsds::SpacePacketFramer`](../../SpacePacketFramer/docs/sdd.md)
- [`Svc::Ccsds::SpacePacketDeframer`](../../SpacePacketDeframer/docs/sdd.md)
- [CCSDS Space Packet Protocol (CCSDS 133.0-B-2)](https://public.ccsds.org/Pubs/133x0b2e1.pdf)
