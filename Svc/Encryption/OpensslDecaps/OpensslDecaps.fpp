module Svc {
module Ccsds{
    @ Decapsulation of KEM key
    passive component OpensslDecaps {
        import Svc.Ccsds.SdlsKeyInterfaceClient

        @ port for taking in kem message
        sync input port kemMsgIn: Fw.BufferSend
        @ Port for forwarding non-recognized packet types
        output port unknownDataOut: Svc.ComDataWithContext
        @ Port for sending derived session key to key management
        output port derivedKeyOut: Fw.BufferSend
        @ Port for requesting the current time
        time get port timeCaller

    }
}
}