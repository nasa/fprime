module Drv {
    passive component TcpServer {

        include "../Interfaces/ByteStreamDriverInterface.fppi"

        @ Allocation for received data
        output port allocate: Fw.BufferGet

    }
}
