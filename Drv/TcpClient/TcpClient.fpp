module Drv {
    passive component TcpClient {

        import ByteStreamDriver

        @ Allocation for received data
        output port allocate: Fw.BufferGet

        @ Deallocation of allocated buffers
        output port deallocate: Fw.BufferSend

    }
}
