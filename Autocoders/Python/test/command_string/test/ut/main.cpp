#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <command_stringGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public AcTest::TestCommandGTestBase {
    public:
        ATester() : AcTest::TestCommandGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
