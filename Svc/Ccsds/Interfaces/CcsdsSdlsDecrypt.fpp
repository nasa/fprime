module Svc {
module Ccsds {

    @ Interface for components performing CCSDS SDLS (Space Data Link Security) decryption:
    @ receives a security association index and iv/data buffer with frame context.
    @ Decrypted data (possibly newly allocated) is sent out decryptOut alongside the
    @ operation status, and its ownership
    @ is returned via decryptReturnIn; incoming buffers are returned via bufferReturnOut.
    interface CcsdsSdlsDecrypt {
        @ Port to receive the security association index and iv/data buffer to decrypt
        guarded input port decryptIn: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for sending the operation status and decrypted data (possibly newly allocated) downstream
        output port decryptOut: Svc.Ccsds.CcsdsSdlsData

        @ Port for receiving back ownership of buffers sent on decryptOut
        sync input port decryptReturnIn: Svc.ComDataWithContext

        @ Port for returning the incoming iv/data buffer for deallocation
        output port bufferReturnOut: Svc.ComDataWithContext
    }

    @ Client-side mirror of CcsdsSdlsDecrypt: sends a security association index and
    @ iv/data buffer for decryption, receives decrypted data, and returns its ownership
    interface CcsdsSdlsDecryptClient {
        @ Port for sending the security association index and iv/data buffer to decrypt
        output port decryptOut: Svc.Ccsds.CcsdsSdlsEncryption

        @ Port for receiving the operation status and decrypted data (possibly newly allocated)
        sync input port decryptIn: Svc.Ccsds.CcsdsSdlsData

        @ Port for returning ownership of buffers received on decryptIn
        output port decryptReturnOut: Svc.ComDataWithContext

        @ Port for receiving back the iv/data buffer sent on decryptOut for deallocation
        sync input port bufferReturnIn: Svc.ComDataWithContext
    }

}
}
