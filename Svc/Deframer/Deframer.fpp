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
    # Memory management for deframing and for sending file packets
    # ----------------------------------------------------------------------

    @ Port for allocating Fw::Buffer objects from a buffer manager.
    @ When Deframer invokes this port, it receives a packet buffer PB and
    @ takes ownership of it. It uses PB internally for deframing.
    output port bufferAllocate: Fw.BufferGet

    @ Port for sending deframed data and context pairs
    @ Ownership of the Fw::Buffer passes to the receiver, which is
    @ responsible for the deallocation.
    output port deframedOut: Fw.BufferContextSend
  }

}
