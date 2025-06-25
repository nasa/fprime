// ======================================================================
// \title  TcDeframerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for TcDeframer component test main function
// ======================================================================

#include "TcDeframerTester.hpp"

TEST(TcDeframer, testDataReturn) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testDataReturn();
}

TEST(TcDeframer, testNominalDeframing) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(TcDeframer, testInvalidScId) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidScId();
}

TEST(TcDeframer, testInvalidVcId) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidVcId();
}

TEST(TcDeframer, testInvalidLengthToken) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidLengthToken();
}

TEST(TcDeframer, testInvalidCrc) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidCrc();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
