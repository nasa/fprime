module Svc {
    interface Deframer {
        @ Port to receive framed data, with optional context
        guarded input port dataIn: Svc.ComDataWithContext

        @ Port to output deframed data, with optional context
        output port dataOut: Svc.ComDataWithContext

        @ Port for returning ownership of received buffers to deframe
        output port dataReturnOut: Svc.ComDataWithContext

        @ Port receiving back ownership of sent buffers
        sync input port dataReturnIn: Svc.ComDataWithContext
    }
}
