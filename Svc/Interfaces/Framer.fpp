module Svc {
    @ ----------------------------------------------------------------------
    @ Framing
    @ ----------------------------------------------------------------------
    interface Framer {
        @ Port to receive data to frame, in a Fw::Buffer with optional context
        sync input port dataIn: Svc.ComDataWithContext

        @ Port to output framed data with optional context
        output port dataOut: Svc.ComDataWithContext

        # ----------------------------------------------------------------------
        # Data ownership
        # ----------------------------------------------------------------------
        @ Port for returning ownership of the incoming Fw::Buffer to its sender
        @ once framing is handled
        output port dataReturnOut: Svc.ComDataWithContext

        @ Buffer coming from a deallocate call in a ComDriver component
        sync input port dataReturnIn: Svc.ComDataWithContext

        # ----------------------------------------------------------------------
        # Handling of ready signals (ComQueue <-> ComInterface)
        # ----------------------------------------------------------------------
        @ Port receiving the general status from the downstream component
        @ indicating it is ready or not-ready for more input
        sync input port comStatusIn: Fw.SuccessCondition

        @ Port receiving indicating the status of framer for receiving more data
        output port comStatusOut: Fw.SuccessCondition
    }
}
