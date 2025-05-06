// ======================================================================
// \title  SpacePacketFramer.hpp
// \author chammard
// \brief  hpp file for SpacePacketFramer component implementation class
// ======================================================================

#ifndef Svc_CCSDS_SpacePacketFramer_HPP
#define Svc_CCSDS_SpacePacketFramer_HPP

#include "Svc/CCSDS/SpacePacketFramer/SpacePacketFramerComponentAc.hpp"

namespace Svc {

namespace CCSDS {

class SpacePacketFramer final : public SpacePacketFramerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SpacePacketFramer object
    SpacePacketFramer(const char* const compName  //!< The component name
    );

    //! Destroy SpacePacketFramer object
    ~SpacePacketFramer();

  PRIVATE:
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

    U16 m_packetSequenceCount = 0;  //!< Packet sequence count (TODO: needs to be per-APID; and wrap around at 14 bits)
};

}  // namespace CCSDS

}  // namespace Svc

#endif
