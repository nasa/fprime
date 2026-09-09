module Svc {
module Ccsds {

    @ Interface for components supplying SDLS keys: fills a caller-provided key
    @ buffer and returns the operation status. The request carries the security
    @ association index whose key is wanted; implementations that key per SA shall
    @ document which indices they serve and return KEY_ERROR for any other, while
    @ implementations that ignore the index shall say so, since sharing one key
    @ across SAs also shares its IV budget
    interface SdlsKeyInterface {
        @ Port to receive a key request: fills the provided buffer with the key for
        @ the requested security association index
        guarded input port keyGet: Svc.Ccsds.SdlsKey
    }

    @ Client-side mirror of SdlsKeyInterface: requests an SDLS key for a security
    @ association index by supplying a buffer to be filled
    interface SdlsKeyInterfaceClient {
        @ Port for requesting a key: the provided buffer is filled with the key bound
        @ to the requested security association index
        output port keyGet: Svc.Ccsds.SdlsKey
    }

}
}
