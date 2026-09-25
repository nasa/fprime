module Svc {
module Ccsds{
    @ AES decryptor
    passive component AesGcmDecryptor {
        import Svc.Ccsds.CcsdsSdlsDecrypt
        import Svc.Ccsds.SdlsKeyInterfaceClient

        ###############################################################################
        # Standard AC Ports: Required for Events                                      #
        ###############################################################################

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

        @ An authenticated frame reused an IV, or carried one outside the anti-replay window, and was dropped
        event IvReplayed(
            securityAssociationIndex: U16 @< Security association the frame arrived on
            received: SdlsIv @< IV carried by the rejected frame
            lastAccepted: SdlsIv @< IV of the last frame accepted on this decryptor
        ) severity warning high \
            format "IV reused on SA {}: received IV {} is not within the anti-replay window after {}" \
            throttle 5
    }
}
}
