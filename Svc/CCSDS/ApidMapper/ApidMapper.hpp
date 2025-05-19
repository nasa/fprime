// ======================================================================
// \title  ApidMapper.hpp
// \author chammard
// \brief  hpp file for ApidMapper component implementation class
// ======================================================================

#ifndef Svc_CCSDS_ApidMapper_HPP
#define Svc_CCSDS_ApidMapper_HPP

#include "Svc/CCSDS/ApidMapper/ApidMapperComponentAc.hpp"

namespace Svc {

namespace CCSDS {

class ApidMapper final : public ApidMapperComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ApidMapper object
    ApidMapper(const char* const compName  //!< The component name
    );

    //! Destroy ApidMapper object
    ~ApidMapper();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comStatusIn
    //!
    //! Port receiving the general status from the downstream component
    //! indicating it is ready or not-ready for more input
    void comStatusIn_handler(FwIndexType portNum,    //!< The port number
                             Fw::Success& condition  //!< Condition success/failure
                             ) override;

    //! Handler implementation for dataIn
    //!
    //! Port to receive data to frame, in a Fw::Buffer with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Buffer coming from a deallocate call in a ComDriver component
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

    //! This struct helps to track sequence counts per APID
    struct ApidSequenceEntry {
        ComCfg::APID::T apid = ComCfg::APID::FW_PACKET_UNKNOWN;
        U16 sequenceCount;
    };

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    static const U8 MAX_TRACKED_APIDS = 5;
    ApidSequenceEntry m_apidSequences[MAX_TRACKED_APIDS];
};

}  // namespace CCSDS
}  // namespace Svc

#endif
