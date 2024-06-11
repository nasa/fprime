module Drv {
    passive component TcpServer {

        include "../Interfaces/ByteStreamDriverInterface.fppi"
        
        output port allocate: Fw.BufferGet

        output port deallocate: Fw.BufferSend

    }
}
