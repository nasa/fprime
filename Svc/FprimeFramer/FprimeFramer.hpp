// ======================================================================
// \title  FprimeFramer.hpp
// \author thomas-bc
// \brief  hpp file for FprimeFramer component implementation class
// ======================================================================

#ifndef Svc_FprimeFramer_HPP
#define Svc_FprimeFramer_HPP

#include "Svc/FprimeFramer/FprimeFramerComponentAc.hpp"

namespace Svc {

class FprimeFramer final : public FprimeFramerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FprimeFramer object
    FprimeFramer(const char* const compName  //!< The component name
    );

    //! Destroy FprimeFramer object
    ~FprimeFramer();

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

    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------

    //! Helper function to send the framed data out of the component
    //! This sequentially calls both frameDataOut and frameStreamOut ports if connected
    void framedOut_helper(Fw::Buffer& frameBuffer, const ComCfg::FrameContext& context);
};

}  // namespace Svc

#endif
