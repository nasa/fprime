// ======================================================================
// \title  Deframer.hpp
// \author chammard
// \brief  hpp file for Deframer component implementation class
// ======================================================================

#ifndef Svc_Deframer_HPP
#define Svc_Deframer_HPP

#include "Svc/Deframer/DeframerComponentAc.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {


namespace FrameConfig {
  //! Token type for F Prime frame header
  typedef U32 TokenType;
  static const U8 HEADER_SIZE = sizeof(TokenType) * 2;
  static const U32 CHECKSUM_SIZE = HASH_DIGEST_LENGTH;
}

class Deframer : public DeframerComponentBase {


  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Deframer object
    Deframer(const char* const compName  //!< The component name
    );

    //! Destroy Deframer object
    ~Deframer();

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
