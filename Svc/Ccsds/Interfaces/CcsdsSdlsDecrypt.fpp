module Svc {
module Ccsds {

    @ Interface for components performing CCSDS SDLS (Space Data Link Security) decryption:
    @ receives a security association index and iv/data buffer, and returns the iv/data buffer
    interface CcsdsSdlsDecrypt {
        @ Port to receive the security association index and iv/data buffer to decrypt
        guarded input port decryptIn: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for returning the iv/data buffer
        output port bufferReturnOut: Fw.BufferSend
    }

    @ Client-side mirror of CcsdsSdlsDecrypt: sends a security association index and
    @ iv/data buffer for decryption, and receives the returned iv/data buffer
    interface CcsdsSdlsDecryptClient {
        @ Port for sending the security association index and iv/data buffer to decrypt
        output port decryptOut: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for receiving the returned iv/data buffer
        sync input port bufferReturnIn: Fw.BufferSend
    }

}
}
