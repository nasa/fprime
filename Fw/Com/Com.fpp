module Fw {

  type ComBuffer

  @ Port for passing communication packet buffers
  port Com(
            ref : ComBuffer @< Buffer containing packet data
            context: U32 @<  context value; meaning chosen by user
          )

}
