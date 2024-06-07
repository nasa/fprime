// ======================================================================
// \title  Router.hpp
// \author thomas-bc
// \brief  hpp file for Router component implementation class
// ======================================================================

#ifndef Svc_Router_HPP
#define Svc_Router_HPP

#include "Svc/Router/RouterComponentAc.hpp"

namespace Svc {

class Router : public RouterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Router object
    Router(const char* const compName  //!< The component name
    );

    //! Destroy Router object
    ~Router();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferIn
    //! Receiving Fw::Buffer from Deframer
    void bufferIn_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                          Fw::Buffer& packetBuffer      //!< The packet buffer
                          ) override;

    // ! Handler for input port cmdResponseIn
    // ! This is a no-op because Router does not need to handle command responses
    // ! but the port must be connected
    void cmdResponseIn_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        FwOpcodeType opcode, //!< The command opcode
        U32 cmdSeq, //!< The command sequence number
        const Fw::CmdResponse& response //!< The command response
    ) override;
};
}  // namespace Svc

#endif
