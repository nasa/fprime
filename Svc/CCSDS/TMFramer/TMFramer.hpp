// ======================================================================
// \title  TMFramer.hpp
// \author chammard
// \brief  hpp file for TMFramer component implementation class
// ======================================================================

#ifndef Svc_CCSDS_TMFramer_HPP
#define Svc_CCSDS_TMFramer_HPP

#include "Svc/CCSDS/TMFramer/TMFramerComponentAc.hpp"

namespace Svc {

namespace CCSDS {

class TMFramer final : public TMFramerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TMFramer object
    TMFramer(const char* const compName  //!< The component name
    );

    //! Destroy TMFramer object
    ~TMFramer();

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

    U8 m_frameBuffer[ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize];  //!< Buffer to hold the frame data
};

}  // namespace CCSDS

}  // namespace Svc

#endif
