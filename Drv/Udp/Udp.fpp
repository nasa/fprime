module Drv {
    passive component Udp {

        include "../Interfaces/ByteStreamDriverInterface.fppi"
        
        output port allocate: Fw.BufferGet

        output port deallocate: Fw.BufferSend

    }
}
