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

### Packet Spanning (Segmentation)

A TC Transfer Frame data field holds at most 1017 bytes, so a packet larger than that cannot be uplinked in a single frame. CCSDS 232.0-B-4 Section 4.1.3.3 defines an optional one-byte **Segment Header** (2-bit Sequence Flags | 6-bit MAP ID) at the start of every frame data field, which allows one packet to be split into segments carried by consecutive TC frames. The `TcDeframer` implements the receiving side of this mechanism (the standard's *MAP Packet Extraction* function). It is disabled by default and enabled with:

```cpp
void configureSegmentation(bool segmentHeaderPresent, U8 mapId, FwSizeType maxSpanningPacketSize);
```

- `segmentHeaderPresent`: If `true`, every valid frame data field is expected to start with a Segment Header. If `false` (default), the whole data field is forwarded as one packet, exactly as when segmentation is not configured.
- `mapId`: The MAP ID (0..63) accepted in the Segment Header. Segments carrying a different MAP ID are dropped.
- `maxSpanningPacketSize`: The maximum size of a reassembled packet, and the size of the buffer requested from `allocate` for each spanning packet.

When enabled, the `allocate` and `deallocate` ports must be connected to a buffer provider (e.g. `Svc.BufferManager`). The Sequence Flags drive the following behavior:

| Sequence Flags | Meaning | Behavior |
|---|---|---|
| `0b11` UNSEGMENTED | Whole packet | Segment Header is stripped and the data is forwarded zero-copy on `dataOut` (the frame buffer is returned to the upstream owner when it comes back on `dataReturnIn`). Any spanning packet in progress is abandoned. |
| `0b01` FIRST | First segment | Any spanning packet in progress is abandoned, a reassembly buffer of `maxSpanningPacketSize` bytes is allocated and the segment data is copied into it. |
| `0b00` CONTINUING | Middle segment | Segment data is appended to the spanning packet in progress. |
| `0b10` LAST | Last segment | Segment data is appended and the reassembled packet is emitted on `dataOut`. |

For FIRST, CONTINUING and LAST segments the source frame buffer is returned upstream on `dataReturnOut` immediately after the segment data has been copied. Reassembled packets are owned by the `TcDeframer`: when one comes back on `dataReturnIn` it is returned to the provider through `deallocate`, whereas any other buffer is forwarded to `dataReturnOut`. At most `MaxSpanningPacketsInFlight` (8) reassembled packets may be outstanding downstream at once; a completed packet is dropped if the table is full. `dataReturnIn` is a `sync` port and may be invoked from a downstream thread (e.g. a file uplink task returning a buffer) while `dataIn` runs under the component guard, so the in-flight table is protected by its own mutex.

Off-nominal segments are dropped and reported via events and `errorNotify`:

- CONTINUING or LAST with no spanning packet in progress (`UnexpectedSegment`, `TC_SEGMENT_UNEXPECTED`).
- MAP ID different from the configured one (`InvalidMapId`, `TC_INVALID_MAP_ID`).
- Reassembled size exceeding `maxSpanningPacketSize` (`SpanningPacketOverflow`, `TC_SEGMENT_OVERFLOW`); the packet in progress is dropped.
- Reassembly buffer allocation failure (`SpanningPacketAllocFailed`, `TC_SEGMENT_ALLOC_FAILED`).
- Frame data field holding only a Segment Header (`InvalidPacket`, `TC_INVALID_LENGTH`).

A spanning packet is abandoned (`SpanningPacketAbandoned`) whenever a FIRST or UNSEGMENTED segment arrives while one is in progress: a lost LAST segment therefore only costs the packet it belonged to. Note that the ground framer must produce the Segment Header in every frame when this mode is enabled: a frame without one is rejected, typically as `InvalidMapId` since the first Space Packet header byte is read as a Segment Header. The stock `fprime-gds` TC framer does not emit Segment Headers; YAMCS (`UdpTcFrameLink`) emits one when `mapId` is set on the virtual channel, but only with UNSEGMENTED flags (it rejects packets larger than a frame rather than segmenting them). In the `Svc.ComCcsdsSdls` stack the reassembled unit is what the SDLS deframer receives, so the ground must encrypt each packet before segmenting it.

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| Input (guarded) | dataIn | Svc.ComDataWithContext | Port to receive framed data |
| Output | dataOut | Svc.ComDataWithContext | Port to output deframed data. The emitted `ComCfg.FrameContext` carries the frame's Virtual Channel ID in its `vcId` field. |
| Output | dataReturnOut | Svc.ComDataWithContext | Port for returning ownership of received buffers to deframe |
| Input (sync) | dataReturnIn | Svc.ComDataWithContext | Port receiving back ownership of sent buffers |
| Output | errorNotify | Ccsds.ErrorNotify | Port to send notification of deframing errors |
| Output | allocate | Fw.BufferGet | Port to allocate reassembly buffers for spanning packets (only used when segmentation is enabled) |
| Output | deallocate | Fw.BufferSend | Port to deallocate reassembly buffers for spanning packets (only used when segmentation is enabled) |

## Events

| Name | Severity | Description |
|---|---|---|
| InvalidSpacecraftId | `warning low` | Deframing received an invalid SCID |
| InvalidFrameLength | `warning high` | Deframing received an invalid frame length |
| InvalidVcId | `activity low` | Deframing received an invalid VCID |
| InvalidCrc | `warning high` | Deframing received an invalid checksum |
| InvalidMapId | `warning low` | Segment Header MAP ID does not match the configured MAP ID; segment dropped |
| UnexpectedSegment | `warning high` | A continuing or last segment was received with no spanning packet in progress; segment dropped |
| SpanningPacketAbandoned | `warning high` | A spanning packet was abandoned before its last segment was received |
| SpanningPacketAllocFailed | `warning high` | Spanning packet buffer allocation failed; packet dropped |
| SpanningPacketOverflow | `warning high` | A spanning packet exceeded the configured maximum size; packet dropped |
| SpanningPacketInFlightLimit | `warning high` | A reassembled spanning packet was dropped because too many are outstanding downstream |

## Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-CCSDS-TC-DEFRAMER-001 | The TcDeframer shall deframe Telecommand (TC) Transfer Frames according to the CCSDS Space Data Link Protocol standard for Type-BD frames. | Unit Test, Inspection |
| SVC-CCSDS-TC-DEFRAMER-002 | The TcDeframer shall perform Frame Validation Check Procedures, including Spacecraft ID, Virtual Channel ID, Frame Length, and CRC. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-003 | The TcDeframer shall be configurable for a specific Spacecraft ID. | Unit Test, Inspection |
| SVC-CCSDS-TC-DEFRAMER-004 | The TcDeframer shall be configurable with a specific Virtual Channel ID (VCID) OR to accept all VCIDs. | Unit Test, Inspection |
| SVC-CCSDS-TC-DEFRAMER-005 | The TcDeframer shall log an `InvalidSpacecraftId` event if a frame with an unexpected Spacecraft ID is received. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-006 | The TcDeframer shall log an `InvalidFrameLength` event if a frame with an invalid length is received. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-007 | The TcDeframer shall log an `InvalidVcId` event if a frame with an unexpected VCID is received (when not configured to accept all VCIDs). | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-008 | The TcDeframer shall log an `InvalidCrc` event if a frame fails the CRC check. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-009 | The TcDeframer shall provide an input port (`dataIn`) to receive framed data, and emit deframed data packets on its `dataOut` output port. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-010 | The TcDeframer shall emit notifications on its `errorNotify` port when deframing errors occur. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-011 | The TcDeframer shall record the received frame's Virtual Channel ID in the `vcId` field of the `ComCfg.FrameContext` emitted on `dataOut`. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-012 | The TcDeframer shall be configurable to process a Segment Header (CCSDS 232.0-B-4 Section 4.1.3.3) at the start of each frame data field; when not so configured it shall forward the whole data field as one packet. | Unit Test, Inspection |
| SVC-CCSDS-TC-DEFRAMER-013 | When segmentation is enabled, the TcDeframer shall forward UNSEGMENTED data zero-copy on `dataOut` with the Segment Header removed. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-014 | When segmentation is enabled, the TcDeframer shall reassemble FIRST, CONTINUING and LAST segments into a single packet in a buffer obtained from `allocate`, and emit it on `dataOut` upon receiving the LAST segment. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-015 | The TcDeframer shall return each source frame buffer on `dataReturnOut` once its segment data has been copied, and shall deallocate reassembled packets returned on `dataReturnIn` via `deallocate`. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-016 | The TcDeframer shall drop CONTINUING or LAST segments received with no spanning packet in progress, and shall abandon any spanning packet in progress when a FIRST or UNSEGMENTED segment is received, reporting each case with an event. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-017 | The TcDeframer shall drop segments whose MAP ID does not match the configured MAP ID and log an `InvalidMapId` event. | Unit Test |
| SVC-CCSDS-TC-DEFRAMER-018 | The TcDeframer shall bound the reassembled packet size to the configured maximum, dropping the packet and logging `SpanningPacketOverflow` when exceeded, and shall drop the packet and log an event when allocation fails or the in-flight packet limit is reached. | Unit Test |
