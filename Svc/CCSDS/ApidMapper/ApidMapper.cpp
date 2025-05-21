// ======================================================================
// \title  ApidMapper.cpp
// \author thomas-bc
// \brief  cpp file for ApidMapper component implementation class
// ======================================================================

#include "Svc/CCSDS/ApidMapper/ApidMapper.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ApidMapper ::ApidMapper(const char* const compName) : ApidMapperComponentBase(compName) {
    // Initialize the APID sequence table with APID values that need to be counted (order does not matter)
    this->m_apidSequences[0].apid = ComCfg::APID::FW_PACKET_LOG;
    this->m_apidSequences[1].apid = ComCfg::APID::FW_PACKET_TELEM;
    this->m_apidSequences[2].apid = ComCfg::APID::FW_PACKET_FILE;
    this->m_apidSequences[3].apid = ComCfg::APID::FW_PACKET_PACKETIZED_TLM;
    this->m_apidSequences[4].apid = ComCfg::APID::FW_PACKET_UNKNOWN;

}

ApidMapper ::~ApidMapper() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ApidMapper ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void ApidMapper ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    ComCfg::FrameContext contextCopy = context;

    // Deserialize Packet Descriptor to map to an APID
    auto deserializer = data.getDeserializer();
    FwPacketDescriptorType descriptorValue = 0;
    Fw::SerializeStatus status = deserializer.deserialize(descriptorValue);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

    // Mapping one-to-one: Descriptor value is APID value
    ComCfg::APID::T apid = static_cast<ComCfg::APID::T>(descriptorValue);
    contextCopy.setapid(static_cast<ComCfg::APID::T>(apid));
    contextCopy.setsequenceCount(this->getAndIncrementSeqCount(static_cast<ComCfg::APID::T>(apid)));
    // printf("APID: %d, SeqCount: %d\n", apid, contextCopy.getsequenceCount());

    // Forward the buffer and context to the output port
    this->dataOut_out(0, data, contextCopy);
}

void ApidMapper ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(portNum, data, context);
}


U16 ApidMapper ::getAndIncrementSeqCount(ComCfg::APID::T apid) {
    for (U8 i = 0; i < MAX_TRACKED_APIDS; ++i) {
        if (m_apidSequences[i].apid == apid) {
            U16 seq = m_apidSequences[i].sequenceCount;
            m_apidSequences[i].sequenceCount = (seq + 1) % (1 << 14); // Wrap around at 14 bits
            return seq;
        }
    }
    return 0; // or some error value
}

}  // namespace CCSDS

}  // namespace Svc
