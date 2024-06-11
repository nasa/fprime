#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <telem_testerGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TelemTesterTesterBase {
    public:
        ATester() : Tlm::TelemTesterTesterBase("comp",10) {
    }

};

int main(int argc, char* argv[]) {

    ATester testBase;

}
