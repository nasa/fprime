#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <FpConfig.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Time::TimeTesterGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester() : Time::TimeTesterGTestBase("comp",10) {
#else
        ATester() : Time::TimeTesterGTestBase(10)  {
#endif
    }

};

int main(int argc, char* argv[]) {

    ATester testBase;

}
