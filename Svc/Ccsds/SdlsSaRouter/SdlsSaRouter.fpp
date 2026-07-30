module Svc {
module Ccsds {
    @ Routes SDLS encryption and decryption requests to downstream crypto components
    @ by mapping the incoming security association (SA) index to a downstream port
    @ index using a compile-time SA-to-port map (SdlsCfg.SaMap). Separate instances
    @ may serve the uplink (decryption) and downlink (encryption) paths.
    passive component SdlsSaRouter {

        # ----------------------------------------------------------------------
        # Upstream side: raw ports mirroring the CcsdsSdlsEncrypt/CcsdsSdlsDecrypt
        # interfaces, which share the same port types. Inlined (rather than
        # importing one of them) so the names apply to both operations.
        # ----------------------------------------------------------------------
        @ Port to receive the SA index and iv/data buffer to route for encryption/decryption
        guarded input port dataIn: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for sending the operation status and processed data (possibly newly allocated) upstream
        output port dataOut: Svc.Ccsds.CcsdsSdlsData

        @ Port for receiving back ownership of buffers sent on dataOut
        guarded input port dataReturnIn: Svc.ComDataWithContext

        @ Port for returning the incoming iv/data buffer for deallocation
        output port bufferReturnOut: Svc.ComDataWithContext

        # ----------------------------------------------------------------------
        # Downstream side: inlined arrays of client-side ports
        # (FPP interfaces are not array-able at this time)
        # These inputs are sync (not guarded): downstream crypto components must
        # be passive with synchronous handlers, calling back on the caller's
        # thread, which already holds the component guard (guarding them would
        # re-enter the mutex). Asynchronous crypto components must not be
        # connected without external synchronization (SVC-CCSDS-SDLS-SA-ROUTER-008).
        # ----------------------------------------------------------------------
        @ Ports for sending the SA index and iv/data buffer to the mapped downstream crypto component
        output port saDataOut: [SdlsCfg.SaRouterPortCount] Svc.Ccsds.CcsdsSdlsEncryption

        @ Ports for receiving the operation status and processed data (possibly newly allocated) from downstream crypto components
        sync input port saDataIn: [SdlsCfg.SaRouterPortCount] Svc.Ccsds.CcsdsSdlsData

        @ Ports for returning ownership of processed data buffers to downstream crypto components
        output port saDataReturnOut: [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext

        @ Ports for receiving back iv/data buffers from downstream crypto components for deallocation
        sync input port saBufferReturnIn: [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext

        ###############################################################################
        # Standard AC Ports: Required for Events                                      #
        ###############################################################################

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

        @ The outstanding-buffer tracking table is full; the data was dropped and its buffer returned
        event TrackingTableFull() severity warning high format "SA router tracking table full: data dropped and buffer returned"

        @ A returned buffer was not found in the tracking table; it was returned upstream
        event UntrackedBufferReturned() severity warning high format "SA router received an untracked buffer return"
    }
}
}
