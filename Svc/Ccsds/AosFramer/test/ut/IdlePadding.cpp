// Verify SPP idle filling when both supported-packet bits are requested.
#include "AosFramerTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"

namespace Svc {
namespace Ccsds {

void AosFramerTester::testIdlePadding(FwSizeType idleBytes, bool fecf, bool sendNow, U8 idlePvns) {
    this->clearHistory();
    constexpr FwSizeType frameSize = 64;
    const FwSizeType headerSize = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;
    const FwSizeType payloadEnd = frameSize - (fecf ? AOSTrailer::SERIALIZED_SIZE : 0);
    const FwSizeType payloadSize = payloadEnd - headerSize;
    ASSERT_GT(idleBytes, 0U);
    ASSERT_LT(idleBytes, payloadSize);
    this->component.configure(frameSize, fecf, ComCfg::SpacecraftId, 1, idlePvns);

    U8 bytes[frameSize] = {};
    const FwSizeType packetSize = payloadSize - idleBytes;
    for (FwSizeType i = 0; i < packetSize; ++i) {
        bytes[i] = static_cast<U8>(0x80U + i);
    }
    Fw::Buffer packet(bytes, packetSize);
    ComCfg::FrameContext context;
    context.set_vcId(1);
    context.set_sendNow(sendNow);
    this->invoke_to_dataIn(0, packet, context);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, packet, context);

    if (!sendNow) {
        ASSERT_from_dataOut_SIZE(0);
        ASSERT_EQ(this->component.m_vcs[0].current_payload_offset, packetSize);
        return;
    }

    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer frame = this->fromPortHistory_dataOut->at(0).data;
    const ComCfg::FrameContext frameContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(frame.getSize(), frameSize);
    ASSERT_EQ(frameContext.get_vcId(), 1U);
    ASSERT_EQ(this->getFramePacketPointer(frame.getData()), 0U);
    for (FwSizeType i = 0; i < packetSize; ++i) {
        ASSERT_EQ(frame.getData()[headerSize + i], bytes[i]);
    }

    // Independent wire expectation: idle APID 0x7FF, unsegmented, no
    // secondary header, and a length token equal to payload length minus one.
    const FwSizeType idleSize = idleBytes < 7 ? 7 : idleBytes;
    const U8 idleHeader[] = {0x07, 0xFF, 0xC0, 0x00, 0x00, static_cast<U8>(idleSize - 7)};
    for (FwSizeType i = 0; i < idleBytes; ++i) {
        const U8 expected = i < sizeof idleHeader ? idleHeader[i] : 0x44;
        ASSERT_EQ(frame.getData()[headerSize + packetSize + i], expected) << i;
    }
    if (fecf) {
        const U16 crc = Utils::CRC16::compute(frame.getData(), static_cast<U32>(payloadEnd));
        ASSERT_EQ(frame.getData()[payloadEnd], static_cast<U8>(crc >> 8));
        ASSERT_EQ(frame.getData()[payloadEnd + 1], static_cast<U8>(crc));
    }

    // Return ownership and report adapter readiness. This works before and
    // after the independent status-ordering fix, without assuming its timing.
    this->invoke_to_dataReturnIn(0, frame, frameContext);
    Fw::Success ready = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, ready);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);  // Internal idle storage has no upstream owner.
    const FwSizeType tailSize = idleSize - idleBytes;
    ASSERT_EQ(this->component.m_vcs[0].current_payload_offset, tailSize);
    ASSERT_FALSE(this->component.m_vcs[0].outstanding.packet.isValid());
    for (FwSizeType i = 0; i < tailSize; ++i) {
        const FwSizeType idleIndex = idleBytes + i;
        const U8 expected = idleIndex < sizeof idleHeader ? idleHeader[idleIndex] : 0x44;
        ASSERT_EQ(this->component.m_vcs[0].frame.backer[headerSize + i], expected);
    }
}

void AosFramerTester::testUnsupportedIdleMask(U8 idlePvns) {
    this->clearHistory();
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(64, true, ComCfg::SpacecraftId, 1, idlePvns), "AosFramer.cpp");
}

}  // namespace Ccsds
}  // namespace Svc

TEST(AosFramerIdlePadding, MixedMaskFillsEveryRemainder) {
    COMMENT("SPP idle padding is retained when EPP is also enabled, including split idle headers.");
    for (U32 crc = 0; crc < 2; ++crc) {
        const FwSizeType payloadSize = 64 - Svc::Ccsds::AOSHeader::SERIALIZED_SIZE -
                                       Svc::Ccsds::M_PDUHeader::SERIALIZED_SIZE -
                                       (crc != 0 ? Svc::Ccsds::AOSTrailer::SERIALIZED_SIZE : 0);
        for (FwSizeType remaining = 1; remaining < payloadSize; ++remaining) {
            Svc::Ccsds::AosFramerTester tester;
            ASSERT_NO_FATAL_FAILURE(tester.testIdlePadding(
                remaining, crc != 0, true, Svc::Ccsds::PvnBitfield::SPP_MASK | Svc::Ccsds::PvnBitfield::EPP_MASK));
        }
    }
}

TEST(AosFramerIdlePadding, SppOnlyRemainsUnchanged) {
    COMMENT("The existing SPP-only path retains its padding and split-packet boundaries.");
    const FwSizeType boundaries[] = {1, 6, 7, 8, 20, 53};
    for (U32 crc = 0; crc < 2; ++crc) {
        for (FwSizeType remaining : boundaries) {
            Svc::Ccsds::AosFramerTester tester;
            tester.testIdlePadding(remaining, crc != 0, true, Svc::Ccsds::PvnBitfield::SPP_MASK);
        }
    }
}

TEST(AosFramerIdlePadding, DoesNotForceTransmissionWithoutSendNow) {
    COMMENT("Enabling EPP alongside SPP does not force a partial frame to be transmitted.");
    for (U32 crc = 0; crc < 2; ++crc) {
        Svc::Ccsds::AosFramerTester tester;
        tester.testIdlePadding(7, crc != 0, false,
                               Svc::Ccsds::PvnBitfield::SPP_MASK | Svc::Ccsds::PvnBitfield::EPP_MASK);
    }
}

TEST(AosFramerIdlePadding, UnsupportedIdleMasksStillAssert) {
    COMMENT("Configuration still requires SPP idle support; EPP-only idle generation is unsupported.");
    Svc::Ccsds::AosFramerTester tester;
    tester.testUnsupportedIdleMask(0);
    tester.testUnsupportedIdleMask(Svc::Ccsds::PvnBitfield::EPP_MASK);
}
