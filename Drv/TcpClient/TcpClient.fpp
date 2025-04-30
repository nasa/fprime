module Drv {
    passive component TcpClient {

        include "../Interfaces/ByteStreamDriverInterface.fppi"

        @ Allocation for received data
        output port allocate: Fw.BufferGet

    }
}
