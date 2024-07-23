module Svc {

  @ A component for deframing input received from the ground
  @ via a byte stream driver, which may be active or passive
  passive component DeframerFp {

    # ----------------------------------------------------------------------
    # Deframer interface
    # ----------------------------------------------------------------------

    include "../Interfaces/DeframerInterface.fppi"

    # ----------------------------------------------------------------------
    # Memory management for deframing
    # ----------------------------------------------------------------------

    @ Port for allocating Fw::Buffer objects from a buffer manager.
    @ When Deframer invokes this port, it receives a packet buffer PB and
    @ takes ownership of it. Ownership is then delegated to the Router
    output port bufferAllocate: Fw.BufferGet

    @ Port for deallocating buffers received on dataIn.
    output port bufferDeallocate: Fw.BufferSend
  }

}
