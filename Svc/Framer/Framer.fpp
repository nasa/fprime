module Svc {

  passive component Framer {

    @ Mutexed input communication port
    guarded input port comIn: [1] Fw.Com

    @ Mutexed input Buffer send port
    guarded input port bufferIn: [1] Fw.BufferSend

    @ Buffer send output port
    output port bufferDeallocate: [1] Fw.BufferSend

    @ Framed allocate output port
    output port framedAllocate: [1] Fw.BufferGet

    @ Framed output port
    output port framedOut: [1] Drv.ByteStreamSend

    @ Time get port
    time get port timeGet

  }

}
