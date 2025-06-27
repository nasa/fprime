# Svc::Ccsds::SpacePacketDeframer

The `Svc::Ccsds::SpacePacketDeframer` is an implementation of the [DeframerInterface](../../../Interfaces/docs/sdd.md) for the CCSDS [Space Packet Protocol](https://ccsds.org/Pubs/133x0b2e2.pdf).

It receives data containing a Space Packet on its input port and extracts the Space Packet. Please refer to the CCSDS [Space Packet Protocol specification (CCSDS 133.0-B-2)](https://ccsds.org/Pubs/133x0b2e2.pdf) for details on the packet format.

The `Svc::Ccsds::SpacePacketDeframer` is typically used downstream of a component that removes transfer frame headers, such as the `Svc::Ccsds::TcDeframer`. It validates the Space Packet header and extracts the payload.

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| Input (guarded) | dataIn | Svc.ComDataWithContext | Port to receive data containing a Space Packet |
| Output | dataOut | Svc.ComDataWithContext | Port to output the extracted Space Packet payload |
| Output | dataReturnOut | Svc.ComDataWithContext | Port for returning ownership of received buffers |
| Input (sync) | dataReturnIn | Svc.ComDataWithContext | Port receiving back ownership of sent buffers |
| Output | validateApidSeqCount | Ccsds.ApidSequenceCount | Port to request validation of a sequence count for a given APID |

## Events

| Name | Severity | Description |
|---|---|---|
| InvalidLength | `warning high` | Deframing received an invalid packet length |

## Requirements

Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
| SPD-001 | The SpacePacketDeframer shall deframe CCSDS Space Packets according to the CCSDS 133.0-B-2 standard. | Unit Test, Inspection |
| SPD-002 | The SpacePacketDeframer shall extract the user data field from valid Space Packets. | Unit Test |
| SPD-003 | The SpacePacketDeframer shall validate the packet length of a Space Packet Primary Header. | Unit Test |
| SPD-009 | The SpacePacketDeframer shall emit an `InvalidLength` event if the packet length token in the header is cannot fit in the received data, and drop the received packet. | Unit Test |
| SPD-004 | The SpacePacketDeframer shall receive incoming data containing Space Packets via the `dataIn` port. | Unit Test |
| SPD-005 | The SpacePacketDeframer shall output the extracted Space Packet user data via the `dataOut` port. | Unit Test |
| SPD-006 | The SpacePacketDeframer shall manage buffer ownership using the `dataReturnIn` and `dataReturnOut` ports. | Unit Test, Inspection |
| SPD-007 | The SpacePacketDeframer shall delegate sequence count validation to another component through the `validateApidSeqCount` port. | Unit Test |
| SPD-008 | The SpacePacketDeframer shall perform sequence count validation for APIDs registered via `validateApidSeqCount`. | Unit Test |
