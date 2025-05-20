// ======================================================================
// \title  TCDeframerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for TCDeframer component test main function
// ======================================================================

#include "TCDeframerTester.hpp"

TEST(TCDeframer, testDataReturn) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testDataReturn();
}

TEST(TCDeframer, testNominalDeframing) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(TCDeframer, testInvalidScId) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testInvalidScId();
}

TEST(TCDeframer, testInvalidVcId) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testInvalidVcId();
}

TEST(TCDeframer, testInvalidLengthToken) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testInvalidLengthToken();
}

TEST(TCDeframer, testInvalidSequenceNumber) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testInvalidSequenceNumber();
}

TEST(TCDeframer, testInvalidCrc) {
    Svc::CCSDS::TCDeframerTester tester;
    tester.testInvalidCrc();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
