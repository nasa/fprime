module Fw {

  type Buffer

  port BufferSend(
                   ref fwBuffer: Fw.Buffer
                 )

  port BufferGet(
                  $size: U32
                ) -> Fw.Buffer

}
