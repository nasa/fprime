module Fw {

  @ The buffer type
  type Buffer

  @ Port for sending a buffer
  port BufferSend(
                   @ The buffer
                   ref fwBuffer: Fw.Buffer
                 )

  @ Port for getting a buffer
  @ Returns the buffer
  port BufferGet(
                  @ The requested size
                  $size: FwSizeType
                ) -> Fw.Buffer

  @ The bufferIn interface for a passive component
  interface PassiveBufferIn {

    @ Port for receiving buffers
    sync input port bufferIn: BufferSend

    @ Port for returning buffers received on bufferIn
    output port bufferInReturn: BufferSend

  }

  @ The bufferOut interface for a passive component
  interface PassiveBufferOut {

    @ Port for sending buffers
    output port bufferOut: BufferSend

    @ Port for receiving buffers sent on bufferOut and then returned
    sync input port bufferOutReturn: BufferSend

  }

}
