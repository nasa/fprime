# Svc::Ccsds::AosDeframer

The `Svc::Ccsds::AosDeframer` is an implementation of the [DeframerInterface](../../../Interfaces/docs/sdd.md) for the CCSDS [AOS Space Data Link Protocol](https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/10/732x0b5ec1.pdf).

It receives fixed-size AOS transfer frames on its input and extracts the packets contained in the frame data zone (M_PDU), emitting each complete packet on its output port. Please refer to the CCSDS [AOS specification (CCSDS 732.0-B-5)](https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/10/732x0b5ec1.pdf) for details on the frame format and protocol.

The `Svc::Ccsds::AosDeframer` is designed to work in the common F Prime uplink stack, receiving frame data from an upstream [`Svc::FrameAccumulator`](../../../FrameAccumulator/docs/sdd.md) or communications adapter and passing extracted packets downstream, commonly to a [`Svc::Ccsds::SpacePacketDeframer`](../../SpacePacketDeframer/docs/sdd.md) or a router. It is the receive-side counterpart of the [`Svc::Ccsds::AosFramer`](../../AosFramer/docs/sdd.md).

## AOS Deframing Support

The AOS Deframer supports the following subset of CCSDS AOS SDL:

* A single accepted virtual channel (`NumVcs = 1`), configured via the `configure()` function
* M_PDU (Multiplexing PDU) data field service, including packets spanning multiple frames
* Optional Frame Error Control Field (FECF) verification per Section 4.1.6
* Space Packet Protocol (SPP) extraction per CCSDS 133.0-B-2 and Encapsulation Packet Protocol (EPP) extraction per CCSDS 133.1-B-3, individually enabled via a packet-version-number (PVN) mask
* Idle frame and idle packet discard
* VC frame count continuity checking with gap detection

Space Data Link Security (SDLS), the Transfer Frame Insert Zone, and the Frame Header Error Control Field are not supported.

## Internals

The deframer validates each incoming frame header: Transfer Frame Version Number, Spacecraft ID, Virtual Channel ID, frame length, and (when enabled) the FECF CRC. Invalid frames are dropped with a corresponding event and, when connected, a notification on the `errorNotify` port.

For each valid frame, the M_PDU First Header Pointer (FHP) is used to locate packet boundaries within the data zone. Complete packets are emitted directly. When a packet spans multiple frames, the deframer allocates a buffer through its `BufferAllocation` interface (`allocate`/`deallocate` ports), accumulates the packet across successive frames, and emits it when complete. A frame count gap, an invalid FHP, or an allocation failure causes any in-progress spanning packet to be abandoned and reported via events.

Ownership of incoming frame buffers is returned to the sender via the `dataReturnOut` port; downstream consumers return emitted packet buffers via the `dataReturnIn` port, which the deframer deallocates when it owns them (spanning-packet buffers) or passes back upstream otherwise.

## Configuration

The `configure()` function must be called before any frames are processed:

```cpp
deframer.configure(
    fixedFrameSize,          // Fixed size of AOS frames in bytes (Section 4.1.1)
    frameErrorControlField,  // Whether the FECF is present (Section 4.1.6)
    spacecraftId,            // Accepted 10-bit spacecraft ID (default: ComCfg::SpacecraftId)
    vcId,                    // Accepted 6-bit virtual channel ID (default: 0)
    pvnMask                  // Bitmask of PVNs to extract (SPP and EPP enabled by default)
);
```

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| guarded input | dataIn | Svc.ComDataWithContext | Receives framed AOS data (VC_RECEIVE.indication, Section 3.4.3.2) |
| output | dataOut | Svc.ComDataWithContext | Outputs extracted packets with context |
| output | dataReturnOut | Svc.ComDataWithContext | Returns ownership of received frame buffers to the sender |
| sync input | dataReturnIn | Svc.ComDataWithContext | Receives back ownership of emitted packet buffers |
| output | errorNotify | Ccsds.ErrorNotify | Notifies a connected component of deframing errors |
| output | allocate | Fw.BufferGet | Allocates buffers for packets spanning multiple frames |
| output | deallocate | Fw.BufferSend | Deallocates spanning-packet buffers |

## Events

| Event | Severity | Description |
|---|---|---|
| InvalidSpacecraftId | warning low | Frame SCID does not match the configured SCID |
| InvalidFrameLength | warning high | Frame length does not match the configured fixed frame size |
| InvalidVcId | activity low | Frame VCID is not in the accepted set |
| InvalidFecf | warning high | FECF (CRC) check failed |
| InvalidTfvn | warning high | Invalid Transfer Frame Version Number |
| DisabledPvn | warning high | Packet version number invalid or disabled by configuration |
| IdleFrame | activity low | Frame contained only idle data |
| SpanningPacketAllocFailed | warning high | Buffer allocation for a spanning packet failed; packet dropped |
| VcFrameCountGap | warning high | Discontinuity detected in the VC frame count sequence |
| SpanningPacketAbandoned | warning high | A spanning packet was abandoned before completion |
| InvalidFhp | warning high | First Header Pointer exceeds the data zone size |

## Telemetry

| Channel | Type | Description |
|---|---|---|
| LatestVcFrameCount | U32 | Latest VC frame count received from the frame header |
| FramesProcessed | U32 | Frames processed per virtual channel |
| PacketsExtracted | U32 | Packets extracted per virtual channel |
| CrcErrorCount | U32 | FECF (CRC) errors per physical channel |
