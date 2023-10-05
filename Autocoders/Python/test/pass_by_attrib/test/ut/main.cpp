#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <pass_by_attribGTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public App::PassByGTestBase {
    public:
        ATester() : App::PassByGTestBase("comp",10) {
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
