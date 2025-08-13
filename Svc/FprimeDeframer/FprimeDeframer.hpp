// ======================================================================
// \title  FprimeDeframer.hpp
// \author thomas-bc
// \brief  hpp file for FprimeDeframer component implementation class
// ======================================================================

#ifndef Svc_FprimeDeframer_HPP
#define Svc_FprimeDeframer_HPP

#include "Svc/FprimeDeframer/FprimeDeframerComponentAc.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

class FprimeDeframer final : public FprimeDeframerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FprimeDeframer object
    FprimeDeframer(const char* const compName  //!< The component name
    );

    //! Destroy FprimeDeframer object
    ~FprimeDeframer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for frame
    //!
    //! Port to receive framed data. The handler will strip the header and trailer from the frame
    //! and pass the deframed data to the deframed output port.
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent frame buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,     //!< The buffer
                              const ComCfg::FrameContext& context) override;
};

}  // namespace Svc

#endif
