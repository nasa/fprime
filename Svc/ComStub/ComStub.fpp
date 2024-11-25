module Svc {
    @ Communication adapter interface implementing communication adapter interface via a Drv.ByteStreamDriverModel.
    passive component ComStub {
        include "../Interfaces/ComInterface.fppi"

        # ----------------------------------------------------------------------
        # Byte stream model
        # ----------------------------------------------------------------------

        @ Ready signal when driver is connected
        sync input port drvConnected: Drv.ByteStreamReady

        @ Data received from driver
        sync input port drvDataIn: Drv.ByteStreamRecv

        @ Data going to the underlying driver
        output port drvDataOut: Drv.ByteStreamSend
    }
}