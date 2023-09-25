    @ Port invoked when the driver is ready to send/receive data
    output port ready: Drv.ByteStreamReady

    @ Port invoked when driver has received data
    output port $recv: Drv.ByteStreamRecv

    @ Port invoked to send data out the driver
    guarded input port send: Drv.ByteStreamSend
