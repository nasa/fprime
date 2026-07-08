module Svc {
module Ccsds {
    @ Routes SDLS decryption requests to downstream decryptor components by mapping
    @ the incoming security association (SA) index to a downstream port index using
    @ a compile-time SA-to-port map (SdlsCfg.SaMap)
    passive component SdlsSaRouter {

        # ----------------------------------------------------------------------
        # Upstream side: implements the CcsdsSdlsDecrypt interface
        # ----------------------------------------------------------------------
        import Svc.Ccsds.CcsdsSdlsDecrypt

        # ----------------------------------------------------------------------
        # Downstream side: inlined arrays of CcsdsSdlsDecryptClient ports
        # (FPP interfaces are not array-able at this time)
        # ----------------------------------------------------------------------
        @ Ports for sending the SA index and iv/data buffer to the mapped downstream decryptor
        output port saDecryptOut: [SdlsCfg.SaRouterPortCount] Svc.Ccsds.CcsdsSdlsEncryption

        @ Ports for receiving decrypted data (possibly newly allocated) from downstream decryptors
        sync input port saDecryptIn: [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext

        @ Ports for returning ownership of decrypted data buffers to downstream decryptors
        output port saDecryptReturnOut: [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext

        @ Ports for receiving back iv/data buffers from downstream decryptors for deallocation
        sync input port saBufferReturnIn: [SdlsCfg.SaRouterPortCount] Svc.ComDataWithContext
    }
}
}
