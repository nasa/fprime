module Drv {
    passive component TcpServer {

        import ByteStreamDriver

        @ Allocation for received data
        output port allocate: Fw.BufferGet

        @ Deallocation of allocated buffers
        output port deallocate: Fw.BufferSend

    }
}
