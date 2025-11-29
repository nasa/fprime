// ======================================================================
// \title  AosDeframerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for AosDeframer component test main function
// ======================================================================

#include "AosDeframerTester.hpp"

TEST(AosDeframer, testDataReturn) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testDataReturn();
}

TEST(AosDeframer, testNominalDeframing) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(AosDeframer, testInvalidScId) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidScId();
}

TEST(AosDeframer, testInvalidVcId) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidVcId();
}

TEST(AosDeframer, testInvalidLengthToken) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidLengthToken();
}

TEST(AosDeframer, testInvalidCrc) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidCrc();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
