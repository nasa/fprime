module Drv {
    passive component TcpClient {

        include "../Interfaces/ByteStreamDriverInterface.fppi"

        @ Allocation for received data
        output port allocate: Fw.BufferGet

        @ Deallocation of allocated buffers
        output port deallocate: Fw.BufferSend

    }
}
