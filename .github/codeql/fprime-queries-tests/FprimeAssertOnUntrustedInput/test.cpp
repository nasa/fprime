typedef unsigned int U32;
typedef unsigned char U8;
namespace Fw {
void SwAssert(const char* file, U32 line);
enum CmdResponse { CMD_OK, CMD_VALIDATION_ERROR };
}  // namespace Fw
#define FW_ASSERT(cond, ...) ((cond) ? (void)0 : Fw::SwAssert(__FILE__, __LINE__))

const U32 MAX_CHANNEL = 8;

class Component {
  public:
    void SET_CHANNEL_cmdHandler(U32 opCode, U32 cmdSeq, U32 channel);
    void SET_GAIN_cmdHandler(U32 opCode, U32 cmdSeq, U32 gain);
    void ARM_cmdHandler(U32 opCode, U32 cmdSeq);
    void applyParam();
    void dataIn_handler(U32 portNum, U8 value);

  private:
    void cmdResponse_out(U32 opCode, U32 cmdSeq, Fw::CmdResponse r);
    U32 paramGet_THRESHOLD(bool& valid);
    U32 m_channel;
};

// Violation of cpp/fprime/assert-on-untrusted-input: command argument asserted
void Component::SET_CHANNEL_cmdHandler(U32 opCode, U32 cmdSeq, U32 channel) {
    FW_ASSERT(channel < MAX_CHANNEL, channel);
    this->m_channel = channel;
    this->cmdResponse_out(opCode, cmdSeq, Fw::CMD_OK);
}

// Compliant: validated and reported
void Component::SET_GAIN_cmdHandler(U32 opCode, U32 cmdSeq, U32 gain) {
    if (gain >= MAX_CHANNEL) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CMD_VALIDATION_ERROR);
        return;
    }
    U32 local = gain;
    FW_ASSERT(this->m_channel < MAX_CHANNEL, this->m_channel);  // Compliant: internal state
    this->m_channel = local;
    this->cmdResponse_out(opCode, cmdSeq, Fw::CMD_OK);
}

// Compliant: opCode/cmdSeq are framework bookkeeping
void Component::ARM_cmdHandler(U32 opCode, U32 cmdSeq) {
    FW_ASSERT(opCode != 0, opCode);
}

// Violation of cpp/fprime/assert-on-untrusted-input: parameter value asserted via local
void Component::applyParam() {
    bool valid = false;
    U32 threshold = this->paramGet_THRESHOLD(valid);
    FW_ASSERT(threshold > 0, threshold);
}
