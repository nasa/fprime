module Svc {
module Ccsds {

    @ Interface for components supplying SDLS keys: fills a caller-provided key
    @ buffer and returns the operation status
    interface SdlsKeyInterface {
        @ Port to receive a key request: fills the provided buffer with the key
        guarded input port keyGet: Svc.Ccsds.SdlsKey
        @ Port to send the key request: fills the provided buffer with the key
        guarded input port keySet: Fw.BufferSend
    }

    @ Client-side mirror of SdlsKeyInterface: requests an SDLS key by supplying
    @ a buffer to be filled
    interface SdlsKeyInterfaceClient {
        @ Port for requesting a key: the provided buffer is filled with the key
        output port keyGet: Svc.Ccsds.SdlsKey
        @ Port to receive a key: the provided buffer contains the key to persist 
        output port keySet: Fw.BufferSend
    }

}
}
