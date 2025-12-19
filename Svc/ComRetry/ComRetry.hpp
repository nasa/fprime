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
    //! State of buffer delivery
    enum RetryState { WAITING_FOR_STATUS, WAITING_FOR_SEND, RETRYING };

  public:
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

    //! Handler implementation for comStatusIn
    //!
    //! Resend last delivered message on failure
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
    //! Buffer coming from a deallocate call in a ComDriver component
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

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
