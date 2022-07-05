module Svc {

  @ A component for deframing input for transmission to the ground
  passive component Framer {

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Port for receiving data packets stored in statically-sized
    @ Com buffers
    guarded input port comIn: Fw.Com

    @ Port for receiving data packets stored in dynamically-sized
    @ managed bufers
    guarded input port bufferIn: Fw.BufferSend

    @ Port for deallocating buffers received on bufferIn, after
    @ copying packet data to the frame buffer
    output port bufferDeallocate: Fw.BufferSend

    @ Port for allocating buffers to hold framed data
    output port framedAllocate: Fw.BufferGet

    @ Port for sending buffers containing framed data. Ownership of the
    @ buffer passes to the receiver.
    output port framedOut: Drv.ByteStreamSend

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Time get port
    time get port timeGet

  }

}
