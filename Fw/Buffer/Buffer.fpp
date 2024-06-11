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
                  $size: U32
                ) -> Fw.Buffer

}
