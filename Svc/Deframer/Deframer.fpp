module Svc {

  @ A component for deframing input received from the ground
  @ via a byte stream driver, which may be active or passive
  passive component Deframer {

    # ----------------------------------------------------------------------
    # Receiving framed data via push
    # ----------------------------------------------------------------------

    @ Port for receiving frame buffers FB pushed from the byte stream driver.
    @ After using a buffer FB received on this port, Deframer deallocates it
    @ by invoking framedDeallocate.
    guarded input port framedIn: Drv.ByteStreamRecv

    @ Port for deallocating buffers received on framedIn.
    output port framedDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Receiving framed data via poll
    # ----------------------------------------------------------------------

    @ Schedule in port, driven by a rate group.
    guarded input port schedIn: Svc.Sched

    @ Port that polls for data from the byte stream driver.
    @ Deframer invokes this port on its schedIn cycle, if it is connected.
    @ No allocation or occurs when invoking this port.
    @ The data transfer uses a pre-allocated frame buffer
    @ owned by Deframer.
    output port framedPoll: Drv.ByteStreamPoll

    # ----------------------------------------------------------------------
    # Memory management for deframing
    # ----------------------------------------------------------------------

    @ Port for allocating Fw::Buffer objects from a buffer manager.
    @ When Deframer invokes this port, it receives a packet buffer PB and
    @ takes ownership of it. Ownership is then delegated to the Router
    output port bufferAllocate: Fw.BufferGet

    # ----------------------------------------------------------------------
    # Sending packets to Router or subsequent Deframer
    # ----------------------------------------------------------------------

    @ Port for sending deframed buffers out to a router, or another deframer
    @ for chained deframing
    output port bufferOut: Fw.BufferSend

  }

}
