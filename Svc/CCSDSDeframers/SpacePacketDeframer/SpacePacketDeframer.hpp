// ======================================================================
// \title  SpacePacketDeframer.hpp
// \author chammard
// \brief  hpp file for SpacePacketDeframer component implementation class
// ======================================================================

#ifndef Svc_SpacePacketDeframer_HPP
#define Svc_SpacePacketDeframer_HPP

#include "Svc/CCSDSDeframers/SpacePacketDeframer/SpacePacketDeframerComponentAc.hpp"

namespace Svc {

class SpacePacketDeframer : public SpacePacketDeframerComponentBase {
  
  static const U8 SPACE_PACKET_HEADER_SIZE = 6;
  
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SpacePacketDeframer object
    SpacePacketDeframer(const char* const compName  //!< The component name
    );

    //! Destroy SpacePacketDeframer object
    ~SpacePacketDeframer();

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
