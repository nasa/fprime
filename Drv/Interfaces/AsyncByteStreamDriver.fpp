module Drv {
    # In the asynchronous ByteStreamDriver interface, the send operation is non-blocking,
    # and returns status through the sendReturnOut callback

    @ Asynchronous ByteStreamDriver interface
    interface AsyncByteStreamDriver {
        @ Port invoked when the driver is ready to send/receive data
        output port ready: Drv.ByteStreamReady

        @ Port invoked by the driver when it receives data
        output port $recv: Drv.ByteStreamData

        @ Invoke this port to send data out the driver (asynchronous)
        @ Status and ownership of the buffer are returned through the sendReturnOut callback
        async input port $send: Fw.BufferSend

        @ Port returning ownership of data received on $send port
        output port sendReturnOut: Drv.ByteStreamData

        @ Port receiving back ownership of data sent out on $recv port
        guarded input port recvReturnIn: Fw.BufferSend
    }
}
