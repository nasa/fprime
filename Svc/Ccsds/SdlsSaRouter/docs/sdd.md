# Svc::Ccsds::SdlsSaRouter

The `Svc::Ccsds::SdlsSaRouter` component routes CCSDS SDLS (Space Data Link Security) encryption and decryption requests to downstream crypto components (encryptors or decryptors) based on the security association (SA) index. Its upstream ports mirror the `Svc.Ccsds.CcsdsSdlsEncrypt`/`CcsdsSdlsDecrypt` interfaces (which share the same port types) with operation-neutral names, and its downstream side provides arrays of the matching client-style ports (inlined, as FPP interfaces are not array-able at this time). Separate instances may serve the uplink (decryption) and downlink (encryption) paths.

The SA-to-port mapping is a compile-time FPP array of {`U16` SA, `FwIndexType` port index} pairs (`SdlsCfg.SaMap`, entry type `Svc.Ccsds.SaMapEntry` defined in `Svc/Ccsds/Types`). The map values and sizing constants are defined in a component-local configuration module (`Svc/Ccsds/SdlsSaRouter/config/SdlsSaRouterConfig/SdlsSaRouterCfg.fpp`, TlmPacketizer-style), which projects may override. This permits more SAs than ports, sparse SAs, or project-defined non-linear SA ranges that all map down to a linear, compact array of outputs.

## Functionality

- Loads the compile-time `SdlsCfg.SaMap` into an internal `Fw::ArrayMap` from SA index to port index at construction, so an instance is usable with no topology setup.
- Accepts a replacement table via `configure(const SdlsCfg::SaMap&)`, called during topology setup before any frame is routed. This exists so the uplink and downlink instances can hold different tables: an SA is simplex (CCSDS 355.0-B-2 §2.3.1.1), and giving both directions the same table would let each accept the other's SPI.
- Receives an SA index, iv/data buffer, and frame context on the guarded `dataIn` port.
- Looks up the SA index in the map; if found, forwards the request out the mapped `saDataOut` port.
- Passes an `UNKNOWN_SA` status forward on `dataOut` (with the untouched buffer) if the SA index has no map entry, or `UNKNOWN_PORT` if the mapped port index is out of range or unconnected; the buffer is not forwarded downstream in either case.
- Passes buffers returned by downstream crypto components (deallocation path) upstream via `bufferReturnOut`.
- Passes the operation status and processed data emitted by downstream crypto components upstream via `dataOut`, recording the originating port in an `Fw::ArrayMap` bookkeeping table (sized by `SdlsCfg.SaRouterMaxOutstandingBuffers`), and routes ownership returns received on `dataReturnIn` back to the originating crypto component via `saDataReturnOut` — or upstream via `bufferReturnOut` for buffers the router itself forwarded on routing errors.

## Port Descriptions

| Kind          | Name               | Port Type                       | Description |
|---------------|--------------------|---------------------------------|-------------|
| guarded input | dataIn          | Svc.Ccsds.CcsdsSdlsEncryption   | Receives the SA index and iv/data buffer to route. |
| output        | dataOut         | Svc.Ccsds.CcsdsSdlsData         | Sends the operation status and processed data (possibly newly allocated) upstream. |
| sync input    | dataReturnIn    | Svc.ComDataWithContext          | Receives back ownership of buffers sent on `dataOut`. |
| output        | bufferReturnOut    | Svc.ComDataWithContext          | Returns incoming iv/data buffers for deallocation. |
| output        | saDataOut       | [SdlsCfg.SaRouterPortCount] Svc.Ccsds.CcsdsSdlsEncryption | Sends the SA index and iv/data buffer to the mapped downstream crypto component. |
| sync input    | saDataIn        | [SdlsCfg.SaRouterPortCount] Svc.Ccsds.CcsdsSdlsData | Receives the operation status and processed data from downstream crypto components. |
| output        | saDataReturnOut | [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext | Returns ownership of processed data buffers to downstream crypto components. |
| sync input    | saBufferReturnIn   | [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext | Receives back iv/data buffers from downstream crypto components for deallocation. |

Three inputs are `sync` rather than `guarded`, for the same reason on both sides of the component. Downstream, `saDataIn` and `saBufferReturnIn` are called back synchronously by the crypto component on the caller's thread, which already holds the component guard. Upstream, `dataReturnIn` is called from inside the client's own `dataOut` handler: `CcsdsSdlsFramer` and `CcsdsSdlsDeframer` return the buffer before the routed `dataIn` call has unwound, so the guard is still held there too. `Os::Mutex` is `PTHREAD_MUTEX_ERRORCHECK`, so a `guarded` kind on any of the three would assert on the re-lock rather than deadlock. This is a topology constraint, not a convention: connected crypto components and upstream clients must be passive with synchronous handlers (see SVC-CCSDS-SDLS-SA-ROUTER-008).

Because those three handlers run unguarded, the outstanding-buffer tracking table they all touch is protected by its own `Os::Mutex`, held across each individual map operation rather than for the whole handler. The component guard alone would not cover them.

## Events

| Name | Severity | Description |
|------|----------|-------------|
| TrackingTableFull | WARNING_HI | The outstanding-buffer tracking table is full; the data was dropped and its buffer returned. |
| UntrackedBufferReturned | WARNING_HI | A returned buffer was not found in the tracking table; it was returned upstream. |

## Configuration

| Constant | Description |
|----------|-------------|
| `SdlsCfg.SaRouterPortCount` | Dimension of the downstream port arrays. |
| `SdlsCfg.SaRouterMapEntryCount` | Number of entries in the SA-to-port map (independent of port count). |
| `SdlsCfg.SaRouterMaxOutstandingBuffers` | Capacity of the outstanding processed-buffer bookkeeping table. |
| `SdlsCfg.SaRouterPorts` | Enumeration of the downstream crypto component ports (`UNCONNECTED = 0`, `PLAINTEXT = 1`). |
| `SdlsCfg.SaMap` | Compile-time array of {SA index, port index} pairs. Default: `{ SA 1 -> PLAINTEXT, SA 0 -> UNCONNECTED }`. |

All of the above are defined in the component-local configuration module `Svc/Ccsds/SdlsSaRouter/config/SdlsSaRouterConfig`.

The default map routes SA 1 to the `PLAINTEXT` port, matching the `SA_INDEX` parameter default in `Svc.Ccsds.CcsdsSdlsFramer`. SA 0 is mapped to the unconnected port rather than omitted: CCSDS 355.0-B-2 reserves it for Extended Procedures PDUs, so routing it yields `UNKNOWN_PORT` rather than `UNKNOWN_SA`.

A deployment overriding the configuration module changes the table for every instance at once. To give the uplink and downlink instances different tables — which the simplex-SA rule requires whenever the two directions do not route identically — call `configure()` on each instance instead.

## Requirements

| Name | Description | Validation |
|------|-------------|------------|
| SVC-CCSDS-SDLS-SA-ROUTER-001 | The SdlsSaRouter shall accept an SA index and iv/data buffer via its guarded `dataIn` port (mirroring the `CcsdsSdlsEncrypt`/`CcsdsSdlsDecrypt` interfaces). | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-002 | The SdlsSaRouter shall map the incoming SA index to a downstream port index using a compile-time SA-to-port map: an FPP array of {U16 SA, FwIndexType port index} pairs (`SdlsCfg.SaMap`) defined in configuration. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-003 | The SdlsSaRouter shall forward the SA index and buffer out the mapped output port, and shall pass the downstream crypto component's `SdlsStatus` forward alongside the processed data via `dataOut`. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-004 | The SdlsSaRouter shall receive returned iv/data buffers from downstream crypto components and pass them upstream via its return output. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-005 | Upon receiving an SA index with no map entry, the SdlsSaRouter shall pass an `UNKNOWN_SA` status forward on `dataOut` with the untouched buffer, without forwarding it downstream. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-006 | Upon a map entry referencing an out-of-range or unconnected port index, the SdlsSaRouter shall pass an `UNKNOWN_PORT` status forward on `dataOut` with the untouched buffer, without forwarding it downstream. | Unit Test |
| SVC-CCSDS-SDLS-SA-ROUTER-007 | The downstream port arrays shall share a single dimension set by a constant in the component configuration module; the SA-map array dimension shall be an independent config constant. | Inspection |
| SVC-CCSDS-SDLS-SA-ROUTER-008 | Downstream crypto components connected to `saDataOut`/`saDataReturnOut` shall invoke `saDataIn`/`saBufferReturnIn` synchronously on the caller's thread (i.e. be passive with synchronous handlers). Asynchronous (queued or hardware-backed) crypto components must not be connected to these ports without adding external synchronization. | Unit Test / Inspection |
| SVC-CCSDS-SDLS-SA-ROUTER-009 | The SdlsSaRouter shall accept a replacement SA-to-port map via `configure()`, applied before any frame is routed, so that the uplink and downlink instances may hold distinct tables as required for simplex security associations (CCSDS 355.0-B-2 §2.3.1.1). | Unit Test |

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsDecrypt.fpp`](../../Interfaces/CcsdsSdlsDecrypt.fpp)
- [`Svc/Ccsds/Interfaces/CcsdsSdlsEncrypt.fpp`](../../Interfaces/CcsdsSdlsEncrypt.fpp)
