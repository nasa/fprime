#ifdef FPRIME_CMAKE
#include <GTestBase.hpp>
#else
#include "Autocoder/GTestBase.hpp"
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public StressTest::Components::TestCommandGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : StressTest::Components::TestCommandGTestBase("comp",10) {
#else
        ATester(void) : StressTest::Components::TestCommandGTestBase(10)  {
#endif
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
