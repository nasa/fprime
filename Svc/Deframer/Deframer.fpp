module Svc {

  @ A component for deframing input received from the ground
  passive component Deframer {

    # ----------------------------------------------------------------------
    # Input ports
    # ----------------------------------------------------------------------

    @ Port for receiving command responses from the command dispatcher
    sync input port cmdResponseIn: Fw.CmdResponse

    @ Port for receiving data pushed from the byte stream driver.
    @ TODO: Why is this port guarded? By assumption framedIn and framedPoll
    @ should never both be connected.
    guarded input port framedIn: Drv.ByteStreamRecv

    @ Schedule in port.
    @ TODO: Why is this port guarded? By assumption framedIn and framedPoll
    @ should never both be connected.
    guarded input port schedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Output ports
    # ----------------------------------------------------------------------

    @ Port for allocating Fw::Buffer objects, e.g.,
    @ for file uplink
    output port bufferAllocate: Fw.BufferGet

    @ Port for deallocating buffers allocated with bufferAllocate
    output port bufferDeallocate: Fw.BufferSend

    @ Port for deallocating received buffers containing framed data
    output port framedDeallocate: Fw.BufferSend

    @ Port for sending buffers allocated with bufferAllocate.
    @ The receiver is responsible for the deallocation.
    output port bufferOut: Fw.BufferSend

    @ Port for sending Com packets, e.g., command packets
    output port comOut: Fw.Com

    @ Port that polls for data from the byte stream driver
    output port framedPoll: Drv.ByteStreamPoll

  }

}
