#include "Autocoders/Python/test/testgen/GTestBase.hpp"
#include "Autocoders/Python/test/testgen/GTestBase.cpp"
#include "Autocoders/Python/test/testgen/TesterBase.hpp"
#include "Autocoders/Python/test/testgen/TesterBase.cpp"
    
// Very minimal to test autocoder. Some day they'll be actual unit test code
    
class ATester : public TestGen::TestComponentGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : TestGen::TestComponentGTestBase("comp", 10) {
#else
        ATester(void) : TestGen:TestComponentGTestBase(10) {
#endif
    }
};

class BTester : public TestGen::TestComponentTesterBase {
    public:
#if FW_OBJECT_NAMES == 1
        BTester(void) : TestGen::TestComponentTesterBase("comp", 10) {
#else
        BTester(void) : TestGen::TestComponentTesterBase(10) {
#endif
    }
};

int main(int argc, char* argv[]) {
        
    ATester testBase1;
        
    BTester testBase2;
}
