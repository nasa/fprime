module Drv {
    passive component Udp {

        import ByteStreamDriver
        
        output port allocate: Fw.BufferGet

        output port deallocate: Fw.BufferSend

    }
}
