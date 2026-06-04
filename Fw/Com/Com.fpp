module Fw {

  type ComBuffer

  @ Port for passing communication packet buffers
  port Com(
            ref data: ComBuffer @< Buffer containing packet data
            context: U32 @< Call context value; meaning chosen by user
          )

  @ Port for sending/receiving a packet to/from the Com layer for transmission
  @ ComBufferSend variant is for ComBuffer type which is statically allocated and fully serialized
  port ComBufferSend(ref data: Fw.ComBuffer, packetType: ComCfg.Apid, context: U32)

  @ Port for sending/receiving a packet to/from the Com layer for transmission
  @ ComPacketSend variant is for Fw.Buffer type which may require careful memory management
  port ComPacketSend(ref data: Fw.Buffer, packetType: ComCfg.Apid)

}
