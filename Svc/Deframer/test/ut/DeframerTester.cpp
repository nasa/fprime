// ======================================================================
// \title  DeframerTester.cpp
// \author thomas-bc
// \brief  cpp file for Deframer component test harness implementation class
// ======================================================================

#include "DeframerTester.hpp"
#include "STest/Random/Random.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

DeframerTester ::DeframerTester()
    : DeframerGTestBase("DeframerTester", DeframerTester::MAX_HISTORY_SIZE), component("Deframer") {
    this->initComponents();
    this->connectPorts();
}

DeframerTester ::~DeframerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void DeframerTester ::testNominalFrame() {
    // TODO: make this test multiple times with different random bytes and lengths
    // Get random byte of data
    U8 randomByte = STest::Random::lowerUpper(0, 255);
    //           |  F´ start word        |     Length (= 1)      |   Data     |   Checksum (4 bytes)   |
    U8 data[13] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0x00, 0x01,  randomByte,  0x00, 0x00, 0x00, 0x00};
    this->mockReceiveData(data, sizeof(data));

    // Assert that something was emitted on the deframedOut port
    ASSERT_from_deframedOut_SIZE(1);
    // Assert that the data that was emitted on deframedOut is equal to Data field above (randomByte)
    ASSERT_EQ(this->fromPortHistory_deframedOut->at(0).data.getData()[0], randomByte);
}

void DeframerTester::testTruncatedFrame() {
    // Send a truncated frame, too short to be valid
    U8 data[11] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    ASSERT_DEATH(this->mockReceiveData(data, sizeof(data)), "Deframer.cpp");
    ASSERT_from_deframedOut_SIZE(0);
}

void DeframerTester::testZeroSizeFrame() {
    // Send a null frame, too short to be valid
    U8 data[0] = {};
    ASSERT_DEATH(this->mockReceiveData(data, sizeof(data)), "Deframer.cpp");
    ASSERT_from_deframedOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Test Helpers
// ----------------------------------------------------------------------

void DeframerTester::mockReceiveData(U8* data, FwSizeType size) {
    Fw::Buffer nullContext;
    Fw::Buffer buffer;
    buffer.setData(data);
    buffer.setSize(size);
    this->invoke_to_framedIn(0, buffer, nullContext);
}

}  // namespace Svc
