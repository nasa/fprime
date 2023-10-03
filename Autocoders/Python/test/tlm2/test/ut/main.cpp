#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <tlm2GTestBase.hpp>
#endif
// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestTlmGTestBase {
    public:
        ATester() : Tlm::TestTlmGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
