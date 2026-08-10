module Svc {
module Ccsds {
    @ WARNING: This component provides NO security. It performs no authentication and
    @ no encryption, passing buffers and contexts through unmodified. It is intended
    @ only for clear-mode operation and testing.
    passive component ClearTextEncryptor {

        import Svc.Ccsds.CcsdsSdlsEncrypt

    }
}
}
