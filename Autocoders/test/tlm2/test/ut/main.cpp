#include <GTestBase.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Tlm::TestTlmGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : Tlm::TestTlmGTestBase("comp",10) {
#else
        ATester(void) : Tlm::TestTlmGTestBase(10)  {
#endif
    }
};

int main(int argc, char* argv[]) {

    ATester testBase;

}
