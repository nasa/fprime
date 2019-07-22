#include "Autocoders/Python/test/testgen/GTestBase.hpp"
#include "Autocoders/Python/test/testgen/GTestBase.cpp"
#include "Autocoders/Python/test/testgen/TesterBase.hpp"
#include "Autocoders/Python/test/testgen/TesterBase.cpp"
#include "Autocoders/Python/test/testgen/Tester.cpp"
#include "Autocoders/Python/test/testgen/Tester.hpp"
#include <Autocoders/Python/test/testgen/TestComponentComponentImpl.hpp>
    
// Very minimal to test autocoder. Some day they'll be actual unit test code
    
class ATester : public TestGen::TestComponentGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : TestGen::TestComponentGTestBase("comp", 10) {
#else
        ATester(void) : TestGen:TestComponentGTestBase(10) {
#endif
    }
            
    void from_bportOut_handler(const NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6) {
        printf("Test Out Handler");
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
            
    void from_bportOut_handler(const NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6) {
        printf("Test Out Handler");
    }
};
    
TEST(Nominal, ToDoTest) {
    TestGen::Tester tester;
    tester.toDo();
}

int main(int argc, char* argv[]) {
        
    ATester testBase1;
        
    BTester testBase2;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
