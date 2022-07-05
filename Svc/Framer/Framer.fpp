module Svc {

  @ A component for deframing input for transmission to the ground
  passive component Framer {

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Mutexed input communication port
    guarded input port comIn: Fw.Com

    @ Mutexed input Buffer send port
    guarded input port bufferIn: Fw.BufferSend

    @ Buffer send output port
    output port bufferDeallocate: Fw.BufferSend

    @ Framed allocate output port
    output port framedAllocate: Fw.BufferGet

    @ Framed output port
    output port framedOut: Drv.ByteStreamSend

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Time get port
    time get port timeGet

  }

}
