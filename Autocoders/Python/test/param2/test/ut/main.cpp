#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <param2GTestBase.hpp>
#endif
// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestPrmGTestBase {
    public:
        ATester() : Tlm::TestPrmGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
