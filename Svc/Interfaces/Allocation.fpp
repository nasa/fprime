module Svc {
    interface Allocation {
        @ Allocation request to buffer manager
        output port allocate: Fw.BufferGet

        @ Deallocation request to buffer manager
        output port deallocate: Fw.BufferSend
    }
}