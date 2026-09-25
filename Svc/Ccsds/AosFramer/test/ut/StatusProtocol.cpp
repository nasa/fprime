// Regression coverage for the per-input communications status protocol.
#include <cstring>
#include "AosFramerTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "Svc/ComStub/ComStub.hpp"

namespace Svc {
namespace Ccsds {

void AosFramerTester::from_dataOut_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    AosFramerGTestBase::from_dataOut_handler(portNum, data, context);
    if (!this->m_checkStatusProtocol) {
        return;
    }
    EXPECT_FALSE(this->m_adapterBusy) << "A frame was sent before the previous adapter status";
    this->m_adapterBusy = true;
    EXPECT_EQ(data.getSize(), 64U);
    EXPECT_EQ(this->getFrameVcCount(data.getData()), this->m_protocolFrames);
    EXPECT_EQ(this->getFrameVcId(data.getData()), context.get_vcId());
    ++this->m_protocolFrames;

    const FwSizeType overhead = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;
    const FwSizeType trailer = this->m_protocolFecf ? AOSTrailer::SERIALIZED_SIZE : 0;
    if (this->m_expectedBytes != nullptr) {
        ASSERT_LE(this->m_protocolBytes, this->m_expectedSize);
        const FwSizeType count =
            FW_MIN(data.getSize() - overhead - trailer, this->m_expectedSize - this->m_protocolBytes);
        EXPECT_EQ(::memcmp(data.getData() + overhead, this->m_expectedBytes + this->m_protocolBytes, count), 0);
        this->m_protocolBytes += count;
    }
    if (this->m_protocolFecf) {
        const U16 crc = Utils::CRC16::compute(data.getData(), static_cast<U32>(data.getSize() - trailer));
        EXPECT_EQ(data.getData()[data.getSize() - 2], static_cast<U8>(crc >> 8));
        EXPECT_EQ(data.getData()[data.getSize() - 1], static_cast<U8>(crc));
    }
    if (this->m_synchronousStatus) {
        const Fw::Success status =
            this->m_protocolFrames == this->m_failureFrame ? Fw::Success::FAILURE : Fw::Success::SUCCESS;
        this->completeProtocolFrame(status);
    }
}

void AosFramerTester::from_dataReturnOut_handler(FwIndexType portNum,
                                                 Fw::Buffer& data,
                                                 const ComCfg::FrameContext& context) {
    AosFramerGTestBase::from_dataReturnOut_handler(portNum, data, context);
    if (this->m_checkStatusProtocol) {
        EXPECT_FALSE(this->m_inputReturned) << "Input ownership returned more than once";
        EXPECT_EQ(data.getData(), this->m_expectedInput.getData());
        EXPECT_EQ(data.getSize(), this->m_expectedInput.getSize());
        this->m_inputReturned = true;
    }
}

void AosFramerTester::from_comStatusOut_handler(FwIndexType portNum, Fw::Success& condition) {
    AosFramerGTestBase::from_comStatusOut_handler(portNum, condition);
    if (this->m_checkStatusProtocol) {
        EXPECT_TRUE(this->m_inputReturned) << "Status preceded return of the original input";
        EXPECT_FALSE(this->m_adapterBusy) << "Upstream credit issued while the adapter is busy";
        EXPECT_TRUE(this->m_waitingForStatus) << "Duplicate status after the upstream queue became ready";
        if (condition == Fw::Success::SUCCESS) {
            this->m_waitingForStatus = false;
        }
    }
}

void AosFramerTester::completeProtocolFrame(Fw::Success condition) {
    ASSERT_TRUE(this->m_adapterBusy);
    const U32 framesBefore = this->m_protocolFrames;
    const FwSizeType statusesBefore = this->fromPortHistory_comStatusOut->size();
    // Copy handles before a synchronous callback can overwrite the history or backing buffer.
    Fw::Buffer frame = this->fromPortHistory_dataOut->at(framesBefore - 1).data;
    const ComCfg::FrameContext context = this->fromPortHistory_dataOut->at(framesBefore - 1).context;
    this->invoke_to_dataReturnIn(0, frame, context);
    EXPECT_EQ(this->m_protocolFrames, framesBefore) << "Buffer return must not initiate another transmission";
    EXPECT_EQ(this->fromPortHistory_comStatusOut->size(), statusesBefore)
        << "Buffer return must not generate a transmission status";
    this->m_adapterBusy = false;
    this->invoke_to_comStatusIn(0, condition);
}

void AosFramerTester::sendProtocolPacket(U8* data, FwSizeType size, bool sendNow) {
    Fw::Buffer input(data, size);
    this->m_expectedInput = input;
    this->m_inputReturned = false;
    this->m_waitingForStatus = true;
    ComCfg::FrameContext context;
    context.set_sendNow(sendNow);
    this->invoke_to_dataIn(0, input, context);
}

void AosFramerTester::testTailWaitsForStatus() {
    this->component.configure(64, true);
    this->clearHistory();
    this->m_checkStatusProtocol = true;
    U8 input[60] = {};
    this->sendProtocolPacket(input, sizeof input, false);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(0);
    this->completeProtocolFrame(Fw::Success::SUCCESS);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(Fw::Success::SUCCESS));
}

void AosFramerTester::testStatusProtocol(FwSizeType packetSize, bool sendNow, bool synchronous, bool fecf) {
    this->component.configure(64, fecf);
    this->clearHistory();
    this->m_checkStatusProtocol = true;
    this->m_synchronousStatus = synchronous;
    this->m_protocolFecf = fecf;
    U8 input[256] = {};
    ASSERT_LE(packetSize + 7, sizeof input);
    for (FwSizeType i = 0; i < sizeof input; ++i) {
        input[i] = static_cast<U8>(i);
    }
    this->m_expectedBytes = input;
    this->m_expectedSize = packetSize;
    this->sendProtocolPacket(input, packetSize, sendNow);
    for (U32 i = 0; i < 8 && this->m_adapterBusy; ++i) {
        this->completeProtocolFrame(Fw::Success::SUCCESS);
    }
    EXPECT_FALSE(this->m_adapterBusy);
    const FwSizeType capacity =
        64 - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - (fecf ? AOSTrailer::SERIALIZED_SIZE : 0);
    const FwSizeType frames = (packetSize + (sendNow ? capacity - 1 : 0)) / capacity;
    ASSERT_EQ(this->m_protocolFrames, frames);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(Fw::Success::SUCCESS));
    EXPECT_EQ(this->m_protocolBytes, sendNow ? packetSize : frames * capacity);

    // A later packet must still be able to flush an unpadded tail, in order.
    if (!sendNow) {
        this->m_expectedSize += 7;
        this->sendProtocolPacket(input + packetSize, 7, true);
        for (U32 i = 0; i < 8 && this->m_adapterBusy; ++i) {
            this->completeProtocolFrame(Fw::Success::SUCCESS);
        }
        EXPECT_FALSE(this->m_adapterBusy);
        ASSERT_from_dataReturnOut_SIZE(2);
        ASSERT_from_comStatusOut_SIZE(2);
        ASSERT_from_comStatusOut(1, Fw::Success(Fw::Success::SUCCESS));
        EXPECT_EQ(this->m_protocolBytes, packetSize + 7);
    }
}

void AosFramerTester::testFailureAndRecovery(U32 failureFrame, bool synchronous) {
    this->component.configure(64, true);
    this->clearHistory();
    this->m_checkStatusProtocol = true;
    this->m_synchronousStatus = synchronous;
    this->m_failureFrame = failureFrame;
    U8 input[117] = {};  // Two full 54-byte data zones and a partial third frame.
    this->sendProtocolPacket(input, sizeof input, true);
    for (U32 i = 0; i < 8 && this->m_adapterBusy; ++i) {
        this->completeProtocolFrame(this->m_protocolFrames == failureFrame ? Fw::Success::FAILURE
                                                                           : Fw::Success::SUCCESS);
    }
    EXPECT_FALSE(this->m_adapterBusy);
    ASSERT_EQ(this->m_protocolFrames, failureFrame);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(Fw::Success::FAILURE));
    EXPECT_FALSE(this->component.m_vcs[0].outstanding.packet.isValid());

    // Recovery forwards one readiness credit, without transmitting a discarded tail.
    Fw::Success recovery = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, recovery);
    ASSERT_EQ(this->m_protocolFrames, failureFrame);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, recovery);
    this->sendProtocolPacket(input, 7, true);
    if (this->m_adapterBusy) {
        this->completeProtocolFrame(Fw::Success::SUCCESS);
    }
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(3);
    ASSERT_from_comStatusOut(2, recovery);
}

void AosFramerTester::testIdleTailStatus(bool fail) {
    this->component.configure(64, true);
    this->clearHistory();
    this->m_checkStatusProtocol = true;
    this->m_synchronousStatus = true;
    this->m_failureFrame = fail ? 1 : 0;
    U8 input[53] = {};
    this->sendProtocolPacket(input, sizeof input, true);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(fail ? Fw::Success::FAILURE : Fw::Success::SUCCESS));
    // One byte of the minimum seven-byte idle packet was sent; six remain on success.
    EXPECT_EQ(this->component.m_vcs[0].current_payload_offset, fail ? 0U : 6U);
    if (fail) {
        Fw::Success recovery = Fw::Success::SUCCESS;
        this->invoke_to_comStatusIn(0, recovery);
    }
    this->sendProtocolPacket(input, 7, true);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(fail ? 3 : 2);
    EXPECT_EQ(this->getFramePacketPointer(this->fromPortHistory_dataOut->at(1).data.getData()), fail ? 0U : 6U);
}

Drv::ByteStreamStatus AosFramerTester::driverSend(Fw::PassiveComponentBase* component,
                                                  FwIndexType portNum,
                                                  Fw::Buffer& data) {
    AosFramerTester& tester = *static_cast<AosFramerTester*>(component);
    const ComCfg::FrameContext context;
    // Reuse the wire checks, but let the real ComStub perform buffer/status callbacks.
    tester.from_dataOut_handler(portNum, data, context);
    tester.m_adapterBusy = false;
    return tester.m_protocolFrames == tester.m_failureFrame ? Drv::ByteStreamStatus::OTHER_ERROR
                                                            : Drv::ByteStreamStatus::OP_OK;
}

void AosFramerTester::testComStubStatus(bool fail) {
    this->component.configure(64, true);
    Svc::ComStub adapter("StatusProtocolAdapter");
    adapter.init();
    Drv::InputByteStreamSendPort driver;
    driver.init();
    driver.addCallComp(this, driverSend);
    adapter.set_drvSendOut_OutputPort(0, &driver);
    adapter.set_dataReturnOut_OutputPort(0, this->component.get_dataReturnIn_InputPort(0));
    adapter.set_comStatusOut_OutputPort(0, this->component.get_comStatusIn_InputPort(0));
    this->component.set_dataOut_OutputPort(0, adapter.get_dataIn_InputPort(0));

    adapter.get_drvConnected_InputPort(0)->invoke();
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(Fw::Success::SUCCESS));
    this->clearHistory();
    this->m_checkStatusProtocol = true;
    this->m_failureFrame = fail ? 1 : 0;
    U8 input[117] = {};
    this->m_expectedBytes = input;
    this->m_expectedSize = sizeof input;
    this->sendProtocolPacket(input, sizeof input, true);
    ASSERT_EQ(this->m_protocolFrames, fail ? 1U : 3U);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(fail ? Fw::Success::FAILURE : Fw::Success::SUCCESS));
    if (fail) {
        adapter.get_drvConnected_InputPort(0)->invoke();
        ASSERT_from_comStatusOut_SIZE(2);
        ASSERT_from_comStatusOut(1, Fw::Success(Fw::Success::SUCCESS));
        this->m_protocolBytes = 0;
        this->m_expectedSize = 7;
        this->sendProtocolPacket(input, 7, true);
        ASSERT_from_dataReturnOut_SIZE(2);
        ASSERT_from_comStatusOut_SIZE(3);
        ASSERT_EQ(this->m_protocolFrames, 2U);
    }
}

}  // namespace Ccsds
}  // namespace Svc

TEST(AosFramerStatus, PartialTailWaitsForStatus) {
    COMMENT("A returned frame with an unpadded packet tail produces only one upstream status.");
    Svc::Ccsds::AosFramerTester tester;
    tester.testTailWaitsForStatus();
}

TEST(AosFramerStatus, PacketBoundaries) {
    COMMENT("Verify per-input credits, ownership and wire bytes with synchronous and delayed adapter statuses.");
    for (const bool synchronous : {false, true}) {
        for (const bool fecf : {false, true}) {
            const FwSizeType capacity = fecf ? 54 : 56;
            const FwSizeType sizes[] = {1, capacity - 1, capacity, capacity + 7, 2 * capacity, 2 * capacity + 9};
            for (const bool sendNow : {false, true}) {
                for (const FwSizeType size : sizes) {
                    SCOPED_TRACE(::testing::Message() << "synchronous=" << synchronous << " fecf=" << fecf
                                                      << " sendNow=" << sendNow << " size=" << size);
                    Svc::Ccsds::AosFramerTester tester;
                    tester.testStatusProtocol(size, sendNow, synchronous, fecf);
                }
            }
        }
    }
}

TEST(AosFramerStatus, FailureAndRecovery) {
    COMMENT("Fail each frame of a spanning packet, return ownership, and resume only after recovery.");
    for (const bool synchronous : {false, true}) {
        for (U32 frame = 1; frame <= 3; ++frame) {
            SCOPED_TRACE(::testing::Message() << "synchronous=" << synchronous << " failureFrame=" << frame);
            Svc::Ccsds::AosFramerTester tester;
            tester.testFailureAndRecovery(frame, synchronous);
        }
    }
}

TEST(AosFramerStatus, IdleTailDoesNotDuplicateCredit) {
    COMMENT("Completing an internal idle packet must not issue a second upstream credit.");
    Svc::Ccsds::AosFramerTester tester;
    tester.testIdleTailStatus();
}

TEST(AosFramerStatus, IdleTailFailure) {
    COMMENT("A failed frame must discard an internal idle tail without returning it upstream.");
    Svc::Ccsds::AosFramerTester tester;
    tester.testIdleTailStatus(true);
}

TEST(AosFramerStatus, ComStubSuccess) {
    COMMENT("Exercise a multi-frame packet through the real synchronous ComStub adapter.");
    Svc::Ccsds::AosFramerTester tester;
    tester.testComStubStatus(false);
}

TEST(AosFramerStatus, ComStubFailureAndReconnect) {
    COMMENT("A ComStub failure must not receive another frame until the driver reconnects.");
    Svc::Ccsds::AosFramerTester tester;
    tester.testComStubStatus(true);
}

TEST(AosFramerStatus, PacketLengthSweep) {
    COMMENT("Sweep all positive packet lengths through three frames, including every idle-tail boundary.");
    for (const bool synchronous : {false, true}) {
        for (const bool fecf : {false, true}) {
            const FwSizeType capacity = fecf ? 54 : 56;
            for (const bool sendNow : {false, true}) {
                for (FwSizeType size = 1; size <= 3 * capacity + 7; ++size) {
                    SCOPED_TRACE(::testing::Message() << "synchronous=" << synchronous << " fecf=" << fecf
                                                      << " sendNow=" << sendNow << " size=" << size);
                    Svc::Ccsds::AosFramerTester tester;
                    tester.testStatusProtocol(size, sendNow, synchronous, fecf);
                }
            }
        }
    }
}
