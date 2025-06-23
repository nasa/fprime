module Svc {
    interface Router {
        # ---------------------------------------------
        # Router <-> Deframers
        # ---------------------------------------------

        @ Receiving data (Fw::Buffer) to be routed with optional context to help with routing
        sync input port dataIn: Svc.ComDataWithContext

        @ Port for returning ownership of data (includes Fw.Buffer) received on dataIn
        output port dataReturnOut: Svc.ComDataWithContext

        # ---------------------------------------------
        # Router <-> CmdDispatch/FileUplink
        # ---------------------------------------------

        @ Port for sending file packets as Fw::Buffer (ownership passed to receiver)
        output port fileOut: Fw.BufferSend

        @ Port for receiving ownership back of buffers sent on fileOut
        sync input port fileBufferReturnIn: Fw.BufferSend

        @ Port for sending command packets as Fw::ComBuffers
        output port commandOut: Fw.Com

        @ Port for receiving command responses from a command dispatcher (can be a no-op)
        sync input port cmdResponseIn: Fw.CmdResponse
    }
}
