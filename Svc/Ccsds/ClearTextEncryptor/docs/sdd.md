# Svc::Ccsds::ClearTextEncryptor

> [!WARNING]
> **This component provides NO security.** It performs no authentication, no integrity
> protection, and no encryption. All data passes through unmodified. It is intended only
> for clear-mode operation (e.g. as the default encryptor in a standard subtopology) and
> for testing. Do not use it where confidentiality, integrity, or authenticity of the
> data link is required.

The `Svc::Ccsds::ClearTextEncryptor` component is a pass-through implementation of the `Svc.Ccsds.CcsdsSdlsEncrypt` interface. It is a pure pass-through in both dimensions: requests received on `encryptIn` are forwarded out `encryptOut` with the buffer and context unmodified (alongside an `SdlsStatus.SUCCESS` status), and buffer ownership returned on `encryptReturnIn` is passed upstream via `bufferReturnOut` for deallocation. No memory is allocated.

## Functionality

- Receives an SA index, iv/data buffer, and frame context on the guarded `encryptIn` port; the SA index is ignored.
- Forwards the buffer and context unmodified out `encryptOut` alongside an `SdlsStatus.SUCCESS` status.
- Passes ownership returns received on `encryptReturnIn` upstream via `bufferReturnOut` (the forwarded buffer is the incoming buffer, as no allocation occurs).

## Port Descriptions

| Kind          | Name            | Port Type                     | Description |
|---------------|-----------------|-------------------------------|-------------|
| guarded input | encryptIn       | Svc.Ccsds.CcsdsSdlsEncryption | Receives the SA index and iv/data buffer (from `CcsdsSdlsEncrypt` interface). |
| output        | encryptOut      | Svc.Ccsds.CcsdsSdlsData       | Sends the operation status and (unmodified) data downstream. |
| sync input    | encryptReturnIn | Svc.ComDataWithContext        | Receives back ownership of buffers sent on `encryptOut`. |
| output        | bufferReturnOut | Svc.ComDataWithContext        | Returns the incoming iv/data buffer for deallocation. |

## Requirements

| Name | Description | Validation |
|------|-------------|------------|
| SVC-CCSDS-CLEARTEXT-ENCRYPTOR-001 | The ClearTextEncryptor shall accept an SA index, iv/data buffer, and frame context via the `Svc.Ccsds.CcsdsSdlsEncrypt` interface (guarded `encryptIn`). | Unit Test |
| SVC-CCSDS-CLEARTEXT-ENCRYPTOR-002 | The ClearTextEncryptor shall perform no authentication and no encryption, passing the received buffer and context unmodified out `encryptOut`. | Unit Test |
| SVC-CCSDS-CLEARTEXT-ENCRYPTOR-003 | The ClearTextEncryptor shall pass an `SdlsStatus.SUCCESS` status forward for every request. | Unit Test |
| SVC-CCSDS-CLEARTEXT-ENCRYPTOR-004 | Upon receiving ownership of a buffer back on `encryptReturnIn`, the ClearTextEncryptor shall pass it upstream via `bufferReturnOut` for deallocation. | Unit Test |

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsEncrypt.fpp`](../../Interfaces/CcsdsSdlsEncrypt.fpp)
- [`Svc/Ccsds/ClearTextDecryptor`](../../ClearTextDecryptor/docs/sdd.md)
