#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <log_testerGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Log::LogTesterGTestBase {
    public:
        ATester() : Log::LogTesterGTestBase("comp",10) {
    }

  void from_Time_handler(
        const FwIndexType portNum,
        Fw::Time &time
    )
  {
    this->pushFromPortEntry_Time(time);
  }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
