// ======================================================================
// \title  TCDeframer.hpp
// \author thomas-bc
// \brief  hpp file for TCDeframer component implementation class
// ======================================================================

#ifndef Svc_CCSDS_TCDeframer_HPP
#define Svc_CCSDS_TCDeframer_HPP

#include "Svc/CCSDS/TCDeframer/TCDeframerComponentAc.hpp"

namespace Svc {
namespace CCSDS {
class TCDeframer : public TCDeframerComponentBase {

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TCDeframer object
    TCDeframer(const char* const compName  //!< The component name
    );

    //! Destroy TCDeframer object
    ~TCDeframer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    //!
    //! Port to receive framed data
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                          Fw::Buffer& data,
                          const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent frame buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
      Fw::Buffer& data,  //!< The buffer
      const ComCfg::FrameContext& context) override;

};
}  // namespace CCSDS
}  // namespace Svc

#endif
