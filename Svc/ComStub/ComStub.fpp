module Svc {
    @ Communication adapter interface implementing communication adapter interface via a Drv.ByteStreamDriverModel.
    passive component ComStub {
        include "../Interfaces/ComInterface.fppi"

        # ----------------------------------------------------------------------
        # Byte stream model
        # ----------------------------------------------------------------------

        @ Ready signal when driver is connected
        sync input port drvConnected: Drv.ByteStreamReady

        @ Receive (read) data from driver. This gets forwarded to comDataOut
        sync input port drvDataIn: Drv.ByteStreamData

        @ Send (write) data to the driver. This gets invoked on comDataIn invocation
        output port drvDataOut: Fw.BufferSend

        @ Callback from drvDataOut (retrieving status and ownership of sent buffer)
        sync input port dataReturnIn: Drv.ByteStreamData

    }
}
