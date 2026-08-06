// ======================================================================
// \title  OpensslDecaps.hpp
// \author cadena
// \brief  hpp file for OpensslDecaps component implementation class
// ======================================================================

#ifndef Svc_Ccsds_OpensslDecaps_HPP
#define Svc_Ccsds_OpensslDecaps_HPP

#include "Svc/Encryption/OpensslDecaps/OpensslDecapsComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class OpensslDecaps final : public OpensslDecapsComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct OpensslDecaps object
    OpensslDecaps(const char* const compName  //!< The component name
    );

    //! Destroy OpensslDecaps object
    ~OpensslDecaps();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for kemMsgIn
    //!
    //! port for taking in kem message
    void kemMsgIn_handler(FwIndexType portNum,  //!< The port number
                          Fw::Buffer& fwBuffer  //!< The buffer
                          ) override;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
