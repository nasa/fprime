#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
namespace Svc {

void FpySequencer::dispatchStatement() {
    if (this->m_runtime.nextStatementIndex == this->m_sequenceObj.getheader().getstatementCount()) {
        this->sequencer_sendSignal_result_dispatchStatement_noMoreStatements();
        return;
    }

    // check to make sure no array out of bounds
    FW_ASSERT(this->m_runtime.nextStatementIndex < this->m_sequenceObj.getheader().getstatementCount());

    const Fpy::Statement& nextStatement = this->m_sequenceObj.getstatements()[this->m_runtime.nextStatementIndex];
    this->m_runtime.nextStatementIndex++;
    this->m_runtime.currentStatementOpcode = nextStatement.getopCode();

    bool result;

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    if (nextStatement.gettype() == Fpy::StatementType::DIRECTIVE) {
        // the problem is that this is both parsing and completely executing
        // the directive. i think we need to split it up into parsing and
        // executing. by the time this line executes, the directive could be completely done
        result = this->dispatchDirective(nextStatement);
    } else {
        // whereas this one just sends the cmd out. there's no chance we see the signal
        // come in until this func finishes, cuz it has to go on the queue
        result = this->dispatchCommand(nextStatement);
    }

    if (result) {
        this->m_tlm.statementsDispatched++;
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

    // little note--theoretically this could produce a cmdResponse before we send the
    // dispatchSuccess signal. however b/c of priorities the dispatchSuccess signal will
    // always get processed first, leaving us in the right state for the cmdresponse
    this->cmdOut_out(0, cmdBuf, 0);

    return true;
}

bool FpySequencer::dispatchDirective(const Fpy::Statement& stmt) {
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
                return false;
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
                return false;
            }
            this->directive_waitAbs_internalInterfaceInvoke(directive);
            break;
        }
        default: {
            // unsure what this opcode is. check compiler version matches sequencer
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.getopCode());
            return false;
        }
    }
    return true;
}

}  // namespace Svc