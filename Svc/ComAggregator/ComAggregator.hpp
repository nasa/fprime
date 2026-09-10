// ======================================================================
// \title  ComAggregator.hpp
// \author lestarch
// \brief  hpp file for ComAggregator component implementation class
// ======================================================================

#ifndef Svc_ComAggregator_HPP
#define Svc_ComAggregator_HPP

#include <atomic>
#include "Os/Mutex.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Utils/IdlePacket.hpp"
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

    //! Configure the aggregator
    //!
    //! When spanning is enabled, packets that do not fit in the remaining aggregation space are split
    //! across aggregates (CCSDS TM packet spanning): the leading bytes fill the current aggregate and
    //! the remainder continues in subsequent aggregates. The First Header Pointer is reported through
    //! the ComCfg::FrameContext for the downstream TM framer. Must be called before the component is
    //! started and before any data is received.
    void configure(bool spanningEnabled  //!< Enable CCSDS TM packet spanning across aggregates
    );

    void preamble() override;

    static constexpr FwSizeType NON_SPANNING_CAPACITY =
        static_cast<FwSizeType>(ComCfg::AggregationSize) - Ccsds::Utils::IdlePacket::MIN_SIZE;
    static_assert(NON_SPANNING_CAPACITY >= FW_COM_BUFFER_MAX_SIZE + Ccsds::SpacePacketHeader::SERIALIZED_SIZE,
                  "ComCfg::AggregationSize must hold a full com buffer Space Packet without spanning");
    static_assert(NON_SPANNING_CAPACITY >= FW_FILE_BUFFER_MAX_SIZE + Ccsds::SpacePacketHeader::SERIALIZED_SIZE,
                  "ComCfg::AggregationSize must hold a full file buffer Space Packet without spanning");

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

    //! Implementation for action doSplitHold of state machine Svc_AggregationMachine
    //!
    //! Hold a buffer, first splitting its leading bytes into the remaining frame space when spanning
    void Svc_AggregationMachine_action_doSplitHold(SmId smId,                              //!< The state machine id
                                                   Svc_AggregationMachine::Signal signal,  //!< The signal
                                                   const Svc::ComDataContextPair& value    //!< The value
                                                   ) override;

    //! Implementation for action doNoteFailure of state machine Svc_AggregationMachine
    //!
    //! Record that the last frame was not acknowledged
    void Svc_AggregationMachine_action_doNoteFailure(SmId smId,                             //!< The state machine id
                                                     Svc_AggregationMachine::Signal signal  //!< The signal
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

    //! Implementation for guard isSpanFull of state machine Svc_AggregationMachine
    //!
    //! Check if the aggregation buffer was completely filled from held continuation data
    bool Svc_AggregationMachine_guard_isSpanFull(SmId smId,                             //!< The state machine id
                                                 Svc_AggregationMachine::Signal signal  //!< The signal
    ) const override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Get the remaining capacity of the aggregation buffer
    FwSizeType remainingCapacity() const;

    //! Record the First Header Pointer at the current fill offset, if not already recorded
    void markFirstHeaderIfUnset();

    //! Fill the aggregation buffer from the held (partially consumed) buffer, returning it when consumed
    void fillFromHeld();

    //! Fill the residual aggregation space with an SPP idle packet, spanning it into the next
    //! aggregate when the residual space is smaller than a minimum idle packet
    void fillResidualWithIdle();

    //! Return a consumed buffer and signal readiness for another buffer
    void returnAndSignalReady(const Svc::ComDataContextPair& pair);

    //! Recover from FAILURE on the next SUCCESS once downstream is ready
    //!
    //! Drops carried idle bytes and a held remainder whose head was in the lost frame.
    void dropLostFrameState();

  private:
    static constexpr U16 FHP_UNSET = 0xFFFF;  //!< Sentinel: no packet header recorded in the current aggregate

    static_assert(static_cast<FwSizeType>(ComCfg::AggregationSize) > Ccsds::Utils::IdlePacket::MIN_SIZE,
                  "ComCfg::AggregationSize must exceed the minimum idle packet size");

    U8 m_frameBufferStore[ComCfg::AggregationSize];  //!< Buffer to hold the frame data
    std::atomic<Fw::Buffer::OwnershipState> m_bufferState{
        Fw::Buffer::OwnershipState::OWNED};  //!< whether m_frameBuffer is owned by TmFramer; shared with the sync
                                             //!< dataReturnIn caller
    Fw::Buffer m_frameBuffer;
    Fw::ExternalSerializeBufferWithMemberCopy m_frameSerializer;  //!< Serializer for m_frameBuffer
    ComCfg::FrameContext m_lastContext;                           //!< Context for the current frame

    Svc::ComDataContextPair m_held;     //!< Held data while waiting for send
    std::atomic<bool> m_allow_timeout;  //!< Whether status has been received

    bool m_spanning;          //!< Whether packet spanning is enabled
    FwSizeType m_capacity;    //!< Active aggregation capacity in bytes
    FwSizeType m_heldOffset;  //!< Bytes of the held buffer already consumed into previous aggregates
    U16 m_fhp;                //!< First Header Pointer for the current aggregate (FHP_UNSET if none)
    U8 m_pendingIdle[Ccsds::Utils::IdlePacket::MIN_SIZE] = {};  //!< Idle packet bytes spanning into the next aggregate
    FwSizeType m_pendingIdleCount;                              //!< Number of valid bytes in m_pendingIdle
    FwSizeType m_leadingIdleCount;  //!< Number of carried idle bytes at the start of the current aggregate
    bool m_lastFrameLost;           //!< Whether the last frame was not acknowledged
};

}  // namespace Svc

#endif
