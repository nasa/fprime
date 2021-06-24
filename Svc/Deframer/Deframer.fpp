module Svc {

  @ A component for deframing framed input
  passive component Deframer {

    @ Com out port
    output port comOut: [1] Fw.Com

    @ Buffer send port
    output port bufferOut: [1] Fw.BufferSend

    @ Buffer allocate port
    output port bufferAllocate: [1] Fw.BufferGet

    @ Buffer deallocate port
    output port bufferDeallocate: [1] Fw.BufferSend

    @ Mutex framed input port
    guarded input port framedIn: [1] Drv.ByteStreamRecv

    @ Framed deallocate port
    output port framedDeallocate: [1] Fw.BufferSend

    @ Framed poll port
    output port framedPoll: [1] Drv.ByteStreamPoll

    @ Mutexed Schedule in port
    guarded input port schedIn: [1] Svc.Sched

  }

}
