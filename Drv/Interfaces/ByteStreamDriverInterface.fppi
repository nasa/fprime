    @ Port invoked when the driver is ready to send/receive data
    output port ready: Drv.ByteStreamReady

    @ Port invoked by the driver when it receives data
    output port $recv: Drv.ByteStreamData

    @ Invoke this port to send data out the driver
    guarded input port $send: Fw.BufferSend

    @ Port invoked to return ownership of sent data back to the sender
    output port dataReturnOut: Drv.ByteStreamData
