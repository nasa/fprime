module Svc {
    @ Communications Adapter Interface
    interface Com {
        @ Data to be sent on the wire (coming in to the component)
        sync input port dataIn: Svc.ComDataWithContext

        @ Data received from the wire (going out of the component)
        output port dataOut: Svc.ComDataWithContext

        @ Status of the last transmission
        output port comStatusOut: Fw.SuccessCondition

        # ----------------------------------------------------------------------
        # Memory management
        # ----------------------------------------------------------------------

        @ Port returning ownership of data that came in on dataIn
        output port dataReturnOut: Svc.ComDataWithContext

        @ Port receiving back ownership of buffer sent out on dataOut
        sync input port dataReturnIn: Svc.ComDataWithContext
    }
}
