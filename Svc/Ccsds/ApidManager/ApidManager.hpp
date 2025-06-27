// ======================================================================
// \title  ApidManager.hpp
// \author thomas-bc
// \brief  hpp file for ApidManager component implementation class
// ======================================================================

#ifndef Svc_Ccsds_ApidManager_HPP
#define Svc_Ccsds_ApidManager_HPP

#include "Fw/Com/ComPacket.hpp"
#include "Svc/Ccsds/ApidManager/ApidManagerComponentAc.hpp"

namespace Svc {

namespace Ccsds {

static_assert(ComCfg::APID::SPP_IDLE_PACKET == 0x07FF,
              "SPP_IDLE_PACKET must exist and equal 0x07FF (as specified by standard)");
static_assert(ComCfg::APID::INVALID_UNINITIALIZED == 0x0800,
              "Invalid APID must be 0x0800 (11 bits values allow 0-2047)");
static_assert(ComCfg::APID::FW_PACKET_COMMAND == Fw::ComPacketType::FW_PACKET_COMMAND,
              "APID FW_PACKET_COMMAND must exist, used by the Framework");
static_assert(ComCfg::APID::FW_PACKET_TELEM == Fw::ComPacketType::FW_PACKET_TELEM,
              "APID FW_PACKET_TELEM must exist, used by the Framework");
static_assert(ComCfg::APID::FW_PACKET_LOG == Fw::ComPacketType::FW_PACKET_LOG,
              "APID FW_PACKET_LOG must exist, used by the Framework");
static_assert(ComCfg::APID::FW_PACKET_FILE == Fw::ComPacketType::FW_PACKET_FILE,
              "APID FW_PACKET_FILE must exist, used by the Framework");
static_assert(ComCfg::APID::FW_PACKET_PACKETIZED_TLM == Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM,
              "APID FW_PACKET_PACKETIZED_TLM must exist, used by the Framework");
static_assert(ComCfg::APID::FW_PACKET_UNKNOWN == Fw::ComPacketType::FW_PACKET_UNKNOWN,
              "APID FW_PACKET_UNKNOWN must exist, used by the Framework");

class ApidManager final : public ApidManagerComponentBase {
    friend class ApidManagerTester;  //!< Friend class for testing

  public:
    static constexpr U16 MAX_TRACKED_APIDS = ComCfg::APID::NUM_CONSTANTS;
    static constexpr U16 SEQUENCE_COUNT_ERROR = std::numeric_limits<U16>::max();
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ApidManager object
    ApidManager(const char* const compName  //!< The component name
    );

    //! Destroy ApidManager object
    ~ApidManager() = default;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for validateApidSeqCountIn
    U16 validateApidSeqCountIn_handler(FwIndexType portNum,  //!< The port number
                                       const ComCfg::APID& apid,
                                       U16 seqCount) override;

    //! Handler implementation for validateApidSeqCountIn
    U16 getApidSeqCountIn_handler(FwIndexType portNum,  //!< The port number
                                  const ComCfg::APID& apid,
                                  U16 seqCount) override;

  private:
    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------
    //! Get the sequence count for a given APID and increment it for the next
    //! Wraps around at 14 bits
    U16 getAndIncrementSeqCount(ComCfg::APID::T apid);

    //! Set the next expected sequence count for a given APID
    void setNextSeqCount(ComCfg::APID::T apid, U16 seqCount);

    //! This struct helps track sequence counts per APID
    //! Future work: update to using a map from Fw/DataStructures when available
    struct ApidSequenceEntry {
        ComCfg::APID::T apid = ComCfg::APID::INVALID_UNINITIALIZED;
        U16 sequenceCount = 0;
    };

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    ApidSequenceEntry m_apidSequences[MAX_TRACKED_APIDS];
};

}  // namespace Ccsds
}  // namespace Svc

#endif
