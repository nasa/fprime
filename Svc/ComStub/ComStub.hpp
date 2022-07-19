// ======================================================================
// \title  ComStub.hpp
// \author mstarch
// \brief  hpp file for ComStub component implementation class
// ======================================================================

#ifndef ComStub_HPP
#define ComStub_HPP

#include "Svc/ComStub/ComStubComponentAc.hpp"

namespace Svc {

class ComStub : public ComStubComponentBase {
  public:
    const NATIVE_UINT_TYPE retryLimit = 10;
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComStub
    //!
    ComStub(const char* const compName /*!< The component name*/
    );

    //! Initialize object ComStub
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object ComStub
    //!
    ~ComStub();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comDataIn
    //!
    Drv::SendStatus comDataIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                      Fw::Buffer& sendBuffer);

    //! Handler implementation for drvConnected
    //!
    void drvConnected_handler(const NATIVE_INT_TYPE portNum);

    //! Handler implementation for drvDataIn
    //!
    void drvDataIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/ Fw::Buffer &recvBuffer, const Drv::RecvStatus &recvStatus);
};

}  // end namespace Svc

#endif
