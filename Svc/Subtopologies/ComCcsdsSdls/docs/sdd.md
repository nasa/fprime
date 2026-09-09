# ComCcsdsSdls (CCSDS Framing with SDLS Decryption) Subtopology — Software Design Document (SDD)

The **ComCcsdsSdls subtopologies** implement F´'s **CCSDS** communications stack for framing/deframing on the flight side, with an **SDLS (Space Data Link Security) decryption stage** inserted in the uplink path. As with `ComCcsds`, there are **two variants** in the same module:

1. A variant that **supplies a `Svc::ComStub`** implementation of `Svc.ComInterface` and expects to be wired to a **`Drv::ByteStreamDriverModel`** (TCP/UDP/UART, etc.), and
2. A variant that **expects an external implementation of [`Svc.ComInterface`](https://fprime.jpl.nasa.gov/latest/docs/reference/communication-adapter-interface/)** provided by the deployment.

Both variants are **composed from the `ComCcsds` layer topologies**: the `ComCcsds.SpacePacketFraming` packet layer and the `ComCcsds.TmTcFraming` transfer frame layer are imported and wired together through their **topology ports**, with the boxed **`SdlsDecryption` layer topology** (`CcsdsSdlsDeframer` → `SdlsSaRouter` → decryptor) inserted between them on the uplink path. Only the SDLS instances are defined in this module; the packet and frame layer instances remain in `ComCcsds` and are configured through `ComCcsdsConfig`.

> [!WARNING]
> The **defaults are `Svc.Ccsds.ClearTextDecryptor` and `Svc.Ccsds.ClearTextEncryptor`, which provide NO security** — no confidentiality, no integrity, and no authentication. Projects requiring security must override the configuration module to select real decryptor and encryptor implementations.

---

## 1. Requirements

| ID                   | Description                                                                                                                                              | Validation |
| -------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-COMCCSDSSDLS-001 | The subtopology shall provide the standard CCSDS framing/deframing communications stack by composing the `ComCcsds.SpacePacketFraming` and `ComCcsds.TmTcFraming` layer topologies, with SDLS decryption inserted in the uplink path. | Inspection |
| SVC-COMCCSDSSDLS-002 | The uplink path shall pass TC-deframed data through a `Svc.Ccsds.CcsdsSdlsDeframer`, which extracts the SA index and delegates decryption before Space Packet deframing. | Inspection |
| SVC-COMCCSDSSDLS-003 | Decryption requests shall be routed by SA index through a `Svc.Ccsds.SdlsSaRouter` to downstream decryptor instances.                                    | Inspection |
| SVC-COMCCSDSSDLS-004 | The decryptor choice shall be configurable via the subtopology configuration module, defaulting to `Svc.Ccsds.ClearTextDecryptor`.                       | Inspection |
| SVC-COMCCSDSSDLS-005 | The default SA map shall route SA 1 to the `PLAINTEXT` port (the default decryptor/encryptor); SA 0, reserved by CCSDS 355.0-B-2 for Extended Procedures, shall route to a port left unconnected. | Inspection |
| SVC-COMCCSDSSDLS-006 | The module shall provide a `FramingSubtopology` (external `Svc.ComInterface`) and a `Subtopology` (supplies `Svc::ComStub`) variant, mirroring ComCcsds. | Inspection |
| SVC-COMCCSDSSDLS-007 | The SDLS instance properties (base ID, decryptor and encryptor selection) shall be configurable via a `ComCcsdsSdlsConfig` module; the reused packet and frame layer instances remain configurable via `ComCcsdsConfig`. | Inspection |
| SVC-COMCCSDSSDLS-008 | The downlink path shall pad each buffer to `ComCfg.SdlsFillTargetSize` with a trailing idle Space Packet before encryption, so idle fill is covered by the security header and trailer rather than appended after them by the TM framer. | Inspection |

---

## 2. Design & Core Functions

### 2.1 Composition

The module defines one new layer topology and reuses two from `ComCcsds`:

| Layer topology                | Source        | Contents                                                                                       |
| ----------------------------- | ------------- | ----------------------------------------------------------------------------------------------- |
| `ComCcsds.SpacePacketFraming` | reused        | Router, ComQueue, aggregator, space packet framer/deframer, APID manager, comms buffer manager.  |
| `SdlsDecryption`              | this module   | `sdlsDeframer`, `decryptionSaRouter`, `decryptor` — the boxed SDLS decryption layer (see 2.2).   |
| `SdlsEncryption`              | this module   | `spacePacketIdleFiller`, `sdlsFramer`, `encryptionSaRouter`, `encryptor` — the boxed SDLS encryption layer. |
| `ComCcsds.TmTcFraming`        | reused        | TM framer (downlink), frame accumulator + TC deframer (uplink).                                  |

Instances defined in this module:

| Instance name  | Type (Svc)                      | Kind    | Purpose (core function)                                                                     |
| -------------- | ------------------------------- | ------- | -------------------------------------------------------------------------------------------- |
| `sdlsDeframer` | `Svc.Ccsds.CcsdsSdlsDeframer`   | Passive | Extracts the SA index from the SDLS frame and delegates decryption.                           |
| `decryptionSaRouter` | `Svc.Ccsds.SdlsSaRouter`  | Passive | Routes decryption requests by SA index to the mapped downstream decryptor.                    |
| `spacePacketIdleFiller` | `Svc.Ccsds.SpacePacketIdleFiller` | Passive | Pads each downlink buffer to `ComCfg.SdlsFillTargetSize` with a trailing idle Space Packet before encryption. |
| `sdlsFramer`   | `Svc.Ccsds.CcsdsSdlsFramer`     | Passive | Delegates encryption and prepends the SA index to build the SDLS frame.                       |
| `encryptionSaRouter` | `Svc.Ccsds.SdlsSaRouter`  | Passive | Routes encryption requests by SA index to the mapped downstream encryptor.                    |
| `decryptor`    | `Svc.Ccsds.ClearTextDecryptor`* | Passive | Default decryptor for the default SA (**pass-through, NO security**). *Configurable — see 2.3. |
| `encryptor`    | `Svc.Ccsds.ClearTextEncryptor`* | Passive | Default encryptor for the default SA (**pass-through, NO security**). *Configurable — see 2.3. |

The layers are wired together exclusively through their **topology ports** (e.g. `ComCcsds.TmTcFraming.dataOut -> SdlsDecryption.dataIn`, `SdlsDecryption.dataOut -> ComCcsds.SpacePacketFraming.dataIn`); the `Subtopology` variant additionally instantiates `ComCcsds.comStub`.

> **Two variants:**
> **A. "With ComStub" (`Subtopology`):** includes `Svc::ComStub` and exposes **ByteStream** ports to your driver.
> **B. "With External ComInterface" (`FramingSubtopology`):** you **provide** an `Svc.ComInterface` implementation in the deployment.

### 2.2 Data Flow (with SDLS)

```
[ ComCcsds.TmTcFraming ]      [    SdlsDecryption    ]      [ ComCcsds.SpacePacketFraming ]
frameAccumulator -> tcDeframer -> sdlsDeframer -> spacePacketDeframer -> fprimeRouter
                                       |  ^
                            decryptOut v  | decryptIn (decrypted data)
                            decryptionSaRouter
                                       |  ^
                          saDataOut[0] v  | saDataIn[0]
                                    decryptor
```

The downlink path inserts the mirrored `SdlsEncryption` layer between `SpacePacketFraming.dataOut` and `TmTcFraming.dataIn`, with the idle filler at its upstream boundary:

```
[ ComCcsds.SpacePacketFraming ]   [        SdlsEncryption        ]   [ ComCcsds.TmTcFraming ]
comQueue -> aggregator -> spacePacketIdleFiller -> sdlsFramer -> framer
                                                       |  ^
                                            encryptOut v  | encryptIn (encrypted data)
                                              encryptionSaRouter
                                                       |  ^
                                          saDataOut[1] v  | saDataIn[1]
                                                   encryptor
```

The `spacePacketIdleFiller` pads every buffer to `ComCfg.SdlsFillTargetSize` before it reaches the framer, appending a trailing idle Space Packet when the buffer is short. Because the padding happens upstream of encryption, the fill lands inside the authenticated data; were it left to `Svc.Ccsds.TmFramer`, it would be appended after the security trailer and fall outside the MAC. See 2.6 for the sizing contract this creates.

The `sdlsDeframer` extracts the leading 16-bit SA index, records it in the frame context, and sends the remaining iv/data to the `decryptionSaRouter`, which maps the SA to the decryptor on the mapped port. Decrypted data flows back through the router and deframer to the `spacePacketDeframer`. Buffer ownership returns flow the reverse paths (`dataReturnIn` → `decryptReturnOut` → decryptor; decryptor `bufferReturnOut` → router `bufferReturnOut` → deframer `dataReturnOut`).

### 2.3 Selecting Different Crypto Components

The `decryptor` and `encryptor` instances are defined in the configuration module (`ComCcsdsSdlsConfig/ComCcsdsSdlsConfig.fpp`), not in the subtopology itself. Projects override the configuration module (CMake `CONFIGURATION_OVERRIDES`) to instantiate different components implementing the `Svc.Ccsds.CcsdsSdlsDecrypt` and `Svc.Ccsds.CcsdsSdlsEncrypt` interfaces — for example the `Svc.Ccsds.AesGcmDecryptor` / `Svc.Ccsds.AesGcmEncryptor` pair, which additionally requires a key source wired to their `keyGet` ports and a matching `ComCfg` override for the added frame overhead (see 2.6). To route additional SAs to additional decryptors, also override the `SdlsSaRouter` configuration (`SdlsCfg.SaMap`, `SdlsCfg.SaRouterPortCount`) and connect the added router ports in the deployment topology.

### 2.4 Default SA Map

The `SdlsSaRouter` default configuration is two deep: `{ SA 1 -> SaRouterPorts.PLAINTEXT, SA 0 -> SaRouterPorts.UNCONNECTED }`. Each subtopology connects only the `PLAINTEXT` port (the default decryptor/encryptor), which the default map reaches with SA 1; this matches the `SA_INDEX` parameter default in `Svc.Ccsds.CcsdsSdlsFramer`, so downlink frames are routed to the default encryptor with no deployment configuration. SA 0 is mapped rather than omitted because CCSDS 355.0-B-2 reserves it for Extended Procedures PDUs: routing it to the unconnected port yields `UNKNOWN_PORT`. The SA mapping is configurable by overriding the `SdlsSaRouter` configuration module.

Uplink and downlink are separate simplex security associations (CCSDS 355.0-B-2 §2.3.1.1). Each direction has its own router instance and its own table, so both may carry the same SA index without either accepting the other's traffic; the TM and TC authenticated data differ in length and layout, so cross-direction authentication cannot succeed regardless.

### 2.5 Required Inputs for Operation

* **Rate Groups:** Connect a rate group to the **`comQueueRun`** (telemetry send rate) and **`aggregatorTimeout`** topology ports.
* **Transport Endpoint:** wire the ComStub ByteStream ports (variant A) or an external `Svc.ComInterface` (variant B) as documented in the usage note in `ComCcsdsSdls.fpp`.

No SDLS instance in this subtopology requires a `configure()` call. `spacePacketIdleFiller` takes its fill target from `ComCfg.SdlsFillTargetSize` at construction, and both SA routers build their tables from the `SdlsCfg.SaMap` default. A deployment that selects a real crypto implementation (see 2.3) adopts whatever setup that component requires — for the AES-GCM pair, a key source: `Svc.Ccsds.SdlsFileKeyManager` needs `configure(path, keySize)` before the first frame, and is not instantiated by this subtopology.

### 2.6 Downlink Sizing Contract

Two `ComCfg` constants govern the downlink path, and they are derived rather than written independently so they cannot drift apart:

| Constant | Default | Definition |
| --- | ---: | --- |
| `ComCfg.SdlsFillTargetSize` | 1014 | `TmFrameFixedSize - 6 (TM header) - 2 (SPI) - 2 (FECF)` — the plaintext size that exactly fills the TM transfer frame data field |
| `ComCfg.AggregationSize` | 1007 | `SdlsFillTargetSize - 7` — the aggregation buffer, held one minimum idle Space Packet (6-byte header + 1 data byte) below the target |

A project selecting a real encryptor subtracts that component's overhead from the target as well — 28 bytes more for AES-256-GCM (12-byte IV plus 16-byte MAC), giving 986 with `AggregationSize` 979 — by overriding `ComCfg` through CMake `CONFIGURATION_OVERRIDES`. Keeping `AggregationSize` defined in terms of `SdlsFillTargetSize` is the recommended form, since it preserves the relation automatically. `SpacePacketIdleFiller` `static_assert`s the target against both the frame data field and `AggregationSize`, so overriding one constant and not the other is a build failure rather than a run-time buffer drop.

## 3. Configuration

`ComCcsdsSdlsConfig` supplies the `BASE_ID` for the SDLS instances and the `decryptor` and `encryptor` instance definitions (see 2.3). The downlink fill target and aggregation buffer come from `ComCfg` (see 2.6). The reused packet and transfer frame layers are configured through `ComCcsdsConfig` (queue sizes, priorities, buffer sizing, memory allocator), exactly as when using `ComCcsds` directly.

## 4. See Also

- [ComCcsds subtopology](../../ComCcsds/docs/sdd.md)
- [`Svc::Ccsds::CcsdsSdlsDeframer`](../../../Ccsds/CcsdsSdlsDeframer/docs/sdd.md)
- [`Svc::Ccsds::CcsdsSdlsFramer`](../../../Ccsds/CcsdsSdlsFramer/docs/sdd.md)
- [`Svc::Ccsds::SdlsSaRouter`](../../../Ccsds/SdlsSaRouter/docs/sdd.md)
- [`Svc::Ccsds::SpacePacketIdleFiller`](../../../Ccsds/SpacePacketIdleFiller/docs/sdd.md)
- [`Svc::Ccsds::ClearTextDecryptor`](../../../Ccsds/ClearTextDecryptor/docs/sdd.md)
- [`Svc::Ccsds::ClearTextEncryptor`](../../../Ccsds/ClearTextEncryptor/docs/sdd.md)
