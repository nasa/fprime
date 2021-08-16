module Svc {

  @ A component for deframing framed input
  passive component Deframer {

    @ Com out port
    output port comOut: Fw.Com

    @ Buffer send port
    output port bufferOut: Fw.BufferSend

    @ Buffer allocate port
    output port bufferAllocate: Fw.BufferGet

    @ Buffer deallocate port
    output port bufferDeallocate: Fw.BufferSend

    @ Mutex framed input port
    guarded input port framedIn: Drv.ByteStreamRecv

    @ Framed deallocate port
    output port framedDeallocate: Fw.BufferSend

    @ Framed poll port
    output port framedPoll: Drv.ByteStreamPoll

    @ Mutexed Schedule in port
    guarded input port schedIn: Svc.Sched

    @ Port for receiving command responses
    sync input port cmdResponseIn: Fw.CmdResponse

  }

}
