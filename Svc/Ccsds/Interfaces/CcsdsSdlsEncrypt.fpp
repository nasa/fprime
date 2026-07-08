module Svc {
module Ccsds {

    @ Interface for components performing CCSDS SDLS (Space Data Link Security) encryption:
    @ receives a security association index and iv/data buffer, returning an SdlsStatus.
    @ Encrypted data (possibly newly allocated) is sent out encryptOut and its ownership
    @ is returned via encryptReturnIn; incoming buffers are returned via bufferReturnOut.
    interface CcsdsSdlsEncrypt {
        @ Port to receive the security association index and iv/data buffer to encrypt
        guarded input port encryptIn: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for sending encrypted data (possibly newly allocated) downstream
        output port encryptOut: Fw.BufferSend

        @ Port for receiving back ownership of buffers sent on encryptOut
        sync input port encryptReturnIn: Fw.BufferSend

        @ Port for returning the incoming iv/data buffer for deallocation
        output port bufferReturnOut: Fw.BufferSend
    }

    @ Client-side mirror of CcsdsSdlsEncrypt: sends a security association index and
    @ iv/data buffer for encryption, receives encrypted data, and returns its ownership
    interface CcsdsSdlsEncryptClient {
        @ Port for sending the security association index and iv/data buffer to encrypt
        output port encryptOut: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for receiving encrypted data (possibly newly allocated)
        sync input port encryptIn: Fw.BufferSend

        @ Port for returning ownership of buffers received on encryptIn
        output port encryptReturnOut: Fw.BufferSend

        @ Port for receiving back the iv/data buffer sent on encryptOut for deallocation
        sync input port bufferReturnIn: Fw.BufferSend
    }

}
}
