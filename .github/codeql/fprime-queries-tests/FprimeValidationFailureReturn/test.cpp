typedef unsigned int U32;
typedef int NATIVE_INT_TYPE;
typedef unsigned int FwOpcodeType;
typedef unsigned int U16;

namespace Fw {
enum CmdResponse { COMMAND_OK, COMMAND_VALIDATION_ERROR };
}

class Component {
  public:
    void START_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 value);
    void STOP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 value);
    void dataIn_handler(NATIVE_INT_TYPE portNum, U32 value);

  private:
    void cmdResponse_out(FwOpcodeType opCode, U32 cmdSeq, Fw::CmdResponse response);
    void log_WARNING_HI_InvalidValue(U32 value);
    void process(U32 value);
};

// Violation of cpp/fprime/validation-failure-return: a failed command
// response is sent but execution continues.
void Component::START_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 value) {
    if (value > 100) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    this->process(value);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
}

// Compliant: the failure branch returns.
void Component::STOP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 value) {
    if (value > 100) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
        return;
    }
    this->process(value);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
}

// Compliant: warning-only branches are not flagged.
void Component::dataIn_handler(NATIVE_INT_TYPE portNum, U32 value) {
    if (value > 100) {
        this->log_WARNING_HI_InvalidValue(value);
    }
    this->process(value);
}
