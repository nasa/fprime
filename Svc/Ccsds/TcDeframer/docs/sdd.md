# Svc::Ccsds::TcDeframer

The `Svc::Ccsds::TcDeframer` is an implementation of the [DeframerInterface](../../../Interfaces/docs/sdd.md) for the CCSDS [TC Space Data Link Protocol](https://ccsds.org/Pubs/232x0b4e1c1.pdf). 

It receives payload data (such as a Space Packet or a VCA_SDU) on input and produces a TC frame on its output port as a result. Please refer to the CCSDS [TC specification (CCSDS 232.0-B-4)](https://ccsds.org/Pubs/232x0b4e1c1.pdf) for details on the frame format and protocol.

The `Svc::Ccsds::TcDeframer` is designed to work in the common F Prime telemetry stack, receiving data from a [Communications Adapter](../../../Interfaces/docs/sdd.md) or the `Svc::FrameAccumulator`, for deframing and transmission to the rest of the system. It is commonly coupled with the [`Svc::Ccsds::SpacePacketDeframer`](../../SpacePacketFramer/docs/sdd.md) to unwrap CCSDS Space Packets from TC frames.

The TcDeframer currently functions only in the "Expedited Service" mode, for Type-B Frames. This means that should Type-A frames be received, no FARM checks would be performed on board.

## Configuration

The `TcDeframer` component can be configured with a specific Virtual Channel ID (VCID) and Spacecraft ID. By default, it uses the spacecraft ID from `config/ComCfg.fpp` and accepts all VCIDs.

```cpp
void configure(U16 vcId, U16 spacecraftId, bool acceptAllVcid);
```

- `vcId`: The virtual channel ID to accept. This is only used if `acceptAllVcid` is `false`.
- `spacecraftId`: The spacecraft ID to accept.
- `acceptAllVcid`: If `true`, the deframer accepts all VCIDs. If `false`, it only accepts the `vcId` specified.

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| Input (guarded) | dataIn | Svc.ComDataWithContext | Port to receive framed data |
| Output | dataOut | Svc.ComDataWithContext | Port to output deframed data |
| Output | dataReturnOut | Svc.ComDataWithContext | Port for returning ownership of received buffers to deframe |
| Input (sync) | dataReturnIn | Svc.ComDataWithContext | Port receiving back ownership of sent buffers |

## Events

| Name | Severity | Description |
|---|---|---|
| InvalidSpacecraftId | `warning low` | Deframing received an invalid SCID |
| InvalidFrameLength | `warning high` | Deframing received an invalid frame length |
| InvalidVcId | `activity low` | Deframing received an invalid VCID |
| InvalidCrc | `warning high` | Deframing received an invalid checksum |

## Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-Ccsds-TC-DEFRAMER-001 | The TcDeframer shall deframe Telecommand (TC) Transfer Frames according to the CCSDS Space Data Link Protocol standard for Type-BD frames. | Unit Test, Inspection |
| SVC-Ccsds-TC-DEFRAMER-002 | The TcDeframer shall perform Frame Validation Check Procedures, including Spacecraft ID, Virtual Channel ID, Frame Length, and CRC. | Unit Test |
| SVC-Ccsds-TC-DEFRAMER-003 | The TcDeframer shall be configurable for a specific Spacecraft ID. | Unit Test, Inspection |
| SVC-Ccsds-TC-DEFRAMER-004 | The TcDeframer shall be configurable with a specific Virtual Channel ID (VCID) OR to accept all VCIDs. | Unit Test, Inspection |
| SVC-Ccsds-TC-DEFRAMER-005 | The TcDeframer shall log an `InvalidSpacecraftId` event if a frame with an unexpected Spacecraft ID is received. | Unit Test |
| SVC-Ccsds-TC-DEFRAMER-006 | The TcDeframer shall log an `InvalidFrameLength` event if a frame with an invalid length is received. | Unit Test |
| SVC-Ccsds-TC-DEFRAMER-007 | The TcDeframer shall log an `InvalidVcId` event if a frame with an unexpected VCID is received (when not configured to accept all VCIDs). | Unit Test |
| SVC-Ccsds-TC-DEFRAMER-008 | The TcDeframer shall log an `InvalidCrc` event if a frame fails the CRC check. | Unit Test |
| SVC-Ccsds-TC-DEFRAMER-009 | The TcDeframer shall provide an input port (`dataIn`) to receive framed data, and emit deframed data packets on its `dataOut` output port. | Unit Test |
