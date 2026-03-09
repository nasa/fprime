module Svc {
    @ Communication adapter interface implementing communication adapter interface via a Drv.ByteStreamDriver
    @ ComStub can use both synchronous and asynchronous byte stream drivers, users should connect the appropriate
    @ based on their driver type
    passive component ComStub {
        import Svc.Com

        # ----------------------------------------------------------------------
        # Ports to connect to a ByteStreamDriver (synchronous)
        # ----------------------------------------------------------------------

        import Drv.ByteStreamDriverClient

        # ----------------------------------------------------------------------
        # Additional ports to connect to an AsyncByteStreamDriver (asynchronous)
        # ----------------------------------------------------------------------
    
        @ Send (write) data to the driver asynchronously
        output port drvAsyncSendOut: Fw.BufferSend

        @ Callback from drvAsyncSendOut (retrieving status and ownership of sent buffer)
        sync input port drvAsyncSendReturnIn: Drv.ByteStreamData

    }
}
