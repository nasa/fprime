// ======================================================================
// \title  FprimeRouter.hpp
// \author thomas-bc
// \brief  hpp file for FprimeRouter component implementation class
// ======================================================================

#ifndef Svc_FprimeRouter_HPP
#define Svc_FprimeRouter_HPP

#include "Svc/FprimeRouter/FprimeRouterComponentAc.hpp"

namespace Svc {

class FprimeRouter final : public FprimeRouterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FprimeRouter object
    FprimeRouter(const char* const compName  //!< The component name
    );

    //! Destroy FprimeRouter object
    ~FprimeRouter();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferIn
    //! Receiving Fw::Buffer from Deframer
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                          Fw::Buffer& packetBuffer, //!< The packet buffer
                          Fw::Buffer& contextBuffer //!< The context buffer
                          ) override;

    // ! Handler for input port cmdResponseIn
    // ! This is a no-op because FprimeRouter does not need to handle command responses
    // ! but the port must be connected
    void cmdResponseIn_handler(FwIndexType portNum,         //!< The port number
                               FwOpcodeType opcode,             //!< The command opcode
                               U32 cmdSeq,                      //!< The command sequence number
                               const Fw::CmdResponse& response  //!< The command response
                               ) override;
};
}  // namespace Svc

#endif
