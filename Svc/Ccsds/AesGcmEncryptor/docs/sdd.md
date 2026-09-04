# Svc::Ccsds::AesGcmEncryptor

The `Svc::Ccsds::AesGcmEncryptor` component encrypts and authenticates downlink data under AES-256-GCM, per CCSDS [Space Data Link Security Protocol (CCSDS 355.0-B-2)](https://ccsds.org/Pubs/355x0b2.pdf). It implements `Svc.Ccsds.CcsdsSdlsEncrypt` toward an encryption client (typically [`Svc::Ccsds::CcsdsSdlsFramer`](../../CcsdsSdlsFramer/docs/sdd.md) via [`Svc::Ccsds::SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)) and `Svc.Ccsds.SdlsKeyInterfaceClient` toward a key supplier such as [`Svc::Ccsds::SdlsFileKeyManager`](../../SdlsFileKeyManager/docs/sdd.md). It is the security-providing alternative to [`Svc::Ccsds::ClearTextEncryptor`](../../ClearTextEncryptor/docs/sdd.md), and the downlink mirror of [`Svc::Ccsds::AesGcmDecryptor`](../../AesGcmDecryptor/docs/sdd.md).

## Introduction

Encrypting a frame proceeds as follows:

1. Receive an SA index, plaintext buffer, and frame context on `encryptIn`.
2. Request the AES-256 key for the frame's security association via `keyGet`; on failure or a wrong-sized key, report `KEY_ERROR`.
3. Draw a fresh IV, encrypt the plaintext into a per-instance output store, authenticating the SA index and virtual channel as AES-GCM additional authenticated data (AAD).
4. Emit `IV (12) | ciphertext | MAC (16)` on `encryptOut` with status `SUCCESS`.
5. Return the plaintext buffer via `bufferReturnOut` on every path — it is copied, not encrypted in place.

The AAD is built by `Svc::Ccsds::Utils::SdlsTmAuthMask`, whose layout matches the ground segment's independent implementation of the same contract. The TM primary header does not yet exist at encryption time, so the virtual channel comes from the frame context — the same field `Svc::Ccsds::TmFramer` reads when it builds that header downstream, so the AAD always authenticates the VC actually transmitted.

The output store is a single per-instance buffer; it must be returned on `encryptReturnIn` before the next frame, and a frame arriving while it is still in flight is dropped with `ENCRYPTION_FAILURE` and the `OutputBufferBusy` event rather than overwriting unsent ciphertext.

## Requirements

| Name | Description | Rationale | Validation |
|---|---|---|---|
| SVC-CCSDS-AES-ENCRYPTOR-001 | On a successful request, the AesGcmEncryptor shall emit on `encryptOut` a buffer laid out as IV (12) \| ciphertext \| MAC (16), with status `SUCCESS`. | The security header and trailer `Svc.Ccsds.CcsdsSdlsFramer` expects. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-002 | The AesGcmEncryptor shall authenticate, as AES-GCM AAD, the masked TM primary header carrying the virtual channel ID together with the SA index. | CCSDS 355.0-B-2 binds a frame to its VC and SA. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-003 | The AesGcmEncryptor shall draw a fresh IV from the CSPRNG for every frame. | A repeated IV under one key forfeits GCM's confidentiality and authenticity. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-004 | The AesGcmEncryptor shall return `KEY_ERROR` when the key manager reports failure or supplies a key that is not AES-256 sized. | A wrong-sized key would otherwise encrypt under unintended material. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-005 | The AesGcmEncryptor shall return `ENCRYPTION_FAILURE` when the plaintext plus IV and MAC would exceed `SdlsCfg.AesMaxOutputSize`. | That is the size of the output store. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-006 | The AesGcmEncryptor shall return the incoming plaintext buffer on `bufferReturnOut`, unmodified. | The plaintext belongs to its sender and is not encrypted in place. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-007 | The AesGcmEncryptor shall mark its output store available again when the emitted buffer arrives on `encryptReturnIn`. | The store is component memory, not allocator memory. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-008 | The AesGcmEncryptor shall authenticate, in the AAD, the virtual channel ID carried on the frame context. | The TM primary header does not exist at encryption time; the context carries the VC that `Svc::Ccsds::TmFramer` will write into it. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-009 | The AesGcmEncryptor shall drop a frame with `ENCRYPTION_FAILURE` and the `OutputBufferBusy` event, rather than overwrite its output store, when a previously emitted frame has not yet returned on `encryptReturnIn`. | The store is a single buffer; overwriting it would let the new frame's MAC cover a silent substitution. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-010 | The AesGcmEncryptor shall emit on `encryptOut` only its own output store, and shall return the incoming plaintext on `bufferReturnOut` on every path, with a failed frame reporting its status against an empty buffer. | Keeps buffer ownership unambiguous on the return path. | Unit Test |
| SVC-CCSDS-AES-ENCRYPTOR-011 | The AesGcmEncryptor shall return `ENCRYPTION_FAILURE` rather than emit a frame if the CSPRNG cannot supply an IV. | Encrypting under a stale or predictable IV is worse than dropping the frame. | Inspection |

## Design

The component is passive with no commands, telemetry, or parameters, and allocates no memory after construction. It composes two interfaces:

| Kind | Name | Port Type | Description |
|---|---|---|---|
| guarded input | encryptIn | Svc.Ccsds.CcsdsSdlsEncryption | Receives the SA index and plaintext buffer to encrypt. |
| output | encryptOut | Svc.Ccsds.CcsdsSdlsData | Sends the operation status and encrypted data downstream. |
| sync input | encryptReturnIn | Svc.ComDataWithContext | Receives back ownership of buffers sent on `encryptOut`. |
| output | bufferReturnOut | Svc.ComDataWithContext | Returns the incoming plaintext buffer for deallocation. |
| output | keyGet | Svc.Ccsds.SdlsKey | Requests the AES-256 key bound to the frame's security association. |

Events: `OutputBufferBusy` (WARNING_HI).

## Configuration

Compile time, in `config/AesGcmEncryptorConfig` (project-overridable):

- `SdlsCfg.AesMaxOutputSize` — size of the output store, and the largest frame the component can emit.
- `SdlsCfg.AesFrameOverhead` — bytes AES-256-GCM adds to the plaintext (12-byte IV + 16-byte MAC); the component `static_assert`s that this agrees with its own IV and MAC lengths.

A deployment should hold `AesMaxOutputSize` to `AesFrameOverhead` plus its largest plaintext with a `static_assert`, as `TestDeploymentsProject/Ref` does in `Ref/Top/RefTopology.cpp`.

Runtime: none. The constructor builds the `EVP_CIPHER_CTX` every frame reuses, which is what keeps `encryptIn` free of dynamic allocation, and the authenticated virtual channel arrives per frame on the frame context.

## Unit Testing

Direct tests covering the frame layout and AAD, IV freshness, both key-error paths, the output-size bound, buffer ownership, and both busy-store cases (the drop, and back-to-back frames succeeding when the buffer returns from inside `encryptOut`, as `CcsdsSdlsFramer` does on a synchronous pipeline). The AAD and ciphertext are checked against an independently built reference rather than assumed self-consistent. Requirements are traced with `REQUIREMENT()` macros in the test main.

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsEncrypt.fpp`](../../Interfaces/CcsdsSdlsEncrypt.fpp)
- [`Svc/Ccsds/Interfaces/SdlsKey.fpp`](../../Interfaces/SdlsKey.fpp)
- [`Svc/Ccsds/AesGcmDecryptor`](../../AesGcmDecryptor/docs/sdd.md)
- [`Svc/Ccsds/CcsdsSdlsFramer`](../../CcsdsSdlsFramer/docs/sdd.md)
- [`Svc/Ccsds/SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)
