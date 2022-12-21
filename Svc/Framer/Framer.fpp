module Svc {

  @ A component for framing input for transmission to the ground
  passive component Framer {

    # ----------------------------------------------------------------------
    # Receiving packets
    # ----------------------------------------------------------------------

    @ Port for receiving data packets of any type stored in statically-sized
    @ Fw::Com buffers
    guarded input port comIn: Fw.Com

    @ Port for receiving file packets stored in dynamically-sized
    @ Fw::Buffer objects
    guarded input port bufferIn: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Allocation and deallocation of buffers
    # ----------------------------------------------------------------------

    @ Port for deallocating buffers received on bufferIn, after
    @ copying packet data to the frame buffer
    output port bufferDeallocate: Fw.BufferSend

    @ Port for allocating buffers to hold framed data
    output port framedAllocate: Fw.BufferGet

    # ----------------------------------------------------------------------
    # Sending frame data
    # ----------------------------------------------------------------------

    @ Port for sending buffers containing framed data. Ownership of the
    @ buffer passes to the receiver.
    output port framedOut: Drv.ByteStreamSend

    # ----------------------------------------------------------------------
    # Handling of of ready signals
    # ----------------------------------------------------------------------

    @ Port receiving the general status from the downstream component
    @ indicating it is ready or not-ready for more input
    guarded input port comStatusIn: Fw.SuccessCondition

    @ Port receiving indicating the status of framer for receiving more data
    output port comStatusOut: Fw.SuccessCondition

  }

}
