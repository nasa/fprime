// ======================================================================
// \title  ComRetry.hpp
// \author valdaarhun
// \brief  hpp file for ComRetry component implementation class
// ======================================================================

#ifndef Svc_ComRetry_HPP
#define Svc_ComRetry_HPP

#include "Svc/ComRetry/ComRetryComponentAc.hpp"

namespace Svc {

class ComRetry final : public ComRetryComponentBase {
    friend class ComRetryTester;

    //! State of buffer delivery
    enum RetryState { WAITING_FOR_STATUS, WAITING_FOR_SEND, RETRYING };

  public:
    //! Minimum instance queue depth: three communication adapter protocol messages plus one health ping (see SDD)
    static constexpr FwSizeType MIN_QUEUE_DEPTH = 4;

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ComRetry object
    ComRetry(const char* const compName  //!< The component name
    );

    //! Destroy ComRetry object
    ~ComRetry();

    //! Configure the number of retries
    void configure(U32 num_retries  //!< Number of retries allowed
    );

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Verify the instance queue depth is at least MIN_QUEUE_DEPTH before processing messages
    void preamble() override;

    //! Handler implementation for comStatusIn
    //!
    //! Forward status upstream, or resend the stored message on recovery SUCCESS after a failure
    void comStatusIn_handler(FwIndexType portNum,    //!< The port number
                             Fw::Success& condition  //!< Condition success/failure
                             ) override;

    //! Handler implementation for dataIn
    //!
    //! Port to receive data in a Fw::Buffer with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Receive ownership of the Fw::Buffer sent on dataOut
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

    //! Handler implementation for pingIn
    //!
    //! Return the health ping key on pingOut
    void pingIn_handler(FwIndexType portNum,  //!< The port number
                        U32 key               //!< Value to return to pinger
                        ) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    U32 m_num_retries;                         //!< Maximum number of retries
    U32 m_retry_count;                         //!< Track number of attempted retries
    RetryState m_retry_state;                  //!< Track current retry state
    ComCfg::FrameContext m_context;            //!< Context for the current frame
    Fw::Buffer m_buffer;                       //!< Store incoming buffer
    Fw::Buffer::OwnershipState m_bufferState;  //!< Track ownership of stored buffer
};

}  // namespace Svc

#endif
