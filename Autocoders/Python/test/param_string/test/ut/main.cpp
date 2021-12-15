#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Prm::TestPrmGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester() : Prm::TestPrmGTestBase("comp",10) {
#else
        ATester() : Prm::TestPrmGTestBase(10)  {
#endif
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
