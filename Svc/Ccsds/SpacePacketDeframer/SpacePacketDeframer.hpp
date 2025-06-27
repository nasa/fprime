// ======================================================================
// \title  SpacePacketDeframer.hpp
// \author thomas-bc
// \brief  hpp file for SpacePacketDeframer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_SpacePacketDeframer_HPP
#define Svc_Ccsds_SpacePacketDeframer_HPP

#include "Svc/Ccsds/SpacePacketDeframer/SpacePacketDeframerComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class SpacePacketDeframer final : public SpacePacketDeframerComponentBase {
    friend class SpacePacketDeframerTester;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SpacePacketDeframer object
    SpacePacketDeframer(const char* const compName  //!< The component name
    );

    //! Destroy SpacePacketDeframer object
    ~SpacePacketDeframer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    //!
    //! Port to receive framed data, with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent frame buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
