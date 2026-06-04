# F´ Wire Format Reference

This document specifies the on-wire byte formats used by the F´ communications
stack: the CCSDS transfer-frame / Space Packet framing (TC, TM, AOS, Space
Packet), the native F´ framing (`FprimeProtocol`), and the individual packet
payload formats (`TlmPacket`, `LogPacket`, `CmdPacket`, `FilePacket`).

It is a reference for the bytes that travel over the link. It complements the
[Communications Adapter Interface](./communication-adapter-interface.md), which
describes the component-level interface contract rather than the byte layout.

## Conventions

* **Endianness.** All multi-byte integer fields are serialized **big-endian**
  (network byte order). This is the F´ default (`Fw::Endianness::BIG`).
* **Bit numbering.** Within tables, bit `[15:14]` means the two most-significant
  bits of a 16-bit field, with bit 15 the MSB.
* **Offsets.** Byte offsets are zero-based from the start of the relevant
  structure (frame, header, or packet).
* **Sizes** are given in bytes (octets) unless noted as bits.

### Framework type widths

The packet/frame field widths below depend on the framework configuration
(`config/FpConfig.fpp`, `config/ComCfg.fpp`). The values listed are the
defaults shipped in the repository.

| Type | Default definition | Serialized size |
| --- | --- | --- |
| `FwPacketDescriptorType` | `U16` | 2 |
| `FprimeProtocol::TokenType` | `U32` | 4 |
| `FwChanIdType` / `FwEventIdType` / `FwOpcodeType` | `FwIdType` = `U32` | 4 |
| `FwSizeStoreType` (serialized length prefix) | `U16` | 2 |
| `FwSizeType` | `PlatformSizeType` = `U64` (unix platform) | 8 |
| `Fw::Time` | `FwTimeBaseStoreType`(U16) + `FwTimeContextStoreType`(U8) + `U32` + `U32` | 11 |

`Fw::Time` is serialized as: time base (`U16`), time context (`U8`), seconds
(`U32`), microseconds (`U32`) — 11 bytes total.

### Communications-stack layering

On downlink, a *packet payload* (e.g. a `TlmPacket` or `LogPacket`) is produced
by the application, tagged with a *packet descriptor* (APID), then wrapped by a
*framing protocol* before going to the byte stream / radio:

```
[ packet payload ]  -->  [ framing protocol adds header + trailer ]  -->  link
```

Two framing protocols are supported:

* **`FprimeProtocol`** — the native F´ frame (`startWord` + length + descriptor
  + payload + CRC32). See [F´ Framing](#f-framing-fprimeprotocol).
* **CCSDS** — Space Packet Protocol carried inside TC / TM / AOS transfer
  frames. See [CCSDS Framing](#ccsds-framing-tc--tm--aos--space-packet).

The packet *descriptor* (APID) identifies which packet payload format follows.
In `FprimeProtocol` it is carried in the frame header `packetDescriptor` field;
in CCSDS it is carried as the APID of the Space Packet primary header. See
[Packet Descriptor / APID](#packet-descriptor--apid).

---

## CCSDS Framing (TC / TM / AOS / Space Packet)

These structures are defined in `Svc/Ccsds/Types/Types.fpp` and implemented by
the components under `Svc/Ccsds/`. F´ uses the CCSDS Space Packet Protocol
(SPP) as the application packet, transported inside a Telecommand (TC) transfer
frame on uplink and a Telemetry (TM) or Advanced Orbiting Systems (AOS)
transfer frame on downlink.

### Frame Error Control Field (CRC16)

TC, TM, and AOS transfer frames are protected by a 2-byte Frame Error Control
Field (FECF). The algorithm is **CRC-16/CCITT-FALSE** (`Svc/Ccsds/Utils/CRC16`):

* Polynomial `0x1021`
* Initial value `0xFFFF`
* Final XOR `0x0000`

The CRC is computed over the entire frame **excluding** the 2-byte FECF itself,
which occupies the last two bytes of the frame.

### Space Packet (SPP)

The CCSDS Space Packet is the application-layer packet. It is a 6-byte primary
header followed by the user data field. Defined as `SpacePacketHeader` and
produced/consumed by `Svc/Ccsds/SpacePacketFramer` and `SpacePacketDeframer`.

**Primary header (6 bytes):**

| Offset | Size | Field | Sub-fields |
| --- | --- | --- | --- |
| 0 | 2 | `packetIdentification` | PVN [15:13] (3b) \| Packet Type [12] (1b) \| Secondary Header Flag [11] (1b) \| APID [10:0] (11b) |
| 2 | 2 | `packetSequenceControl` | Sequence Flags [15:14] (2b) \| Sequence Count [13:0] (14b) |
| 4 | 2 | `packetDataLength` | length of user data field **minus 1** (16b) |

* **PVN** is always `0b000` for SPP.
* **Packet Type** = 0 for telemetry (downlink), 1 for telecommand (uplink).
* **APID** (11 bits) carries the F´ packet descriptor (see
  [Packet Descriptor / APID](#packet-descriptor--apid)).
* **Sequence Flags** = `0b11` (unsegmented user data).
* **`packetDataLength`** is the number of user-data bytes minus one (CCSDS
  standard). A value of `N` means `N + 1` bytes of user data follow the header.

**User data field:** the packet payload bytes (e.g. a serialized `TlmPacket`).
The APID is the descriptor; it is **not** duplicated inside the user data
field.

```
+-----------------------------------------------+-------------------------+
| Space Packet Primary Header (6 bytes)         | User Data Field (N+1)   |
| pktId(2) | seqCtrl(2) | dataLength = N (2)    | payload bytes           |
+-----------------------------------------------+-------------------------+
```

### TM Transfer Frame

Fixed-size telemetry transfer frame (`TMHeader` / `TMTrailer`, produced by
`Svc/Ccsds/TmFramer`). The total frame size is fixed at
`ComCfg.TmFrameFixedSize` (default 1024 bytes); unused space in the data field
is filled with an SPP idle packet.

**Primary header (6 bytes):**

| Offset | Size | Field | Sub-fields |
| --- | --- | --- | --- |
| 0 | 2 | `globalVcId` | Transfer Frame Version [15:14] (2b) \| Spacecraft ID [13:4] (10b) \| Virtual Channel ID [3:1] (3b) \| OCF flag [0] (1b) |
| 2 | 1 | `masterFrameCount` | Master Channel Frame Count |
| 3 | 1 | `virtualFrameCount` | Virtual Channel Frame Count |
| 4 | 2 | `dataFieldStatus` | Sec. Hdr flag [15] \| Sync flag [14] \| Pkt Order flag [13] \| Segment Length ID [12:11] (2b) \| First Header Pointer [10:0] (11b) |

* Segment Length ID is set to `0b11` per standard.
* First Header Pointer is `0` (a single packet starts at offset 0 of the data
  field).

**Data field:** the Space Packet(s); a single SPP packet at offset 0, the
remainder padded with an idle packet to reach the fixed frame size.

**Trailer — `TMTrailer` (2 bytes):** `fecf` (CRC16) over the whole frame minus
the FECF.

```
+------------------------------+------------------------+-----------+
| TM Primary Header (6 bytes)  | Data Field (+ idle)    | FECF (2)  |
+------------------------------+------------------------+-----------+
   <----------------------- TmFrameFixedSize ----------------------->
```

### TC Transfer Frame

Telecommand transfer frame, consumed on uplink by `Svc/Ccsds/TcDeframer`
(`TCHeader` / `TCTrailer`). F´ uses TC Type-BD frames (bypass FARM, data).

**Primary header (5 bytes):**

| Offset | Size | Field | Sub-fields |
| --- | --- | --- | --- |
| 0 | 2 | `flagsAndScId` | TF Version [15:14] (2b) \| Bypass Flag [13] \| Control Cmd Flag [12] \| Reserved [11:10] (2b) \| Spacecraft ID [9:0] (10b) |
| 2 | 2 | `vcIdAndLength` | Virtual Channel ID [15:10] (6b) \| Frame Length [9:0] (10b) |
| 4 | 1 | `frameSequenceNum` | Frame Sequence Number (unused for Type-B) |

* **Frame Length** is the total frame length in bytes **minus 1** (CCSDS
  standard); the deframer adds 1 to recover the byte length.

**Data field:** the Space Packet(s).

**Trailer — `TCTrailer` (2 bytes):** `fecf` (CRC16) over the whole frame minus
the FECF.

```
+-----------------------------+------------------------+-----------+
| TC Primary Header (5 bytes) | Data Field             | FECF (2)  |
+-----------------------------+------------------------+-----------+
   <-------- Frame Length (= total bytes - 1, +1 to decode) ------->
```

### AOS Transfer Frame

Advanced Orbiting Systems transfer frame, produced by `Svc/Ccsds/AosFramer`
(`AOSHeader`, `M_PDUHeader`, `AOSTrailer`). Fixed-size frame
(`configure(fixedFrameSize, ...)`, bounded by `ComCfg.AosMaxFrameFixedSize`).

**Primary header — `AOSHeader` (6 bytes):**

| Offset | Size | Field | Sub-fields |
| --- | --- | --- | --- |
| 0 | 2 | `globalVcId` | Transfer Frame Version [15:14] (2b) \| Spacecraft ID LSB [13:6] (8b) \| Virtual Channel ID [5:0] (6b) |
| 2 | 4 | `frameCountAndSignaling` | VC Frame Count [31:8] (24b) \| Replay flag [7] \| VC frame count cycle flag [6] \| Spacecraft ID MSB [5:4] (2b) \| VC frame count cycle [3:0] (4b) |

**M_PDU header — `M_PDUHeader` (2 bytes):**

| Offset | Size | Field | Notes |
| --- | --- | --- | --- |
| 6 | 2 | `firstHeaderPointer` | byte offset of the first new packet header. `0xFFFF` = no packet starts in frame; `0xFFFE` = idle data only |

**Data field (M_PDU packet zone):** Space Packet(s).

**Trailer — `AOSTrailer` (2 bytes, optional):** `fecf` (CRC16) over the whole
frame minus the FECF. Present when the FECF is enabled in `configure(...)`.

```
+----------------------+----------------+--------------------+-----------+
| AOS Header (6 bytes)  | M_PDU Hdr (2) | M_PDU Packet Zone  | FECF (2)? |
+----------------------+----------------+--------------------+-----------+
   <--------------------------- fixedFrameSize ------------------------->
```

---

## F´ Framing (`FprimeProtocol`)

The native F´ frame is defined in `Svc/FprimeProtocol/FprimeProtocol.fpp` and
implemented by `Svc/FprimeFramer` (framing), `Svc/FprimeDeframer` and
`Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector` (deframing).

Wire layout:

```
[ startWord ][ lengthField ][ packetDescriptor ][ payload ][ crcField ]
   4 bytes       4 bytes          2 bytes          N bytes    4 bytes
```

### Frame header — `FrameHeader` (10 bytes)

| Offset | Size | Field | Type | Notes |
| --- | --- | --- | --- | --- |
| 0 | 4 | `startWord` | `TokenType` (U32) | constant `0xDEADBEEF` |
| 4 | 4 | `lengthField` | `TokenType` (U32) | counts `packetDescriptor` + `payload` bytes |
| 8 | 2 | `packetDescriptor` | `FwPacketDescriptorType` (U16) | packet type / APID (see [below](#packet-descriptor--apid)) |

### Payload

`payload` immediately follows the header at offset 10. It is the serialized
packet payload (a `TlmPacket`, `LogPacket`, `CmdPacket`, `FilePacket`, etc.).
The packet type is given by `packetDescriptor`; the payload itself does not
repeat the descriptor.

### Frame trailer — `FrameTrailer` (4 bytes)

| Offset | Size | Field | Type | Notes |
| --- | --- | --- | --- | --- |
| 10 + N | 4 | `crcField` | `U32` | CRC-32 over `header + payload` (everything preceding the trailer) |

The CRC is computed with the framework hash (`Utils/Hash`, configured to
CRC-32) over the header and payload bytes — i.e. the entire frame except the
4-byte trailer — and stored big-endian.

### `lengthField` semantics

`lengthField` counts the **`packetDescriptor` (2 bytes) plus the payload (N
bytes)** — it does **not** count the `startWord`, the `lengthField` itself, or
the trailer CRC. Therefore:

```
lengthField        = sizeof(FwPacketDescriptorType) + N        = 2 + N
payload size N     = lengthField - sizeof(FwPacketDescriptorType)
total frame bytes  = FrameHeader::SERIALIZED_SIZE (10) + N + FrameTrailer::SERIALIZED_SIZE (4)
                   = 10 + (lengthField - 2) + 4
```

A deframer rejects any frame whose `lengthField` is smaller than
`sizeof(FwPacketDescriptorType)` (it could not contain the descriptor).

**Full frame byte map** (payload of N bytes):

| Offset | Size | Field |
| --- | --- | --- |
| 0 | 4 | `startWord` = `0xDEADBEEF` |
| 4 | 4 | `lengthField` = `2 + N` |
| 8 | 2 | `packetDescriptor` |
| 10 | N | payload |
| 10 + N | 4 | `crcField` (CRC32) |

---

## Packet Formats

Packet payloads are the bytes carried in the framing layer's payload / user
data field. Each packet's *type* is identified by a packet descriptor (APID),
carried by the framing layer (the `FprimeProtocol` `packetDescriptor` field or
the Space Packet APID), **not** embedded at the front of the payload.

### Packet Descriptor / APID

Defined by the `ComCfg.Apid` enum (`config/ComCfg.fpp`), of underlying type
`FwPacketDescriptorType` (`U16`):

| Name | Value | Direction | Payload format |
| --- | --- | --- | --- |
| `FW_PACKET_COMMAND` | `0x0000` | uplink | [`CmdPacket`](#cmdpacket) |
| `FW_PACKET_TELEM` | `0x0001` | downlink | [`TlmPacket`](#tlmpacket) |
| `FW_PACKET_LOG` | `0x0002` | downlink | [`LogPacket`](#logpacket) |
| `FW_PACKET_FILE` | `0x0003` | both | [`FilePacket`](#filepacket) |
| `FW_PACKET_PACKETIZED_TLM` | `0x0004` | downlink | packetized telemetry |
| `FW_PACKET_DP` | `0x0005` | downlink | data product |
| `FW_PACKET_IDLE` | `0x0006` | — | idle |
| `FW_PACKET_HAND` | `0x00FE` | — | handshake |
| `FW_PACKET_UNKNOWN` | `0x00FF` | — | unknown |
| `SPP_IDLE_PACKET` | `0x07FF` | — | SPP idle (all-ones APID) |

### TlmPacket

Telemetry packet (`Fw/Tlm/TlmPacket`). Descriptor `FW_PACKET_TELEM`. A
`TlmPacket` carries a count followed by one or more channel-value entries.

**Packet body (`serializeTo`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 8 | `numEntries` | `FwSizeType` (U64 on the unix platform) |
| 2 | variable | entries | `numEntries` × channel entry (below) |

**Channel value entry (`addValue`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 4 | channel id | `FwChanIdType` (U32) |
| 2 | 11 | time tag | `Fw::Time` |
| 3 | variable | value bytes | raw channel value (no length prefix) |

> The per-entry value is stored without a length prefix, so a reader must know
> each channel's value size from the dictionary to walk multiple entries.

### LogPacket

Event / log packet (`Fw/Log/LogPacket`). Descriptor `FW_PACKET_LOG`.

**Packet body (`serializeTo`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 4 | event id | `FwEventIdType` (U32) |
| 2 | 11 | time tag | `Fw::Time` |
| 3 | variable | event arguments | `LogBuffer` bytes (no length prefix) |

The event-argument bytes occupy the remainder of the payload (serialized with
`OMIT_LENGTH`); the reader uses the framing length to bound them.

### CmdPacket

Command packet (`Fw/Cmd/CmdPacket`). Descriptor `FW_PACKET_COMMAND`. Uplink
only; F´ deserializes it (`deserializeFrom`).

**Packet body:**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 4 | opcode | `FwOpcodeType` (U32) |
| 2 | variable | command arguments | `CmdArgBuffer` bytes (no length prefix) |

The argument bytes occupy the remainder of the payload after the opcode.

### FilePacket

File transfer packet (`Fw/FilePacket`). Descriptor `FW_PACKET_FILE`. Used for
uplink and downlink file transfer. A `FilePacket` is a union of four packet
variants, all beginning with a common header.

**Common header — `FilePacket::Header` (5 bytes):**

| Offset | Size | Field | Type | Notes |
| --- | --- | --- | --- | --- |
| 0 | 1 | `type` | `U8` | `T_START=0`, `T_DATA=1`, `T_END=2`, `T_CANCEL=3` (`T_NONE=255`) |
| 1 | 4 | `sequenceIndex` | `U32` | packet sequence index |

**`PathName`** (used inside StartPacket):

| Offset | Size | Field | Type |
| --- | --- | --- | --- |
| 0 | 1 | length | `U8` (max 255) |
| 1 | `length` | path characters | raw bytes (no null terminator) |

**StartPacket (`T_START`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 5 | header | `Header` (type = `T_START`) |
| 2 | 4 | `fileSize` | `U32` |
| 3 | 1 + L₁ | source path | `PathName` |
| 4 | 1 + L₂ | destination path | `PathName` |

**DataPacket (`T_DATA`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 5 | header | `Header` (type = `T_DATA`) |
| 2 | 4 | `byteOffset` | `U32` |
| 3 | 2 | `dataSize` | `U16` |
| 4 | `dataSize` | file data | raw bytes |

**EndPacket (`T_END`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 5 | header | `Header` (type = `T_END`) |
| 2 | 4 | `checksum` | `U32` (CFDP checksum) |

**CancelPacket (`T_CANCEL`):**

| Order | Size | Field | Type |
| --- | --- | --- | --- |
| 1 | 5 | header | `Header` (type = `T_CANCEL`) |

---

## References

* `Svc/Ccsds/Types/Types.fpp` — CCSDS header/trailer struct definitions
* `Svc/Ccsds/{SpacePacketFramer,SpacePacketDeframer,TmFramer,TcDeframer,AosFramer,AosDeframer}` — CCSDS framing implementations
* `Svc/Ccsds/Utils/CRC16.hpp` — CCSDS FECF (CRC-16/CCITT)
* `Svc/FprimeProtocol/FprimeProtocol.fpp` — F´ `FrameHeader` / `FrameTrailer`
* `Svc/{FprimeFramer,FprimeDeframer}`, `Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector` — F´ framing implementations
* `Fw/Com/ComPacket`, `Fw/Tlm/TlmPacket`, `Fw/Log/LogPacket`, `Fw/Cmd/CmdPacket`, `Fw/FilePacket` — packet payload formats
* `config/FpConfig.fpp`, `config/ComCfg.fpp` — framework type widths and APID enum
* [Communications Adapter Interface](./communication-adapter-interface.md)
