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

    @ Port for sending a packet (using an Fw::ComBuffer copied through the port)
    @ along with the packet's APID. Used at the App<->Com interface for senders
    @ that own static / inline buffer storage (e.g. EventManager, TlmChan,
    @ TlmPacketizer) and from FprimeRouter to CmdDispatcher. The packet's APID
    @ is passed explicitly rather than embedded in the buffer. The optional
    @ context value is forwarded by command dispatchers for response correlation
    @ (e.g. FpySequencer cmdUid); senders that do not need command tracking pass 0.
    port ComBufferSend(ref data: Fw.ComBuffer, packetType: ComCfg.Apid, context: U32)

    @ Port for sending a packet (using an Fw::Buffer whose ownership is transferred
    @ to the receiver) along with the packet's APID. Used at the App<->Com interface
    @ for senders that own externally-managed buffers (e.g. FileDownlink, FileUplink
    @ over a BufferManager). The packet's APID is passed explicitly rather than
    @ embedded in the buffer. Ownership is returned on a paired Fw.BufferSend port.
    port ComPacketSend(ref data: Fw.Buffer, packetType: ComCfg.Apid)

}
