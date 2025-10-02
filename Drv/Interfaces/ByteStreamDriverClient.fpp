module Drv {
    @ A client of a synchronous byte stream driver
    interface ByteStreamDriverClient {

        # ----------------------------------------------------------------------
        # Ready interface 
        # ----------------------------------------------------------------------
        @ Port for receiving ready signals from the driver
        @ Sample connection: byteStreamDriver.ready -> byteStreamDriverClient.byteStreamReady
        sync input port byteStreamReady: Drv.ByteStreamReady

        # ---------------------------------------------------------------------- 
        # Receive interface
        # ---------------------------------------------------------------------- 
        @ Port for receiving data from the driver
        @ Sample connection: byteStreamDriver.$recv -> byteStreamDriverClient.byteStreamIn
        sync input port byteStreamIn: Drv.ByteStreamData

        @ Port for returning ownership of buffers received on the $recv port
        @ Sample connection: byteStreamDriverClient.byteStreamReturn -> byteStreamDriver.recvReturnIn
        output port byteStreamInReturn: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Send interface 
        # ----------------------------------------------------------------------
        @ Port for sending data to the driver
        @ Sample connection: byteStreamDriverCient.byteStreamOut -> byteStreamDriver.$send
        output port byteStreamOut: Drv.ByteStreamSend
    }
}
