module Svc {
module Ccsds {
    @ WARNING: This component provides NO security. It performs no authentication and
    @ no encryption, passing buffers and contexts through unmodified. It is intended
    @ only for clear-mode operation and testing.
    passive component ClearTextEncryptor {

        import Svc.Ccsds.CcsdsSdlsEncrypt

        @ Raised for every frame handled by this null cipher so that its presence in a
        @ configuration is visible in telemetry. A deployment requiring security must never see it.
        event NullCipherInUse(securityAssociationIndex: U16) severity warning high \
            format "ClearTextEncryptor handled a frame on SA {} with NO authentication or encryption" \
            throttle 5

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

    }
}
}
