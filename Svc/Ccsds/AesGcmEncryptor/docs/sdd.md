# Svc::Ccsds::AesGcmEncryptor

The `Svc::Ccsds::AesGcmEncryptor` component encrypts and authenticates downlink data under AES-256-GCM, per CCSDS [Space Data Link Security Protocol (CCSDS 355.0-B-2)](https://ccsds.org/Pubs/355x0b2.pdf). It implements `Svc.Ccsds.CcsdsSdlsEncrypt` toward an encryption client (typically [`Svc::Ccsds::CcsdsSdlsFramer`](../../CcsdsSdlsFramer/docs/sdd.md) via [`Svc::Ccsds::SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)) and `Svc.Ccsds.SdlsKeyInterfaceClient` toward a key supplier such as [`Svc::Ccsds::SdlsFileKeyManager`](../../SdlsFileKeyManager/docs/sdd.md). It is the security-providing alternative to [`Svc::Ccsds::ClearTextEncryptor`](../../ClearTextEncryptor/docs/sdd.md), and the downlink mirror of [`Svc::Ccsds::AesGcmDecryptor`](../../AesGcmDecryptor/docs/sdd.md).

## Introduction

Encrypting a frame proceeds as follows:

1. Receive an SA index, plaintext buffer, and frame context on `encryptIn`.
2. Request the AES-256 key for the frame's security association via `keyGet`; on failure or a wrong-sized key, report `KEY_ERROR`.
3. Take the next IV from a 96-bit big-endian sequence (wrapping at 2^96), encrypt the plaintext into a per-instance output store, authenticating the SA index and virtual channel as AES-GCM additional authenticated data (AAD).
4. Emit `IV (12) | ciphertext | MAC (16)` on `encryptOut` with status `SUCCESS`.
5. Return the plaintext buffer via `bufferReturnOut` on every path — it is copied, not encrypted in place.

The AAD is built by `Svc::Ccsds::Utils::SdlsTmAuthMask`, whose layout matches the ground segment's independent implementation of the same contract. The TM primary header does not yet exist at encryption time, so the virtual channel comes from the frame context — the same field `Svc::Ccsds::TmFramer` reads when it builds that header downstream, so the AAD always authenticates the VC actually transmitted.

The output store is a single per-instance buffer; it must be returned on `encryptReturnIn` before the next frame, and a frame arriving while it is still in flight is dropped with `ENCRYPTION_FAILURE` and the `OutputBufferBusy` event rather than overwriting unsent ciphertext.

## Requirements

| Name | Description | Rationale | Validation |
|---|---|---|---|
| SVC-CCSDS-AES-ENCRYPTOR-001 | On a successful request, the AesGcmEncryptor shall emit on `encryptOut` a buffer laid out as IV (12) \| ciphertext \| MAC (16), with status `SUCCESS`. | The security header and trailer `Svc.Ccsds.CcsdsSdlsFramer` expects. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-002 | The AesGcmEncryptor shall authenticate, as AES-GCM AAD, the masked TM primary header carrying the virtual channel ID together with the SA index. | CCSDS 355.0-B-2 binds a frame to its VC and SA. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-003 | The AesGcmEncryptor shall place in each emitted frame the next value of a 96-bit big-endian IV sequence, incremented once per frame that reaches the cipher and never repeated within the sequence. | A repeated IV under one key forfeits GCM's confidentiality and authenticity; a sequential IV lets the receiver apply an anti-replay window, as YAMCS does. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-004 | The AesGcmEncryptor shall return `KEY_ERROR` when the key manager reports failure or supplies a key that is not AES-256 sized. | A wrong-sized key would otherwise encrypt under unintended material. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-005 | The AesGcmEncryptor shall return `ENCRYPTION_FAILURE` when the plaintext plus IV and MAC would exceed the TM data field less the 2-byte SA index (`ComCfg.TmFrameFixedSize` - 6 - 2 - 2). | A larger output would overrun the transfer frame data field in `Svc.Ccsds.TmFramer`. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-006 | The AesGcmEncryptor shall return the incoming plaintext buffer on `bufferReturnOut`, unmodified. | The plaintext belongs to its sender and is not encrypted in place. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-007 | The AesGcmEncryptor shall mark its output store available again when the emitted buffer arrives on `encryptReturnIn`. | The store is component memory, not allocator memory. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-008 | The AesGcmEncryptor shall authenticate, in the AAD, the virtual channel ID carried on the frame context. | The TM primary header does not exist at encryption time; the context carries the VC that `Svc::Ccsds::TmFramer` will write into it. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-009 | The AesGcmEncryptor shall drop a frame with `ENCRYPTION_FAILURE` and the `OutputBufferBusy` event, rather than overwrite its output store, when a previously emitted frame has not yet returned on `encryptReturnIn`. | The store is a single buffer; overwriting it would let the new frame's MAC cover a silent substitution. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-010 | The AesGcmEncryptor shall emit on `encryptOut` only its own output store, and shall return the incoming plaintext on `bufferReturnOut` on every path, with a failed frame reporting its status against an empty buffer. | Keeps buffer ownership unambiguous on the return path. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-011 | The AesGcmEncryptor shall wrap the IV sequence from all ones to zero, and shall start the sequence from the value given to `setNextIv()`, or zero if none was given. | The sequence is a modular counter the ground segment mirrors; a deployment restores it across restarts so no IV recurs under one key. | Unit Test |

## Design

The component is passive with no commands, telemetry, or parameters, and allocates no memory after construction. It composes two interfaces:

| Kind | Name | Port Type | Description |
|---|---|---|---|
| guarded input | encryptIn | Svc.Ccsds.CcsdsSdlsEncryption | Receives the SA index and plaintext buffer to encrypt. |
| output | encryptOut | Svc.Ccsds.CcsdsSdlsData | Sends the operation status and encrypted data downstream. |
| sync input | encryptReturnIn | Svc.ComDataWithContext | Receives back ownership of buffers sent on `encryptOut`. |
| output | bufferReturnOut | Svc.ComDataWithContext | Returns the incoming plaintext buffer for deallocation. |
| output | keyGet | Svc.Ccsds.SdlsKey | Requests the AES-256 key for the frame's security association, passing the SA index with the request. |

Events: `OutputBufferBusy` (WARNING_HI).

The SA index is passed on every `keyGet` request, but whether it selects the key is up to the connected key source. Per `Svc.Ccsds.SdlsKeyInterface`, an implementation keyed per SA documents which indices it serves and returns `KEY_ERROR` for any other, while one that ignores the index says so. The in-tree `Svc.Ccsds.SdlsFileKeyManager` ignores it and returns its single file key for every SA — adequate where the deployment uses one key across its SAs by design, but note that SAs sharing a key also share its IV budget (see Security Considerations).

## Configuration

Compile time: none of its own. The output store is sized directly from `ComCfg.TmFrameFixedSize`, the project-overridable TM frame size every deployment already sets; the accepted output is bounded by the TM data field less the SA index that `Svc.Ccsds.CcsdsSdlsFramer` prepends.

Runtime: `setNextIv(iv)` seeds the IV sequence during topology setup; without it the sequence starts at zero, which matches a YAMCS receiver with no `initialSeqNum` configured. The constructor builds the `EVP_CIPHER_CTX` every frame reuses, which is what keeps `encryptIn` free of dynamic allocation, and the authenticated virtual channel arrives per frame on the frame context.

## Security Considerations

Each frame is encrypted under the next value of a 96-bit IV sequence, the deterministic construction of NIST SP 800-38D §8.2.1 with the whole IV as the invocation field. A repeated IV under one key forfeits both confidentiality and authenticity for GCM, so this is the property the design protects most carefully. The sequence is incremented the moment an IV is copied into the output store, so a frame that then fails in the cipher still retires its IV; a frame refused earlier (key error, oversize plaintext, busy store) never draws one, which keeps the sequence gap-free for the receiver's anti-replay window. It also lets the ground segment reject replays: YAMCS's `SecurityAssociationAes256Gcm128` treats the IV as its sequence number and, when `verifySeqNum` is set, accepts a frame only if the IV falls within `seqNumWindow` of the last one seen, and [`Svc::Ccsds::AesGcmDecryptor`](../../AesGcmDecryptor/docs/sdd.md) applies the same check on uplink.

A sequence has no birthday bound, but it has state, and the state is in process memory. Two consequences follow:

- **The sequence must survive a restart, or the key must not.** The component starts at zero (or at `setNextIv()`) every boot. Under a key that outlives the process — which the in-tree `Svc.Ccsds.SdlsFileKeyManager` serves by construction — a restart would reuse IVs already transmitted. A deployment must either persist the next IV and restore it through `setNextIv()`, or rotate keys on restart through a key manager of its own. With 2^96 values the sequence itself is not exhaustible in practice.
- **One sequence per instance, not per SA.** Several security associations served by one instance draw from the same sequence. This is safe (no IV recurs) and is what a receiver tracking one sequence per decryptor instance expects, but it means the per-SA IVs interleave rather than each counting from zero.

## Unit Testing

Direct tests covering the frame layout and AAD, the IV sequence (consecutive values from zero, seeding through `setNextIv()`, the all-ones wrap and a carry into the upper bytes, and that refused frames draw no IV), both key-error paths, the output-size bound, buffer ownership, and both busy-store cases (the drop, and back-to-back frames succeeding when the buffer returns from inside `encryptOut`, as `CcsdsSdlsFramer` does on a synchronous pipeline). The AAD and ciphertext are checked against an independently built reference rather than assumed self-consistent. Requirements are traced with `REQUIREMENT()` macros in the test main.

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsEncrypt.fpp`](../../Interfaces/CcsdsSdlsEncrypt.fpp)
- [`Svc/Ccsds/Interfaces/SdlsKey.fpp`](../../Interfaces/SdlsKey.fpp)
- [`Svc/Ccsds/AesGcmDecryptor`](../../AesGcmDecryptor/docs/sdd.md)
- [`Svc/Ccsds/CcsdsSdlsFramer`](../../CcsdsSdlsFramer/docs/sdd.md)
- [`Svc/Ccsds/SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)
