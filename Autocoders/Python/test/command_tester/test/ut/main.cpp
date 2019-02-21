#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <Fw/Cfg/Config.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Cmd::CommandTesterGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : Cmd::CommandTesterGTestBase("comp",10) {
#else
        ATester(void) : Cmd::CommandTesterGTestBase(10)  {
#endif
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
