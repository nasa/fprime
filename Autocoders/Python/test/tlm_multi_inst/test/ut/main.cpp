#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <tlm_multi_instGTestBase.hpp>
#endif
// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestTlmTesterBase {
    public:
        ATester() : Tlm::TestTlmTesterBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
