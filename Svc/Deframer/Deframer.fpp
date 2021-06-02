module Svc {

  passive component Deframer {

    output port comOut: [1] Fw.Com

    output port bufferOut: [1] Fw.BufferSend

    output port bufferAllocate: [1] Fw.BufferGet

    output port bufferDeallocate: [1] Fw.BufferSend

    guarded input port framedIn: [1] Drv.ByteStreamRecv

    output port framedDeallocate: [1] Fw.BufferSend

    output port framedPoll: [1] Drv.ByteStreamPoll

    guarded input port schedIn: [1] Svc.Sched

  }

}
