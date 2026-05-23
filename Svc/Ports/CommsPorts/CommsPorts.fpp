#####
# Communications Ports:
#
# A port for passing framing / deframing data
# This is used by the Communications components which need context to interpret framed data
#####

module Svc {
    @ Struct representing a communications data buffer along with context information
    @ for use storing the inputs ComDataWithContext port
    struct ComDataContextPair {
        data: Fw.Buffer
        context: ComCfg.FrameContext
    }
 
    @ Port for sending communications data (frames) buffer along with context information
    port ComDataWithContext(ref data: Fw.Buffer, context: ComCfg.FrameContext)


    @ Port for sending/receiving a packet to/from the Com layer for transmission
    @ ComBufferSend variant is for ComBuffer type which is statically allocated and fully serialized
    port ComBufferSend(ref data: Fw.ComBuffer, packetType: ComCfg.Apid, context: U32)

    # REVIEW NOTE: I would like to get rid of the context: U32 above... to confirm with Tim

    @ Port for sending/receiving a packet to/from the Com layer for transmission
    @ ComPacketSend variant is for Fw.Buffer type which may require careful memory management
    port ComPacketSend(ref data: Fw.Buffer, packetType: ComCfg.Apid)

}
