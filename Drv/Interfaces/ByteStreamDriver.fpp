module Drv {
    # In the synchronous ByteStreamDriver interface, the send operation is blocking
    # and returns a send status

    @ Synchronous ByteStreamDriver interface
    interface ByteStreamDriver {
        @ Port invoked when the driver is ready to send/receive data
        output port ready: Drv.ByteStreamReady

        @ Port invoked by the driver when it receives data
        output port $recv: Drv.ByteStreamData

        @ Invoke this port to send data out the driver (synchronous)
        @ Status is returned, and ownership of the buffer is retained by the caller
        guarded input port $send: Drv.ByteStreamSend

        @ Port receiving back ownership of data sent out on $recv port
        guarded input port recvReturnIn: Fw.BufferSend
    }
}
