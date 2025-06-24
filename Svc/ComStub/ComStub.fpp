module Svc {
    @ Communication adapter interface implementing communication adapter interface via a Drv.ByteStreamDriverModel.
    passive component ComStub {
        import Com

        # ----------------------------------------------------------------------
        # Byte stream model
        # ----------------------------------------------------------------------

        @ Ready signal when driver is connected
        sync input port drvConnected: Drv.ByteStreamReady

        @ Receive (read) data from driver. This gets forwarded to dataOut
        sync input port drvReceiveIn: Drv.ByteStreamData

        @ Send (write) data to the driver. This gets invoked on dataIn invocation
        output port drvSendOut: Fw.BufferSend

        @ Callback from drvSendOut (retrieving status and ownership of sent buffer)
        sync input port drvSendReturnIn: Drv.ByteStreamData

        @ Returning ownership of buffer that came in on drvReceiveIn
        output port drvReceiveReturnOut: Fw.BufferSend

    }
}
