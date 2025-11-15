// ======================================================================
// \title  DpCatalogTestMain.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test main function
// ======================================================================

#include <Svc/DpCatalog/DpCatalog.hpp>
#include <cstdio>
#include <list>
#include "DpCatalogTester.hpp"

TEST(NominalManual, initTest) {
    Svc::DpCatalogTester tester;
    tester.doInit();
}

TEST(NominalManual, TreeTestManual1) {
    Svc::DpCatalogTester tester;
    tester.test_TreeTestManual1();
}

TEST(NominalManual, TreeTestManual2) {
    Svc::DpCatalogTester tester;
    tester.test_TreeTestManual2();
}

TEST(NominalManual, TreeTestManual3) {
    Svc::DpCatalogTester tester;
    tester.test_TreeTestManual3();
}

TEST(NominalManual, TreeTestManual5) {
    Svc::DpCatalogTester tester;
    tester.test_TreeTestManual5();
}

TEST(NominalManual, TreeTestRandom) {
    Svc::DpCatalogTester tester;
    tester.test_TreeTestRandomPriority();
    tester.test_TreeTestRandomTime();
    tester.test_TreeTestRandomId();
    tester.test_TreeTestRandomPrioIdTime();
}

TEST(NominalManual, TreeTestRandomTransmitted) {
    Svc::DpCatalogTester tester;
    tester.test_TreeTestRandomTransmitted();
}

TEST(NominalManual, OneDp) {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dir;
    dir = "./DpTest_OneDp";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");

    Svc::DpCatalogTester::DpSet dpSet;
    dpSet.id = 0x123;
    dpSet.prio = 10;
    dpSet.state = Fw::DpState::UNTRANSMITTED;
    dpSet.time.set(1000, 100);
    dpSet.dataSize = 100;
    dpSet.dir = dir.toChar();

    tester.readDps(&dir, 1, stateFile, &dpSet, 1);
}

TEST(NominalManual, FiveDp) {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dirs[2];
    dirs[0] = "./DpTest_FiveDp_1";
    dirs[1] = "./DpTest_FiveDp_2";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");

    Svc::DpCatalogTester::DpSet dpSet[5];

    dpSet[0].id = 123;
    dpSet[0].prio = 10;
    dpSet[0].state = Fw::DpState::UNTRANSMITTED;
    dpSet[0].time.set(1000, 100);
    dpSet[0].dataSize = 100;
    dpSet[0].dir = dirs[0].toChar();

    dpSet[1].id = 234;
    dpSet[1].prio = 12;
    dpSet[1].state = Fw::DpState::UNTRANSMITTED;
    dpSet[1].time.set(2000, 200);
    dpSet[1].dataSize = 50;
    dpSet[1].dir = dirs[1].toChar();

    dpSet[2].id = 1000000;
    dpSet[2].prio = 3;
    dpSet[2].state = Fw::DpState::UNTRANSMITTED;
    dpSet[2].time.set(3000, 300);
    dpSet[2].dataSize = 200;
    dpSet[2].dir = dirs[0].toChar();

    dpSet[3].id = 2;
    dpSet[3].prio = 255;
    dpSet[3].state = Fw::DpState::UNTRANSMITTED;
    dpSet[3].time.set(1, 500);
    dpSet[3].dataSize = 300;
    dpSet[3].dir = dirs[1].toChar();

    dpSet[4].id = 0x98765432;
    dpSet[4].prio = 17;
    dpSet[4].state = Fw::DpState::UNTRANSMITTED;
    dpSet[4].time.set(1000, 100);
    dpSet[4].dataSize = 2;
    dpSet[4].dir = dirs[0].toChar();

    tester.readDps(dirs, 2, stateFile, dpSet, 5);
}

TEST(NominalManual, TwoDp_OneNotify) {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dir;
    dir = "./DpTest_TwoDp";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");

    Svc::DpCatalogTester::DpSet dpSet[2];

    dpSet[0].id = 123;
    dpSet[0].prio = 10;
    dpSet[0].state = Fw::DpState::UNTRANSMITTED;
    dpSet[0].time.set(1000, 100);
    dpSet[0].dataSize = 100;
    dpSet[0].dir = dir.toChar();

    dpSet[1].id = 234;
    dpSet[1].prio = 12;
    dpSet[1].state = Fw::DpState::UNTRANSMITTED;
    dpSet[1].time.set(2000, 200);
    dpSet[1].dataSize = 50;
    dpSet[1].dir = dir.toChar();

    tester.readDps(&dir, 1, stateFile, dpSet, 2, 1);
}

TEST(NominalManual, SixDp_ThreeNotify) {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dirs[2];
    dirs[0] = "./DpTest_SixDp_1";
    dirs[1] = "./DpTest_SixDp_2";
    Fw::FileNameString stateFile("./DpTest/DpTest/dpState.dat");

    Svc::DpCatalogTester::DpSet dpSet[6];

    dpSet[0].id = 123;
    dpSet[0].prio = 10;
    dpSet[0].state = Fw::DpState::UNTRANSMITTED;
    dpSet[0].time.set(1000, 100);
    dpSet[0].dataSize = 100;
    dpSet[0].dir = dirs[0].toChar();

    dpSet[1].id = 234;
    dpSet[1].prio = 12;
    dpSet[1].state = Fw::DpState::UNTRANSMITTED;
    dpSet[1].time.set(2000, 200);
    dpSet[1].dataSize = 50;
    dpSet[1].dir = dirs[1].toChar();

    dpSet[2].id = 1000000;
    dpSet[2].prio = 3;
    dpSet[2].state = Fw::DpState::UNTRANSMITTED;
    dpSet[2].time.set(3000, 300);
    dpSet[2].dataSize = 200;
    dpSet[2].dir = dirs[0].toChar();

    dpSet[3].id = 2;
    dpSet[3].prio = 255;
    dpSet[3].state = Fw::DpState::UNTRANSMITTED;
    dpSet[3].time.set(1, 500);
    dpSet[3].dataSize = 300;
    dpSet[3].dir = dirs[1].toChar();

    dpSet[4].id = 0x98765432;
    dpSet[4].prio = 17;
    dpSet[4].state = Fw::DpState::UNTRANSMITTED;
    dpSet[4].time.set(1000, 100);
    dpSet[4].dataSize = 2;
    dpSet[4].dir = dirs[0].toChar();

    dpSet[5].id = 17278;
    dpSet[5].prio = 55;
    dpSet[5].state = Fw::DpState::UNTRANSMITTED;
    dpSet[5].time.set(50, 50);
    dpSet[5].dataSize = 300;
    dpSet[5].dir = dirs[1].toChar();

    tester.readDps(dirs, 2, stateFile, dpSet, 6, 3);
}

TEST(NominalManual, SixDp_ThreeStop) {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dirs[2];
    dirs[0] = "./DpTest_SixDp_1";
    dirs[1] = "./DpTest_SixDp_2";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");

    Svc::DpCatalogTester::DpSet dpSet[6];

    dpSet[0].id = 123;
    dpSet[0].prio = 10;
    dpSet[0].state = Fw::DpState::UNTRANSMITTED;
    dpSet[0].time.set(1000, 100);
    dpSet[0].dataSize = 100;
    dpSet[0].dir = dirs[0].toChar();

    dpSet[1].id = 234;
    dpSet[1].prio = 12;
    dpSet[1].state = Fw::DpState::UNTRANSMITTED;
    dpSet[1].time.set(2000, 200);
    dpSet[1].dataSize = 50;
    dpSet[1].dir = dirs[1].toChar();

    dpSet[2].id = 1000000;
    dpSet[2].prio = 3;
    dpSet[2].state = Fw::DpState::UNTRANSMITTED;
    dpSet[2].time.set(3000, 300);
    dpSet[2].dataSize = 200;
    dpSet[2].dir = dirs[0].toChar();

    dpSet[3].id = 2;
    dpSet[3].prio = 255;
    dpSet[3].state = Fw::DpState::UNTRANSMITTED;
    dpSet[3].time.set(1, 500);
    dpSet[3].dataSize = 300;
    dpSet[3].dir = dirs[1].toChar();

    dpSet[4].id = 0x98765432;
    dpSet[4].prio = 17;
    dpSet[4].state = Fw::DpState::UNTRANSMITTED;
    dpSet[4].time.set(1000, 100);
    dpSet[4].dataSize = 2;
    dpSet[4].dir = dirs[0].toChar();

    dpSet[5].id = 17278;
    dpSet[5].prio = 55;
    dpSet[5].state = Fw::DpState::UNTRANSMITTED;
    dpSet[5].time.set(50, 50);
    dpSet[5].dataSize = 300;
    dpSet[5].dir = dirs[1].toChar();

    tester.readDps(dirs, 2, stateFile, dpSet, 6, 0, 3);
}

TEST(NominalManual, TwoDp_OneStop) {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dir;
    dir = "./DpTest_TwoDp";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");

    Svc::DpCatalogTester::DpSet dpSet[2];

    dpSet[0].id = 123;
    dpSet[0].prio = 10;
    dpSet[0].state = Fw::DpState::UNTRANSMITTED;
    dpSet[0].time.set(1000, 100);
    dpSet[0].dataSize = 100;
    dpSet[0].dir = dir.toChar();

    dpSet[1].id = 234;
    dpSet[1].prio = 12;
    dpSet[1].state = Fw::DpState::UNTRANSMITTED;
    dpSet[1].time.set(2000, 200);
    dpSet[1].dataSize = 50;
    dpSet[1].dir = dir.toChar();

    tester.readDps(&dir, 1, stateFile, dpSet, 2, 0, 1);
}

TEST(NominalManual, RandomDp) {
    Svc::DpCatalogTester tester;
    tester.test_RandomDp();
}

TEST(NominalManual, XmitBeforeInit) {
    Svc::DpCatalogTester tester;
    tester.test_XmitBeforeInit();
}

TEST(NominalManual, StopWarn) {
    Svc::DpCatalogTester tester;
    tester.test_StopWarn();
}

TEST(NominalManual, CompareEntries) {
    Svc::DpCatalogTester tester;
    tester.test_CompareEntries();
}

TEST(NominalManual, PingIn) {
    Svc::DpCatalogTester tester;
    tester.test_PingIn();
}

TEST(NominalManual, BadFileDone) {
    Svc::DpCatalogTester tester;
    tester.test_BadFileDone();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
