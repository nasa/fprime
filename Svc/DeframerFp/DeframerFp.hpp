// ======================================================================
// \title  DeframerFp.hpp
// \author chammard
// \brief  hpp file for DeframerFp component implementation class
// ======================================================================

#ifndef Svc_DeframerFp_HPP
#define Svc_DeframerFp_HPP

#include "Svc/DeframerFp/DeframerFpComponentAc.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {


namespace FrameConfig {
  //! Token type for F Prime frame header
  typedef U32 TokenType;
  static const U8 HEADER_SIZE = sizeof(TokenType) * 2;
  static const U32 CHECKSUM_SIZE = HASH_DIGEST_LENGTH;
}

class DeframerFp : public DeframerFpComponentBase {


  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct DeframerFp object
    DeframerFp(const char* const compName  //!< The component name
    );

    //! Destroy DeframerFp object
    ~DeframerFp();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for frame
    //!
    //! Port to receive framed data
    void framedIn_handler(FwIndexType portNum,  //!< The port number
                       Fw::Buffer& data,
                       Fw::Buffer& context) override;
};

}  // namespace Svc

#endif
