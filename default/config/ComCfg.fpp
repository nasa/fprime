# ======================================================================
# FPP file for configuration of the communications stack
# ======================================================================

@ The width of packet descriptors when they are serialized by the framework
dictionary type FwPacketDescriptorType = U16
constant SIZE_OF_FwPacketDescriptorType = 2  @< Size of FwPacketDescriptorType in bytes

module ComCfg {

    @ Spacecraft ID (10 bits) for CCSDS Data Link layer
    dictionary constant SpacecraftId = 0x0044
    
    @ Fixed size of CCSDS TM frames
    dictionary constant TmFrameFixedSize = 1024  # Needs to be at least COM_BUFFER_MAX_SIZE + (2 * SpacePacketHeaderSize) + 1

    @ Aggregation buffer for ComAggregator component
    constant AggregationSize = TmFrameFixedSize - 6 - 6 - 1 - 2  # 2 header (6) + 1 idle byte + 2 trailer bytes

    @ APIDs are 11 bits in the Space Packet protocol, so we use U16. Max value 7FF
    dictionary enum Apid : FwPacketDescriptorType {
        # APIDs prefixed with FW are reserved for F Prime and need to be present
        # in the enumeration. Their values can be changed
        FW_PACKET_COMMAND        = 0x0000  @< Command packet type - incoming
        FW_PACKET_TELEM          = 0x0001  @< Telemetry packet type - outgoing
        FW_PACKET_LOG            = 0x0002  @< Log type - outgoing
        FW_PACKET_FILE           = 0x0003  @< File type - incoming and outgoing
        FW_PACKET_PACKETIZED_TLM = 0x0004  @< Packetized telemetry packet type
        FW_PACKET_DP             = 0x0005  @< Data Product packet type
        FW_PACKET_IDLE           = 0x0006  @< F Prime idle
        FW_PACKET_HAND           = 0x00FE  @< F Prime handshake
        FW_PACKET_UNKNOWN        = 0x00FF  @< F Prime unknown packet
        SPP_IDLE_PACKET          = 0x07FF  @< Per Space Packet Standard, all 1s (11bits) is reserved for Idle Packets
        INVALID_UNINITIALIZED    = 0x0800  @< Anything equal or higher value is invalid and should not be used
    } default INVALID_UNINITIALIZED

    @ Type used to pass context info between components during framing/deframing
    struct FrameContext {
        comQueueIndex: FwIndexType  @< Queue Index used by the ComQueue, other components shall not modify
        apid: Apid                  @< 11 bits APID in CCSDS
        sequenceCount: U16          @< 14 bit Sequence count - sequence count is incremented per APID
        vcId: U8                    @< 6 bit Virtual Channel ID - used for TC and TM
    } default {
        comQueueIndex = 0
        apid = Apid.FW_PACKET_UNKNOWN
        sequenceCount = 0
        vcId = 1
    }

}
