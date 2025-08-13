module Svc {
    @ Communication adapter interface implementing communication adapter interface via a Drv.ByteStreamDriver
    @ ComStub can use both synchronous and asynchronous byte stream drivers, users should connect the appropriate
    @ based on their driver type
    passive component ComStub {
        import Com

        # ----------------------------------------------------------------------
        # Byte stream model (common)
        # ----------------------------------------------------------------------

        @ Ready signal when driver is connected
        sync input port drvConnected: Drv.ByteStreamReady

        @ Receive (read) data from driver. This gets forwarded to dataOut
        sync input port drvReceiveIn: Drv.ByteStreamData

        @ Returning ownership of buffer that came in on drvReceiveIn
        output port drvReceiveReturnOut: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Byte stream model (synchronous)
        # ----------------------------------------------------------------------

        @ Send (write) data to the driver. This gets invoked on dataIn invocation
        output port drvSendOut: Drv.ByteStreamSend

        # ----------------------------------------------------------------------
        # Byte stream model (asynchronous)
        # ----------------------------------------------------------------------
    
        @ Send (write) data to the driver asynchronously
        output port drvAsyncSendOut: Fw.BufferSend

        @ Callback from drvAsyncSendOut (retrieving status and ownership of sent buffer)
        sync input port drvAsyncSendReturnIn: Drv.ByteStreamData

    }
}
