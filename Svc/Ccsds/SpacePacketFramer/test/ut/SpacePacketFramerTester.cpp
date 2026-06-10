// ======================================================================
// \title  SpacePacketFramerTester.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketFramer component test harness implementation class
// ======================================================================

#include <cstdio>

#include "STest/Random/Random.hpp"
#include "SpacePacketFramerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SpacePacketFramerTester ::SpacePacketFramerTester()
    : SpacePacketFramerGTestBase("SpacePacketFramerTester", SpacePacketFramerTester::MAX_HISTORY_SIZE),
      component("SpacePacketFramer") {
    this->initComponents();
    this->connectPorts();
}

SpacePacketFramerTester ::~SpacePacketFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SpacePacketFramerTester::testComStatusPassthrough() {
    // Simulate a comStatusIn event and check comStatusOut
    Fw::Success status = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, status);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, status);
    this->clearHistory();
    status = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, status);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, status);
}

void SpacePacketFramerTester::testDataReturnPassthrough() {
    // Simulate a dataReturnIn event and check bufferDeallocate_out
    U8 data[8] = {0};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;
    this->invoke_to_dataReturnIn(0, buffer, context);
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferDeallocate->at(0).fwBuffer.getData(), data);
    ASSERT_EQ(this->fromPortHistory_bufferDeallocate->at(0).fwBuffer.getSize(), sizeof(data));
}

void SpacePacketFramerTester::testNominalFraming() {
    // Simulate framing a buffer and check output
    U8 payload[16];
    for (U32 i = 0; i < sizeof(payload); ++i) {
        payload[i] = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));
    }
    Fw::Buffer data(payload, sizeof(payload));
    // Choose a random 11-bit APID
    // Choose from among the set of configured constants for ComCfg::Apid
    const ComCfg::Apid::SerialType selectedIdx =
        static_cast<ComCfg::Apid::SerialType>(STest::Random::startLength(0, ComCfg::Apid::NUM_CONSTANTS));
    // Choose from within the bounds provided by CCSDS and the SerialType.
    // 1. We have to respect the CCSDS bound because of the component under test.
    // 2. We have to respect the SerialType bound because F Prime may not be configured
    // to use CCSDS, but we still want this unit test to be valid, if possible.
    constexpr auto ccsdsBound = static_cast<ComCfg::Apid::SerialType>((1 << 11) - 1);  // 11 bits
    constexpr auto serialTypeBound = std::numeric_limits<ComCfg::Apid::SerialType>::max();
    constexpr auto bound = std::min(ccsdsBound, serialTypeBound);
    // Search through the interval [0, maxApid] until we find a valid APID at the
    // selected index, or we run out of numbers
    ComCfg::Apid::SerialType idx = 0;
    ComCfg::Apid::SerialType apid = 0;
    for (ComCfg::Apid::SerialType candidateApid = 0; candidateApid <= bound; candidateApid++) {
        if (ComCfg::Apid::isValid(candidateApid)) {
            // Found a valid APID: store it
            apid = candidateApid;
            if (idx == selectedIdx) {
                // We are at the selected index: done
                break;
            }
            // Not yet at the selected index: keep going
            // We'll either go onto the next valid APID or use the current one
            // if we run off the end of the 11-bit range
            idx++;
        }
    }
    // Print out the apid we found, for diagnostic and debugging purposes
    printf("apid=%0X\n", apid);
    fflush(stdout);
    // If the APID we found is not valid, then skip the test
    // This can happen if all of the configured APIDs are out of the 11-bit range
    // required by CCSDS
    if (!ComCfg::Apid::isValid(apid)) {
        GTEST_SKIP() << "Could not find a valid 11-bit APID\n";
    }
    // Choose a random 14-bit sequence count
    U16 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0x3FFF));
    ComCfg::FrameContext context;
    context.set_apid(static_cast<ComCfg::Apid::T>(apid));
    this->m_nextSeqCount = seqCount;  // seqCount to be returned by getApidSeqCount output port

    this->invoke_to_dataIn(0, data, context);

    // Check dataOut
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), sizeof(payload) + SpacePacketHeader::SERIALIZED_SIZE);
    // Check that the payload is present at the correct offset
    for (U32 i = 0; i < sizeof(payload); ++i) {
        ASSERT_EQ(outBuffer.getData()[SpacePacketHeader::SERIALIZED_SIZE + i], payload[i]);
    }
    // Check that dataReturnOut is called for the original buffer
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), payload);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(payload));
}

void SpacePacketFramerTester ::testOversizedAllocatorBufferIsTrimmed() {
    U8 payload[16];
    for (U32 i = 0; i < sizeof(payload); ++i) {
        payload[i] = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));
    }
    Fw::Buffer data(payload, sizeof(payload));
    ComCfg::FrameContext context;
    context.set_apid(static_cast<ComCfg::Apid::T>(0x01));
    this->m_nextSeqCount = 0;

    // Signal the allocator handler to return a larger-than-needed buffer,
    // simulating a BufferManager bin that is bigger than the exact packet size.
    this->m_useOversizedAlloc = true;
    this->invoke_to_dataIn(0, data, context);
    this->m_useOversizedAlloc = false;

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);

    const FwSizeType expectedFrameSize = sizeof(payload) + SpacePacketHeader::SERIALIZED_SIZE;

    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    // If setSize() is missing from SpacePacketFramer, getSize() returns the
    // oversized allocation (2 * expectedFrameSize) and this assertion fails.
    ASSERT_EQ(outBuffer.getSize(), expectedFrameSize);
}

// ----------------------------------------------------------------------
// Output port handler overrides
// ----------------------------------------------------------------------

U16 SpacePacketFramerTester ::from_getApidSeqCount_handler(FwIndexType portNum,
                                                           const ComCfg::Apid& apid,
                                                           U16 sequenceCount) {
    return this->m_nextSeqCount;
}

Fw::Buffer SpacePacketFramerTester ::from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) {
    FwSizeType allocation = (this->m_useOversizedAlloc) ? sizeof(this->m_internalDataBuffer) : size;
    return Fw::Buffer(this->m_internalDataBuffer, allocation);
}

}  // namespace Ccsds

}  // namespace Svc
