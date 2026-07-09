# Svc::Ccsds::CcsdsSdlsDeframer

The `Svc::Ccsds::CcsdsSdlsDeframer` component deframes buffers containing CCSDS SDLS (Space Data Link Security) frames. It is both a deframer (implementing the `Svc.Deframer` interface within a deframing pipeline) and a decryption client (implementing `Svc.Ccsds.CcsdsSdlsDecryptClient` toward a decryption helper such as `Svc.Ccsds.SdlsSaRouter` or a decryptor component).

## Introduction

Deframing an SDLS frame proceeds as follows:

1. Check that the frame is long enough to contain the 16-bit security association (SA) index.
2. Extract the SA index from the front of the frame and record it in the frame context.
3. Remove the SA index and pass the remaining data, SA index, and updated context to the decryption helper.
4. Check the status passed forward with the decrypted data; on failure raise an event, notify `errorNotify`, and return the buffer to the decryption subsystem.
5. Pass successfully decrypted data downstream for further processing.

Buffer ownership follows the standard F Prime data-with-context return pattern: frames that cannot be processed (or fail decryption) are returned via `dataReturnOut`, decrypted data ownership returns from downstream (`dataReturnIn`) are routed back to the decryption helper (`decryptReturnOut`), and original frame buffers returned by the helper (`bufferReturnIn`) go back upstream via `dataReturnOut`.

## Requirements

| Name | Description | Rationale | Validation |
|---|---|---|---|
| SVC-CCSDS-SDLS-DEFRAMER-001 | The CcsdsSdlsDeframer shall accept framed SDLS data with frame context via the `Svc.Deframer` interface (guarded `dataIn`). | Standard deframing pipeline entry point. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-002 | Upon receiving a frame containing at least 16 bits, the CcsdsSdlsDeframer shall extract the leading 16-bit security association (SA) index and record it in the frame context (`saIndex`). | The SA index selects the decryption path and must accompany the data. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-003 | The CcsdsSdlsDeframer shall remove the SA index from the frame and pass the remaining data, the SA index, and the updated context to the decryption helper via `decryptOut`. | The decryption helper operates on the iv/data, not the SA header. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-004 | Upon receiving a frame shorter than 16 bits, the CcsdsSdlsDeframer shall emit the `InsufficientLength` WARNING_HI event, shall not invoke the decryption helper, and shall return the frame buffer via `dataReturnOut`. | Malformed input must be reported and its buffer must not leak. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-005 | Upon a non-SUCCESS status passed forward on `decryptIn`, the CcsdsSdlsDeframer shall emit the `DecryptionFailed` WARNING_HI event, notify `errorNotify` with `SDLS_DECRYPTION_FAILURE`, and return ownership of the accompanying buffer to the decryption subsystem via `decryptReturnOut`. | Decryption failures must be visible to the system and the buffer must not leak. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-006 | The CcsdsSdlsDeframer shall pass successfully decrypted data received on `decryptIn` downstream via `dataOut`. | Continues the deframing pipeline with plaintext data. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-007 | The CcsdsSdlsDeframer shall route ownership returns of downstream data (`dataReturnIn`) to the decryption helper via `decryptReturnOut`. | Decrypted buffers are owned by the decryption subsystem. | Unit test |
| SVC-CCSDS-SDLS-DEFRAMER-008 | The CcsdsSdlsDeframer shall return iv/data buffers received back from the decryption helper (`bufferReturnIn`) upstream via `dataReturnOut`. | Original frame buffers must return to their upstream allocator. | Unit test |

## Design

The component is passive with no commands, telemetry, or parameters. It composes two interfaces:

| Kind | Name | Port Type | Description |
|---|---|---|---|
| guarded input | dataIn | Svc.ComDataWithContext | Receives framed SDLS data (from `Svc.Deframer`). |
| output | dataOut | Svc.ComDataWithContext | Sends decrypted, deframed data downstream. |
| output | dataReturnOut | Svc.ComDataWithContext | Returns frame buffer ownership upstream. |
| sync input | dataReturnIn | Svc.ComDataWithContext | Receives back ownership of buffers sent on `dataOut`. |
| output | decryptOut | Svc.Ccsds.CcsdsSdlsEncryption | Sends the SA index and payload to the decryption helper. |
| sync input | decryptIn | Svc.Ccsds.CcsdsSdlsData | Receives the operation status and decrypted data from the helper. |
| output | decryptReturnOut | Svc.ComDataWithContext | Returns ownership of decrypted buffers to the helper. |
| sync input | bufferReturnIn | Svc.ComDataWithContext | Receives back the iv/data buffer sent on `decryptOut`. |
| output | errorNotify | Svc.Ccsds.ErrorNotify | Notifies the pipeline of frame errors. |

Events: `InsufficientLength` (WARNING_HI) and `DecryptionFailed` (WARNING_HI, carries the `SdlsStatus`).

## Configuration

None. The component requires no initialization-time configuration.

## Unit Testing

Rule-based testing (STest) with rules covering the nominal deframe path, both error paths, and the three data/ownership flow paths; a 10000-step randomized scenario interleaves all rules. Requirements are traced with `REQUIREMENT()` macros in the test main.

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsDecrypt.fpp`](../../Interfaces/CcsdsSdlsDecrypt.fpp)
- [`Svc/Ccsds/SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)
- [`Svc/Ccsds/ClearTextDecryptor`](../../ClearTextDecryptor/docs/sdd.md)
