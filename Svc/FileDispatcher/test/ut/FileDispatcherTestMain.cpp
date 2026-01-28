// ======================================================================
// \title  FileDispatcherTestMain.cpp
// \author tcanham
// \brief  cpp file for FileDispatcher component test main function
// ======================================================================

#include "FileDispatcherTester.hpp"

TEST(Nominal, DispatchTest) {
    Svc::FileDispatcherTester tester;
    tester.dispatchTest();
}

TEST(Nominal, DispatchAllDisabledTest) {
    Svc::FileDispatcherTester tester;
    tester.dispatchAllDisabledTest();
}

TEST(Nominal, DispatchAllCmdDisabledTest) {
    Svc::FileDispatcherTester tester;
    tester.dispatchAllCmdDisabledTest();
}

TEST(Nominal, DispatchAllCmdEnabledTest) {
    Svc::FileDispatcherTester tester;
    tester.dispatchAllCmdEnabledTest();
}

TEST(Nominal, DispatchPingTest) {
    Svc::FileDispatcherTester tester;
    tester.dispatchPingTest();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
