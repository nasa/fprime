#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

void FpySequencer::stepStatement() {
    // conops:
    // check should cancel
    // check no more statements
    // get next statement
    // dispatch that statement

    if (m_runtime.cancelNextStatement) {
        this->sequencer_sendSignal_result_stepStatement_cancelled();
        return;
    }

    if (m_runtime.statementIndex == m_sequenceObj.getheader().getstatementCount()) {
        this->sequencer_sendSignal_result_stepStatement_noMoreStatements();
        return;
    }

    // check to make sure no array out of bounds
    FW_ASSERT(m_runtime.statementIndex < m_sequenceObj.getheader().getstatementCount());
    Fpy::Statement nextStatement = m_sequenceObj.getstatements()[m_runtime.statementIndex];

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    bool isDirective = checkOpcodeIsDirective(nextStatement.getopCode());
    bool result = false;

    if (isDirective) {
        result = dispatchDirective(nextStatement);
    } else {
        result = dispatchCommand(nextStatement);
    }

    m_runtime.statementIndex++;

    m_runtime.currentStatementOpcode = nextStatement.getopCode();

    if (result) {
        this->sequencer_sendSignal_result_stepStatement_success();
    } else {
        this->sequencer_sendSignal_result_stepStatement_failure();
    }
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
    stat = cmdBuf.serialize(stmt);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            sizeof(stmt.getopCode()) + sizeof(stmt.getargs().getBuffLength()), stat);
        return false;
    }
    this->cmdOut_out(0, cmdBuf, 0);
    return true;
}

void FpySequencer::handleStatementResult(FwOpcodeType opCode,             //!< Command Op Code
                                         const Fw::CmdResponse& response  //!< The command response argument
) {
    if (opCode != this->m_runtime.currentStatementOpcode) {
        // just got an opcode back for a cmd that we didn't expect
        this->log_WARNING_LO_UnexpectedStatementResponseOpcode(m_runtime.currentStatementOpcode, opCode, response);
        // keep on waiting for the one we're looking for...
        return;
    }

    // okay got a response back for our cmd
    // clear the opcode we're currently executing
    m_runtime.currentStatementOpcode = Fpy::DirectiveId::INVALID;
    // send signal that we got a response
    this->sequencer_sendSignal_statementResponseIn(FpySequencer_StatementResponse(opCode, response));
}

bool FpySequencer::checkOpcodeIsDirective(FwOpcodeType opcode) {
    return opcode < Fpy::DirectiveId::MAX_DIRECTIVE_ID;
}

bool FpySequencer::dispatchDirective(const Fpy::Statement& stmt) {
    switch (stmt.opcode) {
        case Fpy::DirectiveId::WAIT_REL: {
            return handleDirective_WAIT_REL(stmt);
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            return handleDirective_WAIT_ABS(stmt);
        }
        default: {
            // unsure what this opcode is. check compiler version?
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.opcode);
            return false;
        }
    }
}

bool FpySequencer::handleDirective_WAIT_REL(const Fpy::Statement& stmt) {
    Fw::Time currentTime = getTime();
    Fw::Time duration;
    Fw::ExternalSerializeBuffer deser(const_cast<U8*>(stmt.args.getBuffAddr()), stmt.args.getBuffLength());
    Fw::SerializeStatus stat = deser.deserialize(duration);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DirectiveDeserializeError(stmt.opcode, stat, deser.getBuffLeft(), deser.getBuffLength());
        return false;
    }

    sleepUntil(Fw::Time::add(currentTime, duration));
    return true;
}

bool FpySequencer::handleDirective_WAIT_ABS(const Fpy::Statement& stmt) {
    Fw::Time wakeupTime;
    Fw::ExternalSerializeBuffer deser(const_cast<U8*>(stmt.args.getBuffAddr()), stmt.args.getBuffLength());
    Fw::SerializeStatus stat = deser.deserialize(wakeupTime);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DirectiveDeserializeError(stmt.opcode, stat, deser.getBuffLeft(), deser.getBuffLength());
        return false;
    }

    sleepUntil(wakeupTime);
    return true;
}

// pause returning a directive response until the given
// absolute time
void FpySequencer::sleepUntil(const Fw::Time& time) {
    FW_ASSERT(!m_runtime.sleeping);  // already sleeping!! should be impossible to start another sleep

    m_runtime.sleeping = true;
    m_runtime.wakeupTime = time;
}

// checks whether we are still sleeping, and if we are no
// longer sleeping, returns a directive response
void FpySequencer::checkShouldWakeUp() {
    if (!m_runtime.sleeping) {
        // we are not sleeping
        return;
    }

    // okay, we are sleeping

    Fw::Time currentTime = getTime();

    if (currentTime < m_runtime.wakeupTime) {
        // not time to wake up!
        return;
    }

    // okay, time to wake up
    m_runtime.sleeping = false;
    m_runtime.wakeupTime = Fw::Time();
}
}  // namespace Svc