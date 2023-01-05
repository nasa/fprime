module Svc {
    @ Communication adapter interface implementing communication adapter interface via a Drv.ByteStreamDriverModel.
    passive component ComStub {

        # ----------------------------------------------------------------------
        # Framer, deframer, and queue ports
        # ----------------------------------------------------------------------

        @ Data coming in from the framing component
        sync input port comDataIn: Drv.ByteStreamSend

        @ Status of the last radio transmission
        output port comStatus: Fw.SuccessCondition

        @ Com data passing back out
        output port comDataOut: Drv.ByteStreamRecv

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