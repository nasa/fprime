// ======================================================================
// \title  FprimeRouter.hpp
// \author thomas-bc
// \brief  hpp file for FprimeRouter component implementation class
// ======================================================================

#ifndef Svc_FprimeRouter_HPP
#define Svc_FprimeRouter_HPP

#include "Svc/FprimeRouter/FprimeRouterComponentAc.hpp"
#include "Fw/Types/ActiveEnumAc.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

class FprimeRouter final : public FprimeRouterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FprimeRouter object
    FprimeRouter(const char* const compName  //!< The component name
    );

    //! Destroy FprimeRouter object
    ~FprimeRouter();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferIn
    //! Receiving Fw::Buffer from Deframer
    void dataIn_handler(FwIndexType portNum,                 //!< The port number
                        Fw::Buffer& packetBuffer,            //!< The packet buffer
                        const ComCfg::FrameContext& context  //!< The context object
                        ) override;

    // ! Handler for input port cmdResponseIn
    // ! This is a no-op because FprimeRouter does not need to handle command responses
    // ! but the port must be connected
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opcode,             //!< The command opcode
                               U32 cmdSeq,                      //!< The command sequence number
                               const Fw::CmdResponse& response  //!< The command response
                               ) override;

    //! Handler implementation for fileBufferReturnIn
    //!
    //! Port for receiving ownership back of buffers sent on fileOut
    void fileBufferReturnIn_handler(FwIndexType portNum,  //!< The port number
                                    Fw::Buffer& fwBuffer  //!< The buffer
                                    ) override;

  private:
    // ----------------------------------------------------------------------
    // Buffer-to-context association table
    // ----------------------------------------------------------------------

    //! One buffer-to-context association
    struct BufferContextEntry {
        Fw::Active state;              //!< ACTIVE if this slot holds an outstanding association, else INACTIVE
        const U8* key;                 //!< Data pointer of the outstanding buffer
        ComCfg::FrameContext context;  //!< Context to restore when the buffer returns
    };

    //! Record the context for a buffer being handed off.
    //! \return SUCCESS, or FAILURE if the table is full
    Fw::Success insertContext(const Fw::Buffer& buffer, const ComCfg::FrameContext& context);

    //! Look up and remove the context for a returned buffer.
    //! \return SUCCESS, or FAILURE if the buffer was not found
    Fw::Success takeContext(const Fw::Buffer& buffer, ComCfg::FrameContext& context);

    //! Fixed-size buffer-to-context table (all entries initialized unused)
    BufferContextEntry m_bufferContextTable[ComCfg::RouterBufferContextTableSize];
};
}  // namespace Svc

#endif
