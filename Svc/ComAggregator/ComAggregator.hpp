// ======================================================================
// \title  ComAggregator.hpp
// \author lestarch
// \brief  hpp file for ComAggregator component implementation class
// ======================================================================

#ifndef Svc_ComAggregator_HPP
#define Svc_ComAggregator_HPP

#include <atomic>
#include "Os/Mutex.hpp"
#include "Svc/ComAggregator/ComAggregatorComponentAc.hpp"

namespace Svc {

class ComAggregator final : public ComAggregatorComponentBase {
    friend class ComAggregatorTester;  // Allow unit test access to private members
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ComAggregator object
    ComAggregator(const char* const compName  //!< The component name
    );

    //! Destroy ComAggregator object
    ~ComAggregator();

    void preamble() override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comStatusIn
    //!
    //! Port receiving the general status from the downstream component
    //! indicating it is ready or not-ready for more input
    void comStatusIn_handler(FwIndexType portNum,    //!< The port number
                             Fw::Success& condition  //!< Condition success/failure
                             ) override;

    //! Handler implementation for dataIn
    //!
    //! Port to receive data to frame, in a Fw::Buffer with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Buffer coming from a deallocate call in a ComDriver component
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

    //! Handler implementation for timeout
    void timeout_handler(FwIndexType portNum,  //!< The port number
                         U32 context           //!< The call order
                         ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action doClear of state machine Svc_AggregationMachine
    //!
    //! Clear the buffer fill state, last status
    void Svc_AggregationMachine_action_doClear(SmId smId,                             //!< The state machine id
                                               Svc_AggregationMachine::Signal signal  //!< The signal
                                               ) override;

    //! Implementation for action doFill of state machine Svc_AggregationMachine
    //!
    //! Fill the buffer with data
    void Svc_AggregationMachine_action_doFill(SmId smId,                              //!< The state machine id
                                              Svc_AggregationMachine::Signal signal,  //!< The signal
                                              const Svc::ComDataContextPair& value    //!< The value
                                              ) override;

    //! Implementation for action doSend of state machine Svc_AggregationMachine
    //!
    //! Send the buffer data
    void Svc_AggregationMachine_action_doSend(SmId smId,                             //!< The state machine id
                                              Svc_AggregationMachine::Signal signal  //!< The signal
                                              ) override;

    //! Implementation for action doHold of state machine Svc_AggregationMachine
    //!
    //! Hold a buffer
    void Svc_AggregationMachine_action_doHold(SmId smId,                              //!< The state machine id
                                              Svc_AggregationMachine::Signal signal,  //!< The signal
                                              const Svc::ComDataContextPair& value    //!< The value
                                              ) override;

    //! Implementation for action assertNoStatus of state machine Svc_AggregationMachine
    //!
    //! Assert no status when in fill state
    void Svc_AggregationMachine_action_assertNoStatus(SmId smId,                             //!< The state machine id
                                                      Svc_AggregationMachine::Signal signal  //!< The signal
                                                      ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard isFull of state machine Svc_AggregationMachine
    //!
    //! Check if full
    bool Svc_AggregationMachine_guard_isFull(SmId smId,                              //!< The state machine id
                                             Svc_AggregationMachine::Signal signal,  //!< The signal
                                             const Svc::ComDataContextPair& value    //!< The value
    ) const override;

    //! Implementation for guard willFill of state machine Svc_AggregationMachine
    //!
    //! Check if the incoming buffer will exactly fill the aggregation buffer
    bool Svc_AggregationMachine_guard_willFill(SmId smId,                              //!< The state machine id
                                               Svc_AggregationMachine::Signal signal,  //!< The signal
                                               const Svc::ComDataContextPair& value    //!< The value
    ) const override;

    //! Implementation for guard isNotEmpty of state machine Svc_AggregationMachine
    //!
    //! Check if not empty
    bool Svc_AggregationMachine_guard_isNotEmpty(SmId smId,                             //!< The state machine id
                                                 Svc_AggregationMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard isGood of state machine Svc_AggregationMachine
    //!
    //! Check if last status is good
    bool Svc_AggregationMachine_guard_isGood(SmId smId,                              //!< The state machine id
                                             Svc_AggregationMachine::Signal signal,  //!< The signal
                                             const Fw::Success& value                //!< The value
    ) const override;

  private:
    U8 m_frameBufferStore[ComCfg::AggregationSize];  //!< Buffer to hold the frame data
    Fw::Buffer::OwnershipState m_bufferState =
        Fw::Buffer::OwnershipState::OWNED;  //!< whether m_frameBuffer is owned by TmFramer
    Fw::Buffer m_frameBuffer;
    Fw::ExternalSerializeBufferWithMemberCopy m_frameSerializer;  //!< Serializer for m_frameBuffer
    ComCfg::FrameContext m_lastContext;                           //!< Context for the current frame

    Svc::ComDataContextPair m_held;     //!< Held data while waiting for send
    std::atomic<bool> m_allow_timeout;  //!< Whether status has been received
};

}  // namespace Svc

#endif
