#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <time_testerGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Time::TimeTesterGTestBase {
    public:
        ATester() : Time::TimeTesterGTestBase("comp",10) {
    }

};

int main(int argc, char* argv[]) {

    ATester testBase;

}
