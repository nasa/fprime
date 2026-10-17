#include <cstring>
#include <limits>
#include "FrameAccumulatorTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {
namespace {
// Independently encoded F Prime frame; CRC32 checked with Python zlib.
constexpr U8 GOLDEN_FRAME[] = {0xDE, 0xAD, 0xBE, 0xEF, 0, 0, 0, 4, 1, 2, 3, 4, 0xE3, 0xF9, 0xA0, 0x73};
constexpr U32 WARNING_LIMIT = FrameAccumulatorCfg::CrcFailureWarningThrottle;
constexpr U32 WARNING_PERIOD = FrameAccumulatorCfg::CrcFailureWarningPeriodSeconds;
}  // namespace

void FrameAccumulatorTester::sendDiagnosticInput(U8* data, FwSizeType size) {
    this->clearHistory();
    Fw::Buffer buffer(data, size);
    const ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
}

void FrameAccumulatorTester::checkDiagnosticCounts(U64 failures, U64 discarded) {
    ASSERT_EQ(this->component.m_crcFailures, failures);
    ASSERT_EQ(this->component.m_resyncBytesDiscarded, discarded);
}

void FrameAccumulatorTester::stepDiagnosticMock(FrameDetector::Status status, FwSizeType size) {
    U8 data[8] = {};
    ASSERT_LE(size, sizeof(data));
    const bool valid = status == FrameDetector::FRAME_DETECTED;
    this->mockDetector.set_next_result(status, valid ? size : 0);
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, this->m_shadowTime, 0));
    const U64 initialFailures = this->m_shadowCrcFailures;
    const U64 initialDiscarded = this->m_shadowResyncBytes;
    U32 expectedWarnings = 0;
    if (status == FrameDetector::CRC_FAILURE) {
        for (FwSizeType i = 0; i < size; ++i) {
            ++this->m_shadowCrcFailures;
            if ((this->m_shadowWarnings == WARNING_LIMIT) &&
                (this->m_shadowTime - this->m_shadowPeriodStart >= WARNING_PERIOD)) {
                this->m_shadowWarnings = 0;
            }
            if (this->m_shadowWarnings < WARNING_LIMIT) {
                if (this->m_shadowWarnings == 0) {
                    this->m_shadowPeriodStart = this->m_shadowTime;
                }
                ++this->m_shadowWarnings;
                ++expectedWarnings;
            }
        }
    }
    if (!valid) {
        this->m_shadowResyncBytes += size;
    }
    this->sendDiagnosticInput(data, size);
    this->checkDiagnosticCounts(this->m_shadowCrcFailures, this->m_shadowResyncBytes);
    ASSERT_EVENTS_FrameCrcFailure_SIZE(expectedWarnings);
    for (U32 i = 0; i < expectedWarnings; ++i) {
        ASSERT_EVENTS_FrameCrcFailure(i, initialFailures + i + 1, initialDiscarded + i + 1);
    }
    ASSERT_from_dataOut_SIZE(valid ? 1 : 0);
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), 0);
}

void FrameAccumulatorTester::testCrcWarningThrottle() {
    this->checkDiagnosticCounts(0, 0);
    for (U32 i = 0; i < WARNING_LIMIT; ++i) {
        this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, 1);
        ASSERT_EVENTS_FrameCrcFailure(0, i + 1, i + 1);
    }
    this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, 3);
    this->stepDiagnosticMock(FrameDetector::FRAME_DETECTED, 1);
    this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, 1);
    this->m_shadowTime += WARNING_PERIOD - 1;
    this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, 1);
    ++this->m_shadowTime;
    this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, 1);
    ASSERT_EVENTS_FrameCrcFailure(0, WARNING_LIMIT + 6, WARNING_LIMIT + 6);
}

void FrameAccumulatorTester::testCrcWarningPayload() {
    this->stepDiagnosticMock(FrameDetector::NO_FRAME_DETECTED, 3);
    this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, 8);
}

void FrameAccumulatorTester::testCrcWarningsDisabled() {
    this->mockDetector.set_next_result(FrameDetector::CRC_FAILURE, 0);
    U8 bytes[3] = {};
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 100, 0));
    this->sendDiagnosticInput(bytes, sizeof(bytes));
    this->checkDiagnosticCounts(3, 3);
    ASSERT_EVENTS_FrameCrcFailure_SIZE(0);
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 100 + WARNING_PERIOD, 0));
    this->sendDiagnosticInput(bytes, sizeof(bytes));
    this->checkDiagnosticCounts(6, 6);
    ASSERT_EVENTS_SIZE(0);
}

void FrameAccumulatorTester::testCrcRecovery(bool embedded) {
    const FrameDetectors::FprimeFrameDetector detector;
    this->component.m_detector = &detector;
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 100, 0));
    U8 stream[2 * sizeof(GOLDEN_FRAME)] = {};
    FwSizeType streamSize = 0;
    U64 expectedDiscarded = 0;
    FwSizeType expectedRemaining = 0;
    if (embedded) {
        // Outer frame contains a valid inner frame and an intentionally wrong CRC.
        const U8 header[] = {0xDE, 0xAD, 0xBE, 0xEF, 0, 0, 0, sizeof(GOLDEN_FRAME)};
        ::memcpy(stream, header, sizeof(header));
        ::memcpy(stream + sizeof(header), GOLDEN_FRAME, sizeof(GOLDEN_FRAME));
        streamSize = sizeof(header) + sizeof(GOLDEN_FRAME) + 4;
        expectedDiscarded = sizeof(header);
        expectedRemaining = 4;
    } else {
        ::memcpy(stream, GOLDEN_FRAME, sizeof(GOLDEN_FRAME));
        stream[8] ^= 1;
        ::memcpy(stream + sizeof(GOLDEN_FRAME), GOLDEN_FRAME, sizeof(GOLDEN_FRAME));
        streamSize = sizeof(stream);
        expectedDiscarded = sizeof(GOLDEN_FRAME);
    }
    // A complete header without the full candidate must not be called a CRC failure.
    const FwSizeType firstSize = embedded ? streamSize - 1 : sizeof(GOLDEN_FRAME) - 1;
    this->sendDiagnosticInput(stream, firstSize);
    this->checkDiagnosticCounts(0, 0);
    ASSERT_EVENTS_SIZE(0);
    ASSERT_from_dataOut_SIZE(0);
    this->sendDiagnosticInput(stream + firstSize, streamSize - firstSize);
    this->checkDiagnosticCounts(1, expectedDiscarded);
    ASSERT_EVENTS_FrameCrcFailure_SIZE(1);
    ASSERT_EVENTS_FrameCrcFailure(0, 1, 1);
    ASSERT_from_dataOut_SIZE(1);
    const Fw::Buffer& forwarded = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(forwarded.getSize(), sizeof(GOLDEN_FRAME));
    ASSERT_EQ(::memcmp(forwarded.getData(), GOLDEN_FRAME, sizeof(GOLDEN_FRAME)), 0);
    ASSERT_EQ(this->component.m_inRing.get_allocated_size(), expectedRemaining);
}

void FrameAccumulatorTester::testCrcAccounting() {
    this->stepDiagnosticMock(FrameDetector::NO_FRAME_DETECTED, 3);
    U8 bytes[2] = {};
    this->mockDetector.set_next_result(FrameDetector::MORE_DATA_NEEDED, 2049);
    this->sendDiagnosticInput(bytes, sizeof(bytes));
    this->checkDiagnosticCounts(0, 5);
    ASSERT_EVENTS_FrameDetectionSizeError_SIZE(2);
    ASSERT_EVENTS_FrameCrcFailure_SIZE(0);
    // A whole valid-frame allocation drop is a different loss category.
    U8 full[2048] = {};
    this->m_failBufferAllocate = true;
    this->mockDetector.set_next_result(FrameDetector::FRAME_DETECTED, sizeof(full));
    this->sendDiagnosticInput(full, sizeof(full));
    this->checkDiagnosticCounts(0, 5);
    ASSERT_EVENTS_FrameDetectionValidFrameDropped_SIZE(1);
    ASSERT_EVENTS_FrameCrcFailure_SIZE(0);
}

void FrameAccumulatorTester::testCrcCounterSaturation() {
    const U64 maximum = std::numeric_limits<U64>::max();
    this->component.m_crcFailures = maximum - 1;
    this->component.m_resyncBytesDiscarded = maximum - 1;
    this->mockDetector.set_next_result(FrameDetector::CRC_FAILURE, 0);
    U8 bytes[3] = {};
    this->sendDiagnosticInput(bytes, sizeof(bytes));
    this->checkDiagnosticCounts(maximum, maximum);
    ASSERT_EVENTS_FrameCrcFailure(0, maximum, maximum);
    this->mockDetector.set_next_result(FrameDetector::NO_FRAME_DETECTED, 0);
    this->sendDiagnosticInput(bytes, sizeof(bytes));
    this->checkDiagnosticCounts(maximum, maximum);
    ASSERT_EVENTS_SIZE(0);
}

bool FrameAccumulatorTester::Crc__Failure__precondition() const {
    return true;
}
void FrameAccumulatorTester::Crc__Failure__action() {
    this->stepDiagnosticMock(FrameDetector::CRC_FAILURE, STest::Random::lowerUpper(1, 8));
}
bool FrameAccumulatorTester::Crc__Noise__precondition() const {
    return true;
}
void FrameAccumulatorTester::Crc__Noise__action() {
    this->stepDiagnosticMock(FrameDetector::NO_FRAME_DETECTED, STest::Random::lowerUpper(1, 8));
}
bool FrameAccumulatorTester::Crc__Valid__precondition() const {
    return true;
}
void FrameAccumulatorTester::Crc__Valid__action() {
    this->stepDiagnosticMock(FrameDetector::FRAME_DETECTED, STest::Random::lowerUpper(1, 8));
}
bool FrameAccumulatorTester::Crc__AdvanceTime__precondition() const {
    return true;
}
void FrameAccumulatorTester::Crc__AdvanceTime__action() {
    this->clearHistory();
    this->m_shadowTime += STest::Random::lowerUpper(1, WARNING_PERIOD + 1);
}

TEST(FrameAccumulatorCrc, WarningThrottle) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-006");
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-009");
    COMMENT("Bound warnings, retain cumulative counts, and re-arm without resetting on valid frames.");
    FrameAccumulatorTester tester;
    tester.testCrcWarningThrottle();
}
TEST(FrameAccumulatorCrc, WarningPayloadAfterNoise) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-006");
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-008");
    COMMENT("Verify every warning snapshot when CRC-failure and discard counts differ.");
    FrameAccumulatorTester tester;
    tester.testCrcWarningPayload();
}
TEST(FrameAccumulatorCrc, WarningsDisabled) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-007");
    COMMENT("Disabled warnings still count CRC failures and return input ownership.");
    FrameAccumulatorTester tester(false);
    tester.testCrcWarningsDisabled();
}
TEST(FrameAccumulatorCrc, FragmentedRecovery) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-008");
    COMMENT("Wait for the trailer and recover a following valid frame byte-for-byte.");
    FrameAccumulatorTester tester;
    tester.testCrcRecovery(false);
}
TEST(FrameAccumulatorCrc, EmbeddedFrameRecovery) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-008");
    COMMENT("Recover an embedded frame without counting the entire rejected candidate as discarded.");
    FrameAccumulatorTester tester;
    tester.testCrcRecovery(true);
}
TEST(FrameAccumulatorCrc, DiscardAccounting) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-008");
    COMMENT("Count actual search rotations separately from valid-frame allocation drops.");
    FrameAccumulatorTester tester;
    tester.testCrcAccounting();
}
TEST(FrameAccumulatorCrc, CounterSaturation) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-010");
    COMMENT("Counters saturate without wraparound or interrupting resynchronization.");
    FrameAccumulatorTester tester;
    tester.testCrcCounterSaturation();
}
TEST(FrameAccumulatorCrc, RandomizedDiagnostics) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-006");
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-007");
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-008");
    COMMENT("Interleave failures, noise, valid frames and time advances in 2000 bounded steps.");
    FrameAccumulatorTester tester;
    FrameAccumulatorTester::Crc__Failure failure;
    FrameAccumulatorTester::Crc__Noise noise;
    FrameAccumulatorTester::Crc__Valid valid;
    FrameAccumulatorTester::Crc__AdvanceTime time;
    STest::Rule<FrameAccumulatorTester>* rules[] = {&failure, &noise, &valid, &time};
    STest::RandomScenario<FrameAccumulatorTester> random("CRC diagnostics", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<FrameAccumulatorTester> bounded("CRC diagnostics bounded", random, 2000);
    bounded.run(tester);
}
TEST(FrameAccumulatorCrc, IndependentInstances) {
    REQUIREMENT("SVC-FRAME-ACCUMULATOR-009");
    COMMENT("Each live instance owns independent cumulative counts and warning quota.");
    FrameAccumulatorTester first;
    FrameAccumulatorTester second;
    first.testCrcWarningThrottle();
    second.testCrcWarningThrottle();
}
}  // namespace Svc
