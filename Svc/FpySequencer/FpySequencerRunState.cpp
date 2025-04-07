#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
namespace Svc {

using Signal = FpySequencer_SequencerStateMachineStateMachineBase::Signal;

Signal FpySequencer::dispatchStatement() {
    if (this->m_runtime.nextStatementIndex == this->m_sequenceObj.getheader().getstatementCount()) {
        return Signal::result_dispatchStatement_noMoreStatements;
    }

    // check to make sure no array out of bounds
    FW_ASSERT(this->m_runtime.nextStatementIndex < this->m_sequenceObj.getheader().getstatementCount());

    const Fpy::Statement& nextStatement = this->m_sequenceObj.getstatements()[this->m_runtime.nextStatementIndex];
    this->m_runtime.nextStatementIndex++;
    this->m_runtime.currentStatementOpcode = nextStatement.getopCode();

    Fw::Success result;

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    if (nextStatement.gettype() == Fpy::StatementType::DIRECTIVE) {
        result = this->dispatchDirective(nextStatement);
    } else {
        result = this->dispatchCommand(nextStatement);
    }

    if (result == Fw::Success::SUCCESS) {
        this->m_tlm.statementsDispatched++;
        return Signal::result_dispatchStatement_success;
    } else {
        return Signal::result_dispatchStatement_failure;
    }
}

// dispatches a command out via port.
// return true if successfully dispatched.
Fw::Success FpySequencer::dispatchCommand(const Fpy::Statement& stmt) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(Fw::ComPacket::FW_PACKET_COMMAND);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            sizeof(Fw::ComPacket::FW_PACKET_COMMAND), stat);
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serialize(stmt.getopCode());
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(), sizeof(stmt.getopCode()),
                                            stat);
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serialize(stmt.getargBuf().getBuffAddr(), stmt.getargBuf().getBuffLength(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            stmt.getargBuf().getBuffLength(), stat);
        return Fw::Success::FAILURE;
    }

    // little note--theoretically this could produce a cmdResponse before we send the
    // dispatchSuccess signal. however b/c of priorities the dispatchSuccess signal will
    // always get processed first, leaving us in the right state for the cmdresponse
    this->cmdOut_out(0, cmdBuf, 0);

    return Fw::Success::SUCCESS;
}

Fw::Success FpySequencer::dispatchDirective(const Fpy::Statement& stmt) {
    Fw::SerializeStatus status;
    // make our own esb so we can deser from stmt without breaking its constness
    Fw::ExternalSerializeBuffer argBuf(const_cast<U8*>(stmt.getargBuf().getBuffAddr()),
                                       stmt.getargBuf().getBuffLength());
    argBuf.setBuffLen(stmt.getargBuf().getBuffLength());

    switch (stmt.getopCode()) {
        case Fpy::DirectiveId::WAIT_REL: {
            FpySequencer_WaitRelDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            this->directive_waitRel_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            FpySequencer_WaitAbsDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            this->directive_waitAbs_internalInterfaceInvoke(directive);
            break;
        }
        default: {
            // unsure what this opcode is. check compiler version matches sequencer
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.getopCode());
            return Fw::Success::FAILURE;
        }
    }
    return Fw::Success::SUCCESS;
}

Signal FpySequencer::checkShouldWake() {
    Fw::Time currentTime = this->getTime();

#ifdef FW_USE_TIME_BASE
    if (currentTime.getTimeBase() != this->m_runtime.wakeupTime.getTimeBase()) {
        // cannot compare these times.
        this->log_WARNING_LO_MismatchedTimeBase(currentTime.getTimeBase(), this->m_runtime.wakeupTime.getTimeBase());

        return Signal::result_timeOpFailed;
    }
#endif

#ifdef FW_USE_TIME_CONTEXT
    if (currentTime.getContext() != this->m_runtime.wakeupTime.getContext()) {
        // cannot compare these times.
        this->log_WARNING_LO_MismatchedTimeContext(currentTime.getContext(), this->m_runtime.wakeupTime.getContext());

        return Signal::result_timeOpFailed;
    }
#endif

    if (currentTime < this->m_runtime.wakeupTime) {
        // not time to wake up!
        return Signal::result_checkShouldWake_keepSleeping;
    }

    // say we've finished our sleep
    return Signal::result_checkShouldWake_wakeup;
}

// checks whether the currently executing statement timed out
Signal FpySequencer::checkStatementTimeout() {
    Fw::ParamValid valid;
    F32 timeout = this->paramGet_STATEMENT_TIMEOUT_SECS(valid);
    if (timeout <= 0 || timeout > static_cast<F32>(std::numeric_limits<U32>::max())) {
        // no timeout
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    Fw::Time currentTime = getTime();

#ifdef FW_USE_TIME_BASE
    if (currentTime.getTimeBase() != this->m_runtime.currentStatementDispatchTime.getTimeBase()) {
        // can't compare time base. must have changed
        this->log_WARNING_LO_MismatchedTimeBase(currentTime.getTimeBase(),
                                                this->m_runtime.currentStatementDispatchTime.getTimeBase());
        return Signal::result_timeOpFailed;
    }
#endif
#ifdef FW_USE_TIME_CONTEXT
    if (currentTime.getContext() != this->m_runtime.currentStatementDispatchTime.getContext()) {
        // can't compare time ctx. must have changed
        this->log_WARNING_LO_MismatchedTimeContext(currentTime.getContext(),
                                                   this->m_runtime.currentStatementDispatchTime.getContext());
        return Signal::result_timeOpFailed;
    }
#endif

    if (this->m_runtime.currentStatementDispatchTime.getSeconds() > currentTime.getSeconds()) {
        // somehow we've gone back in time... just ignore it and move on. should get fixed
        // if we wait I guess
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    if (this->m_runtime.currentStatementDispatchTime.getSeconds() == currentTime.getSeconds() && 
        this->m_runtime.currentStatementDispatchTime.getUSeconds() > currentTime.getUSeconds()) {
        // same as above
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    U64 currentUSeconds = currentTime.getSeconds() * 1000000 + currentTime.getUSeconds();
    U64 dispatchUSeconds = this->m_runtime.currentStatementDispatchTime.getSeconds() * 1000000 + 
                           this->m_runtime.currentStatementDispatchTime.getUSeconds();

    U64 timeoutUSeconds = static_cast<U64>(timeout * 1000000.0f);

    if (currentUSeconds - dispatchUSeconds < timeoutUSeconds) {
        // not over timeout
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    return Signal::result_checkStatementTimeout_statementTimeout;
}

}  // namespace Svc
