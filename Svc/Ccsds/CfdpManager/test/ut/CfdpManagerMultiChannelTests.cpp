// ======================================================================
// \title  CfdpManagerMultiChannelTests.cpp
// \author Auto-generated
// \brief  cpp file for CfdpManager component multi-channel unit tests
//
// ======================================================================

#include "CfdpManagerTester.hpp"

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Multi-Channel RX Tests
// ----------------------------------------------------------------------
//
// These reuse the full RX verification helpers on a non-zero channel to
// exercise the per-channel port-array indexing in the uplink path. The
// channel-0 nominal RX tests (testClass1RxNominal / testClass2RxNominal)
// cover the same flow on channel 0; passing channelId == 1 here validates
// that index arithmetic and completion behavior are correct off channel 0.

void CfdpManagerTester::testClass1RxNominalChannel1() {
    const U16 fileDataSize = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());

    sendAndVerifyClass1Rx("test/ut/output/test_rx_ch1_source.bin", "test/ut/output/test_rx_ch1_received.bin",
                          "/ground/test_rx_ch1_source.bin", fileDataSize, TEST_CHANNEL_ID_1);
}

void CfdpManagerTester::testClass2RxNominalChannel1() {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    sendAndVerifyClass2Rx("test/ut/output/test_class2_rx_ch1_source.bin",
                          "test/ut/output/test_class2_rx_ch1_received.bin", "/ground/test_class2_rx_ch1_source.bin",
                          expectedFileSize,
                          false,  // No NAK simulation
                          TEST_CHANNEL_ID_1);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
