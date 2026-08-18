// ======================================================================
// \title  PassThroughRouter.hpp
// \author kessler
// \brief  hpp file for PassThroughRouter component implementation class
// ======================================================================

#ifndef Svc_PassThroughRouter_HPP
#define Svc_PassThroughRouter_HPP

#include "Svc/PassThroughRouter/PassThroughRouterComponentAc.hpp"

namespace Svc {

class PassThroughRouter final : public PassThroughRouterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct PassThroughRouter object
    PassThroughRouter(const char* const compName  //!< The component name
    );

    //! Destroy PassThroughRouter object
    ~PassThroughRouter();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for allPacketsReturnIn
    //!
    //! Port for receiving ownership back of buffers sent on allPacketsOut
    void allPacketsReturnIn_handler(FwIndexType portNum,  //!< The port number
                                    Fw::Buffer& fwBuffer  //!< The buffer
                                    ) override;

    //! Handler implementation for dataIn
    //!
    //! Receiving data (Fw::Buffer) to be routed with optional context to help with routing
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& packetBuffer,
                        const ComCfg::FrameContext& context) override;
};

}  // namespace Svc

#endif
