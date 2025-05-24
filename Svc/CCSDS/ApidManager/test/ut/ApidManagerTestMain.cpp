// ======================================================================
// \title  ApidManagerTestMain.cpp
// \author chammard
// \brief  cpp file for ApidManager component test main function
// ======================================================================

#include "ApidManagerTester.hpp"

TEST(ApidManager, getExistingSeqCount) {
    Svc::CCSDS::ApidManagerTester tester;
    Svc::CCSDS::ApidManagerTester::GetExistingSeqCount rule1;
    rule1.apply(tester);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
