#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <command_multi_instGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Cmd::Test1GTestBase {
    public:
        ATester() : Cmd::Test1GTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
