# Svc::Ccsds::AesGcmDecryptor

The `Svc::Ccsds::AesGcmDecryptor` component decrypts and authenticates uplink data under AES-256-GCM, per CCSDS. It implements `Svc.Ccsds.CcsdsSdlsDecrypt` toward a decryption client (typically [`Svc::Ccsds::CcsdsSdlsDeframer`](../../CcsdsSdlsDeframer/docs/sdd.md) via [`Svc::Ccsds::SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)) and `Svc.Ccsds.SdlsKeyInterfaceClient` toward a key supplier such as [`Svc::Ccsds::SdlsFileKeyManager`](../../SdlsFileKeyManager/docs/sdd.md). It is the security-providing alternative to [`Svc::Ccsds::ClearTextDecryptor`](../../ClearTextDecryptor/docs/sdd.md), and the uplink mirror of [`Svc::Ccsds::AesGcmEncryptor`](../../AesGcmEncryptor/docs/sdd.md).

## Introduction

Decrypting a frame proceeds as follows:

1. Receive an SA index, ciphertext buffer (`IV (12) | ciphertext | MAC (16)`), and frame context on `decryptIn`.
2. Reject a buffer too short to hold an IV and a MAC, or larger than `SdlsCfg.AesMaxInputSize`, with `DECRYPTION_FAILURE` — before requesting a key, since the uplink is attacker-influenced.
3. Request the AES-256 key for the frame's security association via `keyGet`; on failure or a wrong-sized key, report `KEY_ERROR`.
4. Decrypt in place, authenticating the SA index and virtual channel as AES-GCM additional authenticated data (AAD).
5. On a failed MAC check, report `MAC_VERIFICATION_FAILURE`, distinct from `DECRYPTION_FAILURE`; the component remains able to decrypt subsequent frames.
6. On success, narrow the buffer to the plaintext and emit it on `decryptOut` with status `SUCCESS`.

Decryption is in place, so the emitted buffer is the one received, advanced past the IV and narrowed to the plaintext length; `Fw::Buffer` keeps its allocation context independently of the data pointer, so it remains deallocatable by the issuing `Svc.BufferManager`. Buffers returned on `decryptReturnIn` are passed upstream via `bufferReturnOut` unconditionally, since every buffer emitted is the one that arrived.

The AAD is built by `Svc::Ccsds::Utils::SdlsTcAuthMask`, whose layout matches the ground segment's independent implementation of the same contract. The virtual channel comes from the frame context: `Svc::Ccsds::TcDeframer` reads it from the TC primary header and sets it on the context before stripping that header, so it is still available by decryption time.

## Requirements

| Name | Description | Rationale | Validation |
|---|---|---|---|
| SVC-CCSDS-AES-DECRYPTOR-001 | On a successful request, the AesGcmDecryptor shall decrypt the buffer in place and emit the plaintext on `decryptOut` with status `SUCCESS`. | Plaintext is never longer than ciphertext, so no second buffer is needed. | Unit Test |
| SVC-CCSDS-AES-DECRYPTOR-002 | The AesGcmDecryptor shall authenticate, as AES-GCM AAD, the masked TC primary header carrying the virtual channel ID together with the SA index, and reject a frame authenticated for any other VC or SA. | CCSDS 355.0-B-2 binds a frame to its VC and SA. | Unit Test |
| SVC-CCSDS-AES-DECRYPTOR-003 | The AesGcmDecryptor shall return `MAC_VERIFICATION_FAILURE`, distinct from `DECRYPTION_FAILURE`, when the IV, ciphertext, or MAC has been modified or the frame was not built for this SA and VC, and shall remain able to decrypt subsequent frames. | CCSDS 355.0-B-2 sect. 3.3.3.2 requires an authentication verdict distinguishable from a processing error; the two call for different ground responses. | Unit Test |
| SVC-CCSDS-AES-DECRYPTOR-004 | The AesGcmDecryptor shall return `DECRYPTION_FAILURE`, without requesting a key, for a buffer too short to hold an IV and a MAC or larger than `SdlsCfg.AesMaxInputSize`. | An uplink buffer is attacker-influenced and must be bounded before use; the ciphertext length is handed to OpenSSL as an `int`. | Unit Test |
| SVC-CCSDS-AES-DECRYPTOR-005 | The AesGcmDecryptor shall return `KEY_ERROR` when the key manager reports failure or supplies a key that is not AES-256 sized. | A wrong-sized key would otherwise decrypt under unintended material. | Unit Test |
| SVC-CCSDS-AES-DECRYPTOR-006 | The buffer emitted on `decryptOut` shall retain the allocation context and original allocation pointer of the buffer received on `decryptIn`. | `Svc.BufferManager` deallocates by context and asserts the data pointer lies within the slot it issued. | Unit Test |
| SVC-CCSDS-AES-DECRYPTOR-007 | A buffer received on `decryptReturnIn` shall be returned on `bufferReturnOut`. | Every buffer emitted is the one that arrived. | Unit Test |

## Design

The component is passive with no commands, telemetry, or parameters, and allocates no memory after construction. It composes two interfaces:

| Kind | Name | Port Type | Description |
|---|---|---|---|
| guarded input | decryptIn | Svc.Ccsds.CcsdsSdlsEncryption | Receives the SA index and ciphertext buffer to decrypt. |
| output | decryptOut | Svc.Ccsds.CcsdsSdlsData | Sends the operation status and decrypted data upstream. |
| sync input | decryptReturnIn | Svc.ComDataWithContext | Receives back ownership of buffers sent on `decryptOut`. |
| output | bufferReturnOut | Svc.ComDataWithContext | Returns the incoming buffer for deallocation. |
| output | keyGet | Svc.Ccsds.SdlsKey | Requests the AES-256 key bound to the frame's security association. |

The component emits no events: every outcome, including a failed authentication, is reported as an `SdlsStatus` on `decryptOut`.

## Configuration

Compile time, in `config/AesGcmDecryptorConfig` (project-overridable): `SdlsCfg.AesMaxInputSize` — the largest frame body accepted, as it arrives once the SA index has been stripped. A deployment sizes it against its TC frame length minus the primary header, the FECF, and the 2-byte SPI.

Runtime: none. The constructor builds the `EVP_CIPHER_CTX` every frame reuses, which is what keeps `decryptIn` free of dynamic allocation, and the authenticated virtual channel arrives per frame on the frame context. See [`Svc/Ccsds/AesGcmEncryptor`](../../AesGcmEncryptor/docs/sdd.md) for measured allocation counts; the decrypt path behaves the same way.

## Unit Testing

Direct tests covering in-place decryption and its allocation context, both shape-check rejections, both key-error paths, and the buffer return path. Authentication is exercised by tampering with the IV, ciphertext, and MAC in turn, and by presenting frames built for another VC and another SA, with a following good frame confirming the shared cipher context survives a rejection. The AAD and a known-answer frame are checked against an independently built reference rather than assumed self-consistent. Requirements are traced with `REQUIREMENT()` macros in the test main.

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsDecrypt.fpp`](../../Interfaces/CcsdsSdlsDecrypt.fpp)
- [`Svc/Ccsds/Interfaces/SdlsKey.fpp`](../../Interfaces/SdlsKey.fpp)
- [`Svc/Ccsds/AesGcmEncryptor`](../../AesGcmEncryptor/docs/sdd.md)
- [`Svc/Ccsds/CcsdsSdlsDeframer`](../../CcsdsSdlsDeframer/docs/sdd.md)
- [`Svc/Ccsds/SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)
