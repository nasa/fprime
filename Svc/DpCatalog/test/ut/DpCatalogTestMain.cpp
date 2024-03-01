// ======================================================================
// \title  DpCatalogTestMain.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test main function
// ======================================================================

#include "DpCatalogTester.hpp"

TEST(Nominal, initTest) {
    Svc::DpCatalogTester tester;
    tester.doInit();
}

TEST(Nominal, OneDp) {

    Svc::DpCatalogTester tester;
    Fw::String dir = "./";

    Svc::DpCatalogTester::DpSet dpSet;
    dpSet.id = 0x123;
    dpSet.prio = 10;
    dpSet.state = Fw::DpState::UNTRANSMITTED;
    dpSet.time.set(1000,100);
    dpSet.dataSize = 100;

    tester.readDps(
        &dir,
        1,
        &dpSet,
        1
    );
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
