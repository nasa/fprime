// ======================================================================
// \title  DpCatalogTestMain.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test main function
// ======================================================================

#include "DpCatalogTester.hpp"

TEST(NominalManual, initTest) {
    Svc::DpCatalogTester tester;
    tester.doInit();
}

TEST(NominalManual, OneDp) {

    Svc::DpCatalogTester tester;
    Fw::FileNameString dir;
    dir = "./DpTest";

    Svc::DpCatalogTester::DpSet dpSet;
    dpSet.id = 0x123;
    dpSet.prio = 10;
    dpSet.state = Fw::DpState::UNTRANSMITTED;
    dpSet.time.set(1000,100);
    dpSet.dataSize = 100;
    dpSet.dir = dir.toChar();

    tester.readDps(
        &dir,
        1,
        &dpSet,
        1
    );
}

TEST(NominalManual, FiveDp) {

    Svc::DpCatalogTester tester;
    Fw::FileNameString dirs[2];
    dirs[0] = "./DpTest1";
    dirs[1] = "./DpTest2";

    Svc::DpCatalogTester::DpSet dpSet[5];
    
    dpSet[0].id = 123;
    dpSet[0].prio = 10;
    dpSet[0].state = Fw::DpState::UNTRANSMITTED;
    dpSet[0].time.set(1000,100);
    dpSet[0].dataSize = 100;
    dpSet[0].dir = dirs[0].toChar();

    dpSet[1].id = 234;
    dpSet[1].prio = 12;
    dpSet[1].state = Fw::DpState::UNTRANSMITTED;
    dpSet[1].time.set(2000,200);
    dpSet[1].dataSize = 50;
    dpSet[1].dir = dirs[1].toChar();

    dpSet[2].id = 1000000;
    dpSet[2].prio = 3;
    dpSet[2].state = Fw::DpState::UNTRANSMITTED;
    dpSet[2].time.set(3000,300);
    dpSet[2].dataSize = 200;
    dpSet[2].dir = dirs[0].toChar();

    dpSet[3].id = 2;
    dpSet[3].prio = 255;
    dpSet[3].state = Fw::DpState::UNTRANSMITTED;
    dpSet[3].time.set(1,500);
    dpSet[3].dataSize = 300;
    dpSet[3].dir = dirs[1].toChar();

    dpSet[4].id = 0x98765432;
    dpSet[4].prio = 17;
    dpSet[4].state = Fw::DpState::UNTRANSMITTED;
    dpSet[4].time.set(1000,100);
    dpSet[4].dataSize = 2;
    dpSet[4].dir = dirs[0].toChar();

    tester.readDps(
        dirs,
        2,
        dpSet,
        5
    );
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
