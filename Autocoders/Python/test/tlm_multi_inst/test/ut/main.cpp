#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif
// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestTlmTesterBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : Tlm::TestTlmTesterBase("comp",10) {
#else
        ATester(void) : Tlm::TestTlmTesterBase(10)  {
#endif
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
