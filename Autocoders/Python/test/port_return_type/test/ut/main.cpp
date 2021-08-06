#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestEventGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester() : Tlm::TestEventGTestBase("comp",10) {
#else
        ATester() : Tlm::TestEventGTestBase(10)  {
#endif
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
