module Svc {
    @ Interface for allocating and deallocating buffers
    interface BufferAllocation {
        @ Allocation request to buffer manager
        output port allocate: Fw.BufferGet

        @ Deallocation request to buffer manager
        output port deallocate: Fw.BufferSend
    }
}
