// ======================================================================
// \title  ApidManager.hpp
// \author thomas-bc
// \brief  hpp file for ApidManager component implementation class
// ======================================================================

#ifndef Svc_CCSDS_ApidManager_HPP
#define Svc_CCSDS_ApidManager_HPP

#include "Svc/CCSDS/ApidManager/ApidManagerComponentAc.hpp"

namespace Svc {

namespace CCSDS {

class ApidManager final : public ApidManagerComponentBase {

    static constexpr U8 MAX_TRACKED_APIDS = ComCfg::APID::NUM_CONSTANTS;
    static constexpr U16 SEQUENCE_COUNT_ERROR = std::numeric_limits<U16>::max();

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ApidManager object
    ApidManager(const char* const compName  //!< The component name
    );

    //! Destroy ApidManager object
    ~ApidManager();

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

    void setNextSeqCount(ComCfg::APID::T apid, U16 seqCount);

    //! This struct helps track sequence counts per APID
    struct ApidSequenceEntry {
        ComCfg::APID::T apid = ComCfg::APID::FW_PACKET_UNKNOWN;
        U16 sequenceCount;
    };

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    ApidSequenceEntry m_apidSequences[MAX_TRACKED_APIDS];
};

}  // namespace CCSDS
}  // namespace Svc

#endif
