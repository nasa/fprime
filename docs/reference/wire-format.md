# F´ Wire Format Reference

This document specifies the on-wire byte formats used by the F´ communications
stack: the individual packet payload formats (`TlmPacket`, `LogPacket`,
`CmdPacket`, `FilePacket`), and the framing layers that carry them — the native
F´ framing (`FprimeProtocol`) and the CCSDS Space Packet / transfer-frame
framing (Space Packet, TM, TC, AOS).

It is a reference for the bytes that travel over the link. It complements the
[Communications Adapter Interface](./communication-adapter-interface.md), which
describes the component-level interface contract rather than the byte layout.

The reading order mirrors how the bytes nest: a *packet payload* is produced
first, then *framed* for transport. See
[Putting It Together](#putting-it-together-encapsulation) at the end for the
full encapsulation picture.

## Conventions

* **Endianness.** All multi-byte integer fields are serialized **big-endian**
  (network byte order). This is the F´ default (`Fw::Endianness::BIG`).
* **Offsets.** Byte offsets are zero-based from the start of the relevant
  structure (packet or frame).
* **Sizes** are given in bytes (octets) unless noted as bits.

### Framework type widths

Packet/frame field widths depend on the framework configuration
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

---

## Packet Formats

Packet payloads are the bytes carried in the framing layer's payload / user
data field. Each packet's *type* is identified by a packet descriptor (APID),
carried by the framing layer (the `FprimeProtocol` `packetDescriptor` field or
the Space Packet APID) — **not** embedded at the front of the payload.

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

## Framing

A packet payload is wrapped by a framing protocol before it goes to the byte
stream / radio. F´ supports two framing stacks:

* **CCSDS** — the payload is carried as the user data of a CCSDS **Space
  Packet**, which is in turn carried inside a **TM**, **TC**, or **AOS**
  transfer frame. These are standardized CCSDS formats; this document does not
  re-specify their byte layouts — see the
  [CCSDS Blue Books](#ccsds-blue-book-references) for the authoritative
  specifications. The F´ implementations and the struct/bit definitions live in
  `Svc/Ccsds/Types/Types.fpp` and the components under `Svc/Ccsds/`.
* **`FprimeProtocol`** — the native F´ frame. Fully specified
  [below](#f-framing-fprimeprotocol).

### Space Packet (SPP)

The CCSDS Space Packet is the application-layer packet that carries an F´
packet payload as its user data field. **Format: see CCSDS 133.0-B-2 (Space
Packet Protocol)** — Blue Book references below.

F´-specific notes:

* The 11-bit **APID** in the Space Packet primary header carries the F´ packet
  descriptor (see [Packet Descriptor / APID](#packet-descriptor--apid)).
* The user data field is the packet payload bytes; the descriptor is **not**
  duplicated inside the user data field.
* Implemented by `Svc/Ccsds/SpacePacketFramer` and
  `Svc/Ccsds/SpacePacketDeframer`; header struct `SpacePacketHeader` in
  `Svc/Ccsds/Types/Types.fpp`.

### TM Transfer Frame

CCSDS Telemetry transfer frame used on downlink. **Format: see CCSDS 132.0-B
(TM Space Data Link Protocol).** Fixed-size frame (`ComCfg.TmFrameFixedSize`);
unused space is filled with an SPP idle packet. Implemented by
`Svc/Ccsds/TmFramer`; structs `TMHeader` / `TMTrailer` in `Types.fpp`.

### TC Transfer Frame

CCSDS Telecommand transfer frame used on uplink. **Format: see CCSDS 232.0-B
(TC Space Data Link Protocol).** F´ uses TC Type-BD frames. Consumed by
`Svc/Ccsds/TcDeframer`; structs `TCHeader` / `TCTrailer` in `Types.fpp`.

### AOS Transfer Frame

CCSDS Advanced Orbiting Systems transfer frame (with M_PDU packet zone), an
alternative downlink frame. **Format: see CCSDS 732.0-B-5 (AOS Space Data Link
Protocol).** Implemented by `Svc/Ccsds/AosFramer` / `Svc/Ccsds/AosDeframer`;
structs `AOSHeader` / `M_PDUHeader` / `AOSTrailer` in `Types.fpp`.

### Frame Error Control Field (FECF)

TM, TC, and AOS transfer frames carry a 2-byte FECF (CRC-16/CCITT, polynomial
`0x1021`, initial `0xFFFF`, final XOR `0x0000`) over the whole frame excluding
the FECF, as defined by the respective Space Data Link Protocol Blue Books.
Implemented in `Svc/Ccsds/Utils/CRC16.hpp`.

### CCSDS Blue Book references

The authoritative byte-level specifications for the CCSDS formats above are the
CCSDS Blue Books (publications listed at
<https://public.ccsds.org/Publications/BlueBooks.aspx>):

| Format | Blue Book |
| --- | --- |
| Space Packet | CCSDS 133.0-B-2 — Space Packet Protocol |
| Encapsulation Packet | CCSDS 133.1-B-3 — Encapsulation Packet Protocol |
| TM transfer frame | CCSDS 132.0-B — TM Space Data Link Protocol |
| TC transfer frame | CCSDS 232.0-B — TC Space Data Link Protocol |
| AOS transfer frame | CCSDS 732.0-B-5 — AOS Space Data Link Protocol |

### F´ Framing (`FprimeProtocol`)

The native F´ frame is defined in `Svc/FprimeProtocol/FprimeProtocol.fpp` and
implemented by `Svc/FprimeFramer` (framing), `Svc/FprimeDeframer` and
`Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector` (deframing). Unlike the
CCSDS formats, this protocol is F´-specific and is fully specified here.

Wire layout:

```
[ startWord ][ lengthField ][ packetDescriptor ][ payload ][ crcField ]
   4 bytes       4 bytes          2 bytes          N bytes    4 bytes
```

**Frame header — `FrameHeader` (10 bytes):**

| Offset | Size | Field | Type | Notes |
| --- | --- | --- | --- | --- |
| 0 | 4 | `startWord` | `TokenType` (U32) | constant `0xDEADBEEF` |
| 4 | 4 | `lengthField` | `TokenType` (U32) | counts `packetDescriptor` + `payload` bytes |
| 8 | 2 | `packetDescriptor` | `FwPacketDescriptorType` (U16) | packet type / APID (see [Packet Descriptor / APID](#packet-descriptor--apid)) |

**Payload:** immediately follows the header at offset 10. It is the serialized
packet payload (a `TlmPacket`, `LogPacket`, `CmdPacket`, `FilePacket`, etc.).
The packet type is given by `packetDescriptor`; the payload itself does not
repeat the descriptor.

**Frame trailer — `FrameTrailer` (4 bytes):**

| Offset | Size | Field | Type | Notes |
| --- | --- | --- | --- | --- |
| 10 + N | 4 | `crcField` | `U32` | CRC-32 over `header + payload` (everything preceding the trailer) |

The CRC is computed with the framework hash (`Utils/Hash`, configured to
CRC-32) over the header and payload bytes — i.e. the entire frame except the
4-byte trailer — and stored big-endian.

**`lengthField` semantics.** `lengthField` counts the **`packetDescriptor` (2
bytes) plus the payload (N bytes)** — it does **not** count the `startWord`,
the `lengthField` itself, or the trailer CRC. Therefore:

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

## Putting It Together (Encapsulation)

A packet payload is produced by the application, tagged with a packet
descriptor (APID), and then wrapped by one of the two framing stacks. The
descriptor/APID is what tells the receiver which packet format the innermost
bytes are.

### F´ native framing

The payload is wrapped directly in a single `FprimeProtocol` frame; the
descriptor sits in the frame header:

```
 packet payload  (TlmPacket | LogPacket | CmdPacket | FilePacket)
        │   descriptor = APID
        ▼
+-----------------------------------------------------------------------+
| startWord(4) | lengthField(4) | packetDescriptor(2) | payload(N) | CRC(4) |
+-----------------------------------------------------------------------+
                                  └──── lengthField counts these (2 + N) ────┘
```

### CCSDS framing

The payload becomes the user data of a Space Packet (APID = descriptor), which
is then encapsulated in a transfer frame:

```
 packet payload  (TlmPacket | LogPacket | CmdPacket | FilePacket)
        │   descriptor = APID
        ▼
 Space Packet:   [ SP primary header (carries APID) | user data = payload ]
        ▼
 transfer frame: [ TF header | ... | Space Packet(s) (+ idle fill) | FECF ]
                   TM / AOS  (downlink)        or        TC  (uplink)
```

* **Downlink:** `TlmPacket` / `LogPacket` → Space Packet → **TM** (or **AOS**)
  transfer frame.
* **Uplink:** `CmdPacket` / `FilePacket` → Space Packet → **TC** transfer frame.

In both stacks the descriptor is carried *once*: in the `FprimeProtocol`
`packetDescriptor` field, or in the Space Packet primary-header APID. The packet
payload bytes themselves are identical regardless of which framing stack
carries them.

---

## References

* `Svc/FprimeProtocol/FprimeProtocol.fpp` — F´ `FrameHeader` / `FrameTrailer`
* `Svc/{FprimeFramer,FprimeDeframer}`, `Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector` — F´ framing implementations
* `Svc/Ccsds/Types/Types.fpp` — CCSDS header/trailer struct definitions
* `Svc/Ccsds/{SpacePacketFramer,SpacePacketDeframer,TmFramer,TcDeframer,AosFramer,AosDeframer}` — CCSDS framing implementations
* `Svc/Ccsds/Utils/CRC16.hpp` — CCSDS FECF (CRC-16/CCITT)
* `Fw/Com/ComPacket`, `Fw/Tlm/TlmPacket`, `Fw/Log/LogPacket`, `Fw/Cmd/CmdPacket`, `Fw/FilePacket` — packet payload formats
* `config/FpConfig.fpp`, `config/ComCfg.fpp` — framework type widths and APID enum
* [CCSDS Blue Books](https://public.ccsds.org/Publications/BlueBooks.aspx) — authoritative CCSDS format specifications
* [Communications Adapter Interface](./communication-adapter-interface.md)
