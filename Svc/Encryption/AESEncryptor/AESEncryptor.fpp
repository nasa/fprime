module Svc {
module Ccsds{
    @ AES encryptor
    passive component AESEncryptor {
        import Svc.Ccsds.CcsdsSdlsEncrypt
        import Svc.Ccsds.SdlsKeyInterfaceClient

        @ Port for requesting the current time
        time get port timeCaller

    }
}
}