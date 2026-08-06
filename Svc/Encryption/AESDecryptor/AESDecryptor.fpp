module Svc {
module Ccsds{
    @ AES decryptor
    passive component AESDecryptor {
        import Svc.Ccsds.CcsdsSdlsDecrypt
        import Svc.Ccsds.SdlsKeyInterfaceClient

        @ Port for requesting the current time
        time get port timeCaller

    }
}
}