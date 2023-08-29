module Drv {
    passive component TcpClient {

        include "../Interfaces/ByteStreamDriverInterface.fppi"
        
        output port allocate: Fw.BufferGet

        output port deallocate: Fw.BufferSend

    }
}
