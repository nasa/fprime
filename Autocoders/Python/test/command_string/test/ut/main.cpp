#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <Fw/Cfg/Config.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public AcTest::TestCommandGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : AcTest::TestCommandGTestBase("comp",10) {
#else
        ATester(void) : AcTest::TestCommandGTestBase(10)  {
#endif
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
