#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <command_testerGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Cmd::CommandTesterGTestBase {
    public:
        ATester() : Cmd::CommandTesterGTestBase("comp",10) {
    }

    void from_cmdSendPort_handler(
              const NATIVE_INT_TYPE portNum, //!< The port number
              FwOpcodeType opCode, //!< Command Op Code
              U32 cmdSeq, //!< Command Sequence
              Fw::CmdArgBuffer &args //!< Buffer containing arguments
          ) {}
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
