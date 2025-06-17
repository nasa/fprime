// ======================================================================
// \title  SpacePacketFramer.hpp
// \author thomas-bc
// \brief  hpp file for SpacePacketFramer component implementation class
// ======================================================================

#ifndef Svc_CCSDS_SpacePacketFramer_HPP
#define Svc_CCSDS_SpacePacketFramer_HPP

#include "Svc/CCSDS/SpacePacketFramer/SpacePacketFramerComponentAc.hpp"
#include "config/APIDEnumAc.hpp"

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
    //!
    //! Header fields are set according to the CCSDS Space Packet standard, and
    //! is described in the component SDD.
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Buffer coming from a deallocate call in a ComDriver component
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

};

}  // namespace CCSDS

}  // namespace Svc

#endif
