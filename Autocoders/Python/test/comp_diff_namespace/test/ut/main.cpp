#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <comp_diff_namespaceGTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public StressTest::Components::TestCommandGTestBase {
    public:
        ATester() : StressTest::Components::TestCommandGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
