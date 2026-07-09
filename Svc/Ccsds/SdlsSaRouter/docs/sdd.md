# Svc::Ccsds::SdlsSaRouter

The `Svc::Ccsds::SdlsSaRouter` component routes CCSDS SDLS (Space Data Link Security) decryption requests to downstream decryptor components based on the security association (SA) index. It implements the `Svc.Ccsds.CcsdsSdlsDecrypt` interface on the upstream side, and on the downstream side provides arrays of `CcsdsSdlsDecryptClient`-style ports (inlined, as FPP interfaces are not array-able at this time).

The SA-to-port mapping is a compile-time FPP array of {`U16` SA, `FwIndexType` port index} pairs (`SdlsCfg.SaMap`, entry type `Svc.Ccsds.SaMapEntry` defined in `Svc/Ccsds/Types`). The map values and sizing constants are defined in a component-local configuration module (`Svc/Ccsds/SdlsSaRouter/config/SdlsSaRouterConfig/SdlsSaRouterCfg.fpp`, TlmPacketizer-style), which projects may override. This permits more SAs than ports, sparse SAs, or project-defined non-linear SA ranges that all map down to a linear, compact array of outputs.

## Functionality

- Loads the compile-time `SdlsCfg.SaMap` into an internal `Fw::ArrayMap` from SA index to port index at construction.
- Receives an SA index, iv/data buffer, and frame context on the guarded `decryptIn` port.
- Looks up the SA index in the map; if found, forwards the request out the mapped `saDecryptOut` port and returns the downstream decryptor's `SdlsStatus` to the caller.
- Returns `UNKNOWN_SA` if the SA index has no map entry, or `UNKNOWN_PORT` if the mapped port index is out of range or unconnected; the buffer is not forwarded in either case.
- Passes buffers returned by downstream decryptors (deallocation path) upstream via `bufferReturnOut`.
- Passes decrypted data emitted by downstream decryptors upstream via `decryptOut`, recording the originating port in an `Fw::ArrayMap` bookkeeping table (sized by `SdlsCfg.SaRouterMaxOutstandingBuffers`), and routes ownership returns received on `decryptReturnIn` back to the originating decryptor via `saDecryptReturnOut`.

## Port Descriptions

| Kind          | Name               | Port Type                       | Description |
|---------------|--------------------|---------------------------------|-------------|
| guarded input | decryptIn          | Svc.Ccsds.CcsdsSdlsEncryption   | Receives the SA index and iv/data buffer to route (from `CcsdsSdlsDecrypt` interface). |
| output        | decryptOut         | Svc.ComDataWithContext          | Sends decrypted data (possibly newly allocated) upstream. |
| sync input    | decryptReturnIn    | Svc.ComDataWithContext          | Receives back ownership of buffers sent on `decryptOut`. |
| output        | bufferReturnOut    | Svc.ComDataWithContext          | Returns incoming iv/data buffers for deallocation. |
| output        | saDecryptOut       | [SdlsCfg.SaRouterPortCount] Svc.Ccsds.CcsdsSdlsEncryption | Sends the SA index and iv/data buffer to the mapped downstream decryptor. |
| sync input    | saDecryptIn        | [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext | Receives decrypted data from downstream decryptors. |
| output        | saDecryptReturnOut | [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext | Returns ownership of decrypted data buffers to downstream decryptors. |
| sync input    | saBufferReturnIn   | [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext | Receives back iv/data buffers from downstream decryptors for deallocation. |

## Configuration

| Constant | Description |
|----------|-------------|
| `SdlsCfg.SaRouterPortCount` | Dimension of the downstream port arrays. |
| `SdlsCfg.SaRouterMapEntryCount` | Number of entries in the SA-to-port map (independent of port count). |
| `SdlsCfg.SaRouterMaxOutstandingBuffers` | Capacity of the outstanding decrypted buffer bookkeeping table. |
| `SdlsCfg.SaRouterPorts` | Enumeration of the downstream decryptor ports (`PLAINTEXT_DECRYPTION = 0`, `UNCONNECTED = 1`). |
| `SdlsCfg.SaMap` | Compile-time array of {SA index, port index} pairs. |

All of the above are defined in the component-local configuration module `Svc/Ccsds/SdlsSaRouter/config/SdlsSaRouterConfig`.

## Requirements

| Name | Description | Validation |
|------|-------------|------------|
| SVC-CCSDS-SDLS-SA-ROUTER-001 | The SdlsSaRouter shall accept an SA index and iv/data buffer via the `Svc.Ccsds.CcsdsSdlsDecrypt` interface (guarded `decryptIn`). | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-002 | The SdlsSaRouter shall map the incoming SA index to a downstream port index using a compile-time SA-to-port map: an FPP array of {U16 SA, FwIndexType port index} pairs (`SdlsCfg.SaMap`) defined in configuration. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-003 | The SdlsSaRouter shall forward the SA index and buffer out the mapped output port and shall return the downstream decryptor's `SdlsStatus` to its caller. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-004 | The SdlsSaRouter shall receive returned iv/data buffers from downstream decryptors and pass them upstream via its return output. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-005 | Upon receiving an SA index with no map entry, the SdlsSaRouter shall return `UNKNOWN_SA` without forwarding the buffer. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-006 | Upon a map entry referencing an out-of-range or unconnected port index, the SdlsSaRouter shall return `UNKNOWN_PORT` without forwarding the buffer. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-007 | The downstream port arrays shall share a single dimension set by a constant in the component configuration module; the SA-map array dimension shall be an independent config constant. | Inspection |

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsDecrypt.fpp`](../../Interfaces/CcsdsSdlsDecrypt.fpp)
