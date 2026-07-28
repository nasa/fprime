typedef unsigned int U32;
typedef U32 FwOpcodeType;

namespace Fw {
enum CmdResponse { OK, EXECUTION_ERROR };
}

class Component {
  public:
    // Violation of cpp/fprime/command-response: the true branch returns
    // without any command response.
    void NO_RESPONSE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        if (this->m_busy) {
            return;
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::OK);
    }

    // Violation: opCode and cmdSeq are saved, but no cmdResponse_out call
    // ever reads the saved members, so the response is never deferred.
    void SAVED_UNREAD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        this->m_lostOpCode = opCode;
        this->m_lostCmdSeq = cmdSeq;
    }

    // Compliant: every path calls cmdResponse_out inline.
    void INLINE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        if (this->m_busy) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::EXECUTION_ERROR);
            return;
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::OK);
    }

    // Compliant: deferred command response pattern. opCode and cmdSeq are
    // stored in member variables that a later cmdResponse_out call reads.
    void DEFERRED_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        if (this->m_busy) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::EXECUTION_ERROR);
            return;
        }
        this->m_opCode = opCode;
        this->m_cmdSeq = cmdSeq;
        this->m_busy = true;
    }

    void completeDeferred() {
        this->m_busy = false;
        this->cmdResponse_out(this->m_opCode, this->m_cmdSeq, Fw::OK);
    }

  private:
    void cmdResponse_out(FwOpcodeType opCode, U32 cmdSeq, Fw::CmdResponse response);

    bool m_busy = false;
    FwOpcodeType m_opCode = 0;
    U32 m_cmdSeq = 0;
    FwOpcodeType m_lostOpCode = 0;
    U32 m_lostCmdSeq = 0;
};
