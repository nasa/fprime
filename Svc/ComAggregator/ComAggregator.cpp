// ======================================================================
// \title  ComAggregator.cpp
// \author starchmd
// \brief  cpp file for ComAggregator component implementation class
// ======================================================================

#include "Svc/ComAggregator/ComAggregator.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ComAggregator ::ComAggregator(const char* const compName) :
    ComAggregatorComponentBase(compName),
    m_bufferState(Fw::Buffer::OwnershipState::OWNED),
    m_frameBuffer(m_frameBufferStore, sizeof(m_frameBufferStore)),
    m_frameSerializer(m_frameBuffer.getSerializer())
{}

ComAggregator ::~ComAggregator() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ComAggregator ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    // TODO: signal status and **dispatch**
}

void ComAggregator ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // TODO: signal fill and **dispatch**
}

void ComAggregator ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::NOT_OWNED);
    this->m_bufferState = Fw::Buffer::OwnershipState::OWNED;
}

void ComAggregator ::timeout_handler(FwIndexType portNum, U32 context) {
    // TODO: signal timeout and **dispatch**
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void ComAggregator ::Svc_AggregationMachine_action_doClear(SmId smId, Svc_AggregationMachine::Signal signal) {
    this->m_frameSerializer.resetSer();
    //TODO serialize hold
}

void ComAggregator ::Svc_AggregationMachine_action_doFill(SmId smId,
                                                          Svc_AggregationMachine::Signal signal,
                                                          const Svc::ComDataContextPair& value) {
    this->m_frameSerializer.serializeFrom(value.get_data().getData(), value.get_data().getSize());
    this->m_lastContext = value.get_context();
    Fw::Success good = Fw::Success::SUCCESS;
    this->comStatusOut_out(0, good);
}

void ComAggregator ::Svc_AggregationMachine_action_doSend(SmId smId, Svc_AggregationMachine::Signal signal) {
    this->dataOut_out(0, this->m_frameBuffer, this->m_lastContext);
}

void ComAggregator ::Svc_AggregationMachine_action_doHold(SmId smId,
                                                          Svc_AggregationMachine::Signal signal,
                                                          const Svc::ComDataContextPair& value) {
    this->m_held = value;
}

void ComAggregator ::Svc_AggregationMachine_action_doStatus(SmId smId,
                                                            Svc_AggregationMachine::Signal signal,
                                                            const Fw::Success& value) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::OWNED);
    // Autocode for comStatusOut port will not alter the value therefore it is
    // safe to cast away const.
    this->comStatusOut_out(0, const_cast<Fw::Success&>(value));
}

void ComAggregator ::Svc_AggregationMachine_action_assertNoFill(SmId smId, Svc_AggregationMachine::Signal signal) {
    // Fill is not possible in this state, confirm by assertion
    FW_ASSERT(0);
}

void ComAggregator ::Svc_AggregationMachine_action_assertNoStatus(SmId smId, Svc_AggregationMachine::Signal signal) {
    // Status is not possible in this state, confirm by assertion
    FW_ASSERT(0);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool ComAggregator ::Svc_AggregationMachine_guard_isFull(SmId smId,
                                                         Svc_AggregationMachine::Signal signal,
                                                         const Svc::ComDataContextPair& value) const {
    FW_ASSERT(value.get_data().getSize() <= ComCfg::AggregationSize);
    return (this->m_frameSerializer.getBuffLeft() < value.get_data().getSize());
}

bool ComAggregator ::Svc_AggregationMachine_guard_isGood(SmId smId,
                                                         Svc_AggregationMachine::Signal signal,
                                                         const Fw::Success& value) const {
    return value == Fw::Success::SUCCESS;
}

}  // namespace Svc
