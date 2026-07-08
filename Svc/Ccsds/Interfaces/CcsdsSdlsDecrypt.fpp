module Svc {
module Ccsds {

    @ Interface for components performing CCSDS SDLS (Space Data Link Security) decryption:
    @ receives a security association index and iv/data buffer, returning an SdlsStatus.
    @ Decrypted data (possibly newly allocated) is sent out decryptOut and its ownership
    @ is returned via decryptReturnIn; incoming buffers are returned via bufferReturnOut.
    interface CcsdsSdlsDecrypt {
        @ Port to receive the security association index and iv/data buffer to decrypt
        guarded input port decryptIn: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for sending decrypted data (possibly newly allocated) downstream
        output port decryptOut: Fw.BufferSend

        @ Port for receiving back ownership of buffers sent on decryptOut
        sync input port decryptReturnIn: Fw.BufferSend

        @ Port for returning the incoming iv/data buffer for deallocation
        output port bufferReturnOut: Fw.BufferSend
    }

    @ Client-side mirror of CcsdsSdlsDecrypt: sends a security association index and
    @ iv/data buffer for decryption, receives decrypted data, and returns its ownership
    interface CcsdsSdlsDecryptClient {
        @ Port for sending the security association index and iv/data buffer to decrypt
        output port decryptOut: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for receiving decrypted data (possibly newly allocated)
        sync input port decryptIn: Fw.BufferSend

        @ Port for returning ownership of buffers received on decryptIn
        output port decryptReturnOut: Fw.BufferSend

        @ Port for receiving back the iv/data buffer sent on decryptOut for deallocation
        sync input port bufferReturnIn: Fw.BufferSend
    }

}
}
