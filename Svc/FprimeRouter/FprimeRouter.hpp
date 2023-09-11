// ======================================================================
// \title  FprimeRouter.hpp
// \author mstarch
// \brief  hpp file for FprimeRouter component implementation class
// ======================================================================

#ifndef FprimeRouter_HPP
#define FprimeRouter_HPP

#include "Svc/FprimeRouter/FprimeRouterComponentAc.hpp"

namespace Svc {

class FprimeRouter : public FprimeRouterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object FprimeRouter
    //!
    FprimeRouter(const char* const compName /*!< The component name*/
    );

    //! Destroy object FprimeRouter
    //!
    ~FprimeRouter();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for cmdResponseIn
    //!
    void cmdResponseIn_handler(const NATIVE_INT_TYPE portNum,  /*!< The port number*/
                               FwOpcodeType opCode,            /*!< Command Op Code*/
                               U32 cmdSeq,                     /*!< Command Sequence*/
                               const Fw::CmdResponse& response /*!< The command response argument*/
    );

    //! Handler implementation for deframedOut
    //!
    void deframedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                            Fw::Buffer& data,
                            Fw::Buffer& context);
};

}  // end namespace Svc

#endif
