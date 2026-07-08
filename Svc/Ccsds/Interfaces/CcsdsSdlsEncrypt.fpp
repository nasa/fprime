module Svc {
module Ccsds {

    @ Interface for components performing CCSDS SDLS (Space Data Link Security) encryption:
    @ receives a security association index and iv/data buffer, and returns the iv/data buffer
    interface CcsdsSdlsEncrypt {
        @ Port to receive the security association index and iv/data buffer to encrypt
        guarded input port encryptIn: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for returning the iv/data buffer
        output port bufferReturnOut: Fw.BufferSend
    }

    @ Client-side mirror of CcsdsSdlsEncrypt: sends a security association index and
    @ iv/data buffer for encryption, and receives the returned iv/data buffer
    interface CcsdsSdlsEncryptClient {
        @ Port for sending the security association index and iv/data buffer to encrypt
        output port encryptOut: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for receiving the returned iv/data buffer
        sync input port bufferReturnIn: Fw.BufferSend
    }

}
}
