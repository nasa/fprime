module Svc {
    passive component FprimeRouter {
        @ Receives deframed data and context
        sync input port deframedIn: Fw.BufferContextSend

        @ Port for sending file data to the file uplink component
        @ Ownership of the Fw::Buffer passes to the receiver, which is
        @ responsible for the deallocation.
        output port fileBufferOut: Fw.BufferSend


        @ Port for deallocating temporary buffers allocated with
        @ bufferAllocate when the buffer is converted to a Fw.Com. Deallocation
        @ occurs here when there is nothing to send on fileBufferOut.
        output port bufferDeallocate: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Sending command packets and receiving command responses
        # ----------------------------------------------------------------------

        @ Port for sending command packets as Com buffers.
        output port comOut: Fw.Com

        @ Port for receiving command responses from a command dispatcher.
        @ Invoking this port does nothing. The port exists to allow the matching
        @ connection in the topology.
        sync input port cmdResponseIn: Fw.CmdResponse
    }
}