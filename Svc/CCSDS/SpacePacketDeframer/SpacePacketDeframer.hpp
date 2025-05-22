// ======================================================================
// \title  SpacePacketDeframer.hpp
// \author thomas-bc
// \brief  hpp file for SpacePacketDeframer component implementation class
// ======================================================================

#ifndef Svc_CCSDS_SpacePacketDeframer_HPP
#define Svc_CCSDS_SpacePacketDeframer_HPP

#include "Svc/CCSDS/SpacePacketDeframer/SpacePacketDeframerComponentAc.hpp"

namespace Svc {

namespace CCSDS {

class SpacePacketDeframer final : public SpacePacketDeframerComponentBase {

    static constexpr U8 MAX_TRACKED_APIDS = 5;
  
    public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SpacePacketDeframer object
    SpacePacketDeframer(const char* const compName  //!< The component name
    );

    //! Destroy SpacePacketDeframer object
    ~SpacePacketDeframer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    //!
    //! Port to receive framed data, with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent frame buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

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
