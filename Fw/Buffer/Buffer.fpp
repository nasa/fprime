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


    @ Port for sending data buffer along with context buffer
    @ This is useful for passing data that needs context to be interpreted
    # Review note: I thought using a ref for context could be neat, maybe we don't want that
    # For example, a downstream framer could theoretically "send context back" by modifying the reference...
    port DataWithContext(ref data: Fw.Buffer, ref context: FprimeProtocol.DataLinkContext)
}
