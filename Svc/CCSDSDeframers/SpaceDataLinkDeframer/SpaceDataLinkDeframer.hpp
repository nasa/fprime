// ======================================================================
// \title  SpaceDataLinkDeframer.hpp
// \author chammard
// \brief  hpp file for SpaceDataLinkDeframer component implementation class
// ======================================================================

#ifndef Svc_SpaceDataLinkDeframer_HPP
#define Svc_SpaceDataLinkDeframer_HPP

#include "Svc/CCSDSDeframers/SpaceDataLinkDeframer/SpaceDataLinkDeframerComponentAc.hpp"

namespace Svc {

class SpaceDataLinkDeframer : public SpaceDataLinkDeframerComponentBase {

  static const U8 SPACE_PACKET_HEADER_SIZE = 5;
  static const U8 SPACE_PACKET_TRAILER_SIZE = 2;
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SpaceDataLinkDeframer object
    SpaceDataLinkDeframer(const char* const compName  //!< The component name
    );

    //! Destroy SpaceDataLinkDeframer object
    ~SpaceDataLinkDeframer();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for framedIn
    //!
    //! Port to receive framed data
    void framedIn_handler(FwIndexType portNum,  //!< The port number
                          Fw::Buffer& data,
                          Fw::Buffer& context) override;
};

}  // namespace Svc

#endif
