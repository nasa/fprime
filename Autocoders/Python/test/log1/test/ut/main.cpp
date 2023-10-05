#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <log1GTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestEventGTestBase {
    public:
        ATester() : Tlm::TestEventGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
