@ Temporary interfaces to work around an FPP bug
@ See nasa/fpp#829
@ The bug causes the FPP analyzer to be sensitive to the order in which
@ interfaces are presented for resolution.
@ When this bug is fixed, we can import the interfaces located at Fw/Buffer/Buffer.fpp.
module Drv {

  @ Interface for allocating and deallocating buffers
  interface BufferAllocation {

    @ Allocation request to buffer manager
    output port allocate: Fw.BufferGet

    @ Deallocation request to buffer manager
    output port deallocate: Fw.BufferSend

  }

  @ The bufferIn interface for a passive component
  interface PassiveBufferIn {

    @ Port receiving buffers
    sync input port bufferIn: Fw.BufferSend

    @ Port for returning buffers received on bufferIn
    output port bufferInReturn: Fw.BufferSend

  }

  @ The bufferOut interface for a passive component
  interface PassiveBufferOut {

    @ Port for sending buffers
    output port bufferOut: Fw.BufferSend

    @ Port for receiving buffers sent on bufferOut and then returned
    sync input port bufferOutReturn: Fw.BufferSend

  }

}
