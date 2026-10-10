module Svc {
    @ A component for retrying message delivery on failure
    active component ComRetry {
        @ Port to receive data to send, in a Fw::Buffer with optional context
        async input port dataIn: Svc.ComDataWithContext

        @ Port to output data to the communication adapter, with optional context
        output port dataOut: Svc.ComDataWithContext

        @ Port for returning ownership of the incoming Fw::Buffer to its sender
        output port dataReturnOut: Svc.ComDataWithContext

        @ Port receiving ownership of the Fw::Buffer sent on dataOut
        async input port dataReturnIn: Svc.ComDataWithContext

        @ Port receiving the status of the downstream communication adapter
        async input port comStatusIn: Fw.SuccessCondition

        @ Port forwarding delivery status upstream
        output port comStatusOut: Fw.SuccessCondition

        @ Ping input port for health checking
        async input port pingIn: Svc.Ping drop

        @ Ping output port for health checking
        output port pingOut: Svc.Ping
    }
}
