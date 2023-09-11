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

    @ Port for receiving file packets stored in dynamically-sized
    @ Fw::Buffer objects along with Fw::Buffer storing contextual metadata
    guarded input port bufferAndContextIn: Fw.BufferContextSend

    # ----------------------------------------------------------------------
    # Allocation and deallocation of buffers
    # ----------------------------------------------------------------------

    @ Port for deallocating buffers received on bufferIn, and bufferAndContextIn after
    @ copying packet data to the frame buffer
    output port bufferDeallocate: Fw.BufferSend

    @ Port for deallocating contextual buffers received on bufferAndContextIn, after
    @ passing data to the frame buffer
    output port contextDeallocate: Fw.BufferSend

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
    sync input port comStatusIn: Fw.SuccessCondition

    @ Port receiving indicating the status of framer for receiving more data
    output port comStatusOut: Fw.SuccessCondition

  }

}
