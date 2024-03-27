// ======================================================================
// \title  DpWriterTestMain.cpp
// \author bocchino
// \brief  cpp file for DpWriter component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/DpWriter/test/ut/Rules/Testers.hpp"
#include "Svc/DpWriter/test/ut/Scenarios/Random.hpp"

namespace Svc {

TEST(BufferSendIn, BufferTooSmallForData) {
    COMMENT("Invoke bufferSendIn with a buffer that is too small to hold the data size specified in the header.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.BufferTooSmallForData();
}

TEST(BufferSendIn, BufferTooSmallForPacket) {
    COMMENT("Invoke bufferSendIn with a buffer that is too small to hold a data product packet.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.BufferTooSmallForPacket();
}

TEST(BufferSendIn, FileOpenError) {
    COMMENT("Invoke bufferSendIn with a file open error.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.FileOpenError();
}

TEST(BufferSendIn, FileWriteError) {
    COMMENT("Invoke bufferSendIn with a file write error.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.FileWriteError();
}

TEST(BufferSendIn, InvalidBuffer) {
    COMMENT("Invoke bufferSendIn with an invalid buffer.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.InvalidBuffer();
}

TEST(BufferSendIn, InvalidHeader) {
    COMMENT("Invoke bufferSendIn with an invalid packet header.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.InvalidHeader();
}

TEST(BufferSendIn, InvalidHeaderHash) {
    COMMENT("Invoke bufferSendIn with a buffer that has an invalid header hash.");
    REQUIREMENT("SVC-DPMANAGER-001");
    BufferSendIn::Tester tester;
    tester.InvalidHeaderHash();
}

TEST(BufferSendIn, OK) {
    COMMENT("Invoke bufferSendIn with nominal input.");
    REQUIREMENT("SVC-DPMANAGER-001");
    REQUIREMENT("SVC-DPMANAGER-002");
    REQUIREMENT("SVC-DPMANAGER-003");
    REQUIREMENT("SVC-DPMANAGER-004");
    REQUIREMENT("SVC-DPMANAGER-005");
    BufferSendIn::Tester tester;
    tester.OK();
}

TEST(CLEAR_EVENT_THROTTLE, OK) {
    COMMENT("Test the CLEAR_EVENT_THROTTLE command.");
    REQUIREMENT("SVC-DPMANAGER-006");
    CLEAR_EVENT_THROTTLE::Tester tester;
    tester.OK();
}

TEST(FileOpenStatus, Error) {
    COMMENT("Set the file open status to an error value.");
    FileOpenStatus::Tester tester;
    tester.Error();
}

TEST(FileOpenStatus, OK) {
    COMMENT("Set the file open status to OP_OK.");
    FileOpenStatus::Tester tester;
    tester.OK();
}

TEST(FileWriteStatus, Error) {
    COMMENT("Set the file write status to an error value.");
    FileWriteStatus::Tester tester;
    tester.Error();
}

TEST(FileWriteStatus, OK) {
    COMMENT("Set the file write status to OP_OK.");
    FileWriteStatus::Tester tester;
    tester.OK();
}

TEST(Scenarios, Random) {
    COMMENT("Random scenario with all rules.");
    REQUIREMENT("SVC-DPMANAGER-001");
    REQUIREMENT("SVC-DPMANAGER-002");
    REQUIREMENT("SVC-DPMANAGER-003");
    REQUIREMENT("SVC-DPMANAGER-004");
    REQUIREMENT("SVC-DPMANAGER-005");
    REQUIREMENT("SVC-DPMANAGER-006");
    const FwSizeType numSteps = 10000;
    Scenarios::Random::Tester tester;
    tester.run(numSteps);
}

TEST(SchedIn, OK) {
    COMMENT("Invoke schedIn with nominal input.");
    REQUIREMENT("SVC-DPMANAGER-006");
    SchedIn::Tester tester;
    tester.OK();
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
