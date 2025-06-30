// ======================================================================
// \title  ApidManagerTester.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component test harness implementation class
// ======================================================================

#include "ApidManagerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

static constexpr ComCfg::APID::T TEST_REGISTERED_APIDS[] = {ComCfg::APID::FW_PACKET_COMMAND,
                                                            ComCfg::APID::FW_PACKET_TELEM, ComCfg::APID::FW_PACKET_LOG,
                                                            ComCfg::APID::FW_PACKET_FILE};

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ApidManagerTester ::ApidManagerTester()
    : ApidManagerGTestBase("ApidManagerTester", ApidManagerTester::MAX_HISTORY_SIZE), component("ApidManager") {
    this->initComponents();
    this->connectPorts();
    // Initialize existing sequence counts for common APIDs
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(TEST_REGISTERED_APIDS)); i++) {
        this->component.m_apidSequences[i].apid = TEST_REGISTERED_APIDS[i];
        this->shadow_seqCounts[TEST_REGISTERED_APIDS[i]] = 0;  // Initialize shadow sequence counts to 0
    }
}

ApidManagerTester ::~ApidManagerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

bool ApidManagerTester::GetExistingSeqCount::precondition(const ApidManagerTester& testerState) {
    return true;  // Can always get existing sequence count
}

void ApidManagerTester::GetExistingSeqCount::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.shadow_getRandomTrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 shadowSeqCount = testerState.shadow_getAndIncrementSeqCount(apid);
    ASSERT_EQ(seqCount, shadowSeqCount) << "Sequence count for APID " << static_cast<U16>(apid)
                                        << " does not match shadow value."
                                        << " Shadow: " << shadowSeqCount << ", Actual: " << seqCount;
}

bool ApidManagerTester::GetNewSeqCountOk::precondition(const ApidManagerTester& testerState) {
    return testerState.shadow_isTableFull == false;
}

void ApidManagerTester::GetNewSeqCountOk::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    // Use local constexpr to potentially avoid ODR-use of ApidManager::MAX_TRACKED_APIDS
    constexpr U8 maxTrackedApidsVal = ApidManager::MAX_TRACKED_APIDS;
    bool isTableFull = !(testerState.shadow_seqCounts.size() < maxTrackedApidsVal);
    if (isTableFull) {
        testerState.shadow_isTableFull = true;
        return;  // Cannot get new sequence count if table is full - skip action
    }

    ComCfg::APID::T apid = testerState.shadow_getRandomUntrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 shadowSeqCount = testerState.shadow_getAndIncrementSeqCount(apid);
    ASSERT_EQ(seqCount, shadowSeqCount) << "Sequence count for APID " << static_cast<U16>(apid)
                                        << " does not match shadow value."
                                        << " Shadow: " << shadowSeqCount << ", Actual: " << seqCount;
}

bool ApidManagerTester::GetNewSeqCountTableFull::precondition(const ApidManagerTester& testerState) {
    return testerState.shadow_isTableFull == true;
}

void ApidManagerTester::GetNewSeqCountTableFull::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.shadow_getRandomUntrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    // Use local constexpr to potentially avoid ODR-use of ApidManager::SEQUENCE_COUNT_ERROR
    constexpr U16 sequenceCountErrorVal = ApidManager::SEQUENCE_COUNT_ERROR;
    ASSERT_EQ(seqCount, sequenceCountErrorVal)
        << "Expected SEQUENCE_COUNT_ERROR for untracked APID " << static_cast<U16>(apid) << ", but got " << seqCount;
    testerState.assertEvents_ApidTableFull_size(__FILE__, __LINE__, 1);
    testerState.assertEvents_ApidTableFull(__FILE__, __LINE__, 0, static_cast<U16>(apid));
}

bool ApidManagerTester::ValidateSeqCountOk::precondition(const ApidManagerTester& testerState) {
    return true;
}

void ApidManagerTester::ValidateSeqCountOk::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.shadow_getRandomTrackedApid();
    U16 shadow_expectedSeqCount = testerState.shadow_seqCounts[apid];
    testerState.invoke_to_validateApidSeqCountIn(0, apid, shadow_expectedSeqCount);
    testerState.shadow_validateApidSeqCount(apid, shadow_expectedSeqCount);  // keep shadow state in sync

    testerState.assertEvents_UnexpectedSequenceCount_size(__FILE__, __LINE__, 0);
}

bool ApidManagerTester::ValidateSeqCountFailure::precondition(const ApidManagerTester& testerState) {
    return true;
}

void ApidManagerTester::ValidateSeqCountFailure::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.shadow_getRandomTrackedApid();
    U16 shadow_expectedSeqCount = testerState.shadow_seqCounts.at(apid);
    U16 invalidSeqCount = static_cast<U16>(
        (shadow_expectedSeqCount + 1) %
        (1 << SpacePacketSubfields::SeqCountWidth));  // Or any other value that's different, ensure wrap around

    // Invoke the port with the deliberately incorrect sequence count
    testerState.invoke_to_validateApidSeqCountIn(0, apid, invalidSeqCount);
    testerState.shadow_validateApidSeqCount(apid, invalidSeqCount);  // keep shadow state in sync

    // Now, the event should be logged
    testerState.assertEvents_UnexpectedSequenceCount_size(__FILE__, __LINE__, 1);
    testerState.assertEvents_UnexpectedSequenceCount(__FILE__, __LINE__, 0, invalidSeqCount, shadow_expectedSeqCount);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

U16 ApidManagerTester::shadow_getAndIncrementSeqCount(ComCfg::APID::T apid) {
    // This is a shadow function to simulate the getAndIncrementSeqCount behavior
    // without modifying the actual component state.
    auto found = this->shadow_seqCounts.find(apid);
    if (found != this->shadow_seqCounts.end()) {
        U16 seqCount = found->second;
        found->second =
            static_cast<U16>((seqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));  // Increment for next call
        return seqCount;  // Return the current sequence count
    }
    // If APID not found, initialize a new entry
    if (this->shadow_seqCounts.size() < this->component.MAX_TRACKED_APIDS) {
        U16 seqCount = 0;
        this->shadow_seqCounts[apid] = static_cast<U16>(seqCount + 1);  // increment for next call
        return seqCount;                                                // Return the initialized sequence count
    }
    return this->component.SEQUENCE_COUNT_ERROR;  // Return error if APID not found
}

void ApidManagerTester::shadow_validateApidSeqCount(ComCfg::APID::T apid, U16 expectedSeqCount) {
    // This simply updates the shadow state to the next expected sequence count
    auto found = this->shadow_seqCounts.find(apid);
    if (found != this->shadow_seqCounts.end()) {
        found->second = static_cast<U16>((expectedSeqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
    }
}

ComCfg::APID::T ApidManagerTester::shadow_getRandomTrackedApid() {
    // Select a random APID from the sequence counts map
    U32 mapSize = static_cast<U32>(this->shadow_seqCounts.size());
    U32 randomIndex = STest::Random::lowerUpper(0, mapSize - 1);
    ComCfg::APID apid = std::next(this->shadow_seqCounts.begin(), randomIndex)->first;
    return apid;
}

ComCfg::APID::T ApidManagerTester::shadow_getRandomUntrackedApid() {
    // Select a random APID that is not currently tracked
    ComCfg::APID::T apid;
    do {
        apid = static_cast<ComCfg::APID::T>(STest::Random::lowerUpper(10, ComCfg::APID::SPP_IDLE_PACKET));
    } while (this->shadow_seqCounts.find(apid) != this->shadow_seqCounts.end());
    return apid;
}

}  // namespace Ccsds
}  // namespace Svc
