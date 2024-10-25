module Svc {
    @ Accumulates data into frames
    passive component FrameAccumulator {

        @ Receives raw data from a ByteStreamDriver, ComStub, or other buffer producing component
        guarded input port dataIn: Drv.ByteStreamRecv

        @ Port for deallocating buffers received on dataIn.
        output port dataDeallocate: Fw.BufferSend


        @ Port for allocating buffer to hold extracted frame
        output port frameAllocate: Fw.BufferGet

        @ Port for sending an extracted frame out
        output port frameOut: Fw.DataWithContext
    }
}