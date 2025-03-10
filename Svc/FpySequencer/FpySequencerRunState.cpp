#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
namespace Svc {

void FpySequencer::dispatchStatement() {
    // conops:
    // check should cancel
    // check no more statements
    // get next statement
    // dispatch that statement

    if (m_runtime.cancelNextStatement) {
        this->sequencer_sendSignal_result_dispatchStatement_cancelled();
        return;
    }

    if (m_runtime.nextStatementIndex == m_sequenceObj.getheader().getstatementCount()) {
        this->sequencer_sendSignal_result_dispatchStatement_noMoreStatements();
        return;
    }

    // check to make sure no array out of bounds
    FW_ASSERT(m_runtime.nextStatementIndex < m_sequenceObj.getheader().getstatementCount());

    Fpy::Statement nextStatement = m_sequenceObj.getstatements()[m_runtime.nextStatementIndex];
    m_runtime.nextStatementIndex++;
    m_runtime.currentStatementOpcode = nextStatement.getopCode();

    bool result;

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    if (nextStatement.gettype() == Fpy::StatementType::DIRECTIVE) {
        // the problem is that this is both parsing and completely executing
        // the directive. i think we need to split it up into parsing and
        // executing. by the time this line executes, the directive could be completely done
        result = dispatchDirective(nextStatement);
    } else {
        // whereas this one just sends the cmd out. there's no chance we see the signal
        // come in until this func finishes, cuz it has to go on the queue
        result = dispatchCommand(nextStatement);
    }

    if (result) {
        m_tlm.statementsDispatched++;
        this->sequencer_sendSignal_result_dispatchStatement_success();
    } else {
        this->sequencer_sendSignal_result_dispatchStatement_failure();
    }
    // was it dispatched successfully?
    // no -> IDLE
    // what state should we go to next?
    // (await cmd, sleep, step statement)
    // cmd -> await cmd
    // wait_rel/abs -> sleep
    // if, goto, etc -> step statement
}

// dispatches a command out via port.
// return true if successfully dispatched.
bool FpySequencer::dispatchCommand(const Fpy::Statement& stmt) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(Fw::ComPacket::FW_PACKET_COMMAND);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            sizeof(Fw::ComPacket::FW_PACKET_COMMAND), stat);
        return false;
    }
    stat = cmdBuf.serialize(stmt.getopCode());
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(), sizeof(stmt.getopCode()),
                                            stat);
        return false;
    }

    stat = cmdBuf.serialize(stmt.getargBuf().getBuffAddr(), stmt.getargBuf().getBuffLength(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            stmt.getargBuf().getBuffLength(), stat);
        return false;
    }

    // this->cmdOut_out(0, cmdBuf, 0);

    // TODO what happens if cmdOut returns a response before we execute this next line?
    // ANSWER: make sm sigs higher prio than cmd response
    // this way we're guaranteed to be in the right state before we process the cmd response

    this->cmdOut_out(0, cmdBuf, 0);

    return true;
}

void FpySequencer::handleStatementResult(FwOpcodeType opCode,             //!< Command Op Code
                                         const Fw::CmdResponse& response  //!< The command response argument
) {
    if (opCode != this->m_runtime.currentStatementOpcode) {
        // just got an opcode back for a cmd that we didn't expect
        this->log_WARNING_LO_WrongStatementResponseOpcode(m_runtime.currentStatementOpcode, opCode, response);
        // keep on waiting for the one we're looking for...
        return;
    }

    // okay got a response back for our cmd
    // clear the opcode we're currently executing
    m_runtime.currentStatementOpcode = Fpy::DirectiveId::INVALID;
    // send signal that we got a response
}

bool FpySequencer::dispatchDirective(Fpy::Statement& stmt) {
    switch (stmt.getopCode()) {
        case Fpy::DirectiveId::WAIT_REL: {
            FpySequencer_WaitRelDirective directive;
            stmt.getargBuf().deserialize(directive);
            directive_waitRel_internalInterfaceInvoke(directive);
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            FpySequencer_WaitAbsDirective directive;
            stmt.getargBuf().deserialize(directive);
            directive_waitAbs_internalInterfaceInvoke(directive);
        }
        default: {
            // unsure what this opcode is. check compiler version?
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.getopCode());
            return false;
        }
    }
    return true;
}

// checks whether we are still sleeping, and if we are no
// longer sleeping, returns a directive response
void FpySequencer::checkShouldWakeUp() {
    if (sequencer_getState() != FpySequencer_SequencerStateMachineStateMachineBase::State::RUNNING_SLEEPING) {
        // we are not sleeping
        return;
    }

    // okay, we are sleeping

    Fw::Time currentTime = getTime();

    if (currentTime.getTimeBase() != m_runtime.wakeupTime.getTimeBase()) {
        // cannot compare these times. break out of sleep with a failure
        m_runtime.wakeupTime = Fw::Time();

        this->log_WARNING_LO_MismatchedTimeBase(currentTime.getTimeBase(), m_runtime.wakeupTime.getTimeBase());

        handleStatementResult(m_runtime.currentStatementOpcode, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    if (currentTime.getContext() != m_runtime.wakeupTime.getContext()) {
        // cannot compare these times. break out of sleep with a failure
        m_runtime.wakeupTime = Fw::Time();

        this->log_WARNING_LO_MismatchedTimeContext(currentTime.getContext(), m_runtime.wakeupTime.getContext());

        handleStatementResult(m_runtime.currentStatementOpcode, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    if (currentTime < m_runtime.wakeupTime) {
        // not time to wake up!
        return;
    }

    // okay, time to wake up
    m_runtime.sleeping = false;
    m_runtime.wakeupTime = Fw::Time();

    // say we've finished our sleep
    handleStatementResult(m_runtime.currentStatementOpcode, Fw::CmdResponse::OK);
}
}  // namespace Svc