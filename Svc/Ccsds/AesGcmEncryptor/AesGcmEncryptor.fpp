module Svc {
module Ccsds{
    @ AES encryptor
    passive component AesGcmEncryptor {
        import Svc.Ccsds.CcsdsSdlsEncrypt
        import Svc.Ccsds.SdlsKeyInterfaceClient

        ###############################################################################
        # Standard AC Ports: Required for Events                                      #
        ###############################################################################

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

        @ A frame arrived while the previous one was still in flight downstream. The output
        @ buffer is a single per-instance store, so the new frame is dropped rather than
        @ written over ciphertext that has not been sent yet.
        event OutputBufferBusy() \
            severity warning high \
            format "Output buffer still in flight; frame dropped"
    }
}
}