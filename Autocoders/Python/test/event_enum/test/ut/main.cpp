#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <event_enumGTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Somewhere::TestLogGTestBase {
    public:
        ATester() : Somewhere::TestLogGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
