#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <interface1GTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Cmd::TestGTestBase {
    public:
        ATester() : Cmd::TestGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
