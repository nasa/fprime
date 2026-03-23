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


    # ----------------------------------------------------------------------
    # Client interface
    # Use this interface to connect to a ByteStreamDriver
    # ----------------------------------------------------------------------

    @ Client of the synchronous ByteStreamDriver interface
    interface ByteStreamDriverClient {
        @ Ready signal when driver is connected
        sync input port drvConnected: Drv.ByteStreamReady

        @ Receive (read) data from driver.
        sync input port drvReceiveIn: Drv.ByteStreamData

        @ Returning ownership of buffer that came in on drvReceiveIn
        output port drvReceiveReturnOut: Fw.BufferSend

        @ Send (write) data to the driver
        output port drvSendOut: Drv.ByteStreamSend
    }
}
