// ======================================================================
// \title  ApidManagerTester.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component test harness implementation class
// ======================================================================

#include "ApidManagerTester.hpp"
#include "STest/Random/Random.hpp"

namespace Svc {

namespace CCSDS {

static constexpr ComCfg::APID::T TEST_REGISTERED_APIDS[] = {
    ComCfg::APID::FW_PACKET_COMMAND,
    ComCfg::APID::FW_PACKET_TELEM,
    ComCfg::APID::FW_PACKET_LOG,
    ComCfg::APID::FW_PACKET_FILE
};
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

// void ApidManagerTester ::toDo() {
//     // TODO
// }

bool ApidManagerTester::GetExistingSeqCount::precondition(const ApidManagerTester& testerState) {
    return true; // Can always get existing sequence count
}

void ApidManagerTester::GetExistingSeqCount::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.getRandomTrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 shadowSeqCount = testerState.shadow_getAndIncrementSeqCount(apid);
    ASSERT_EQ(seqCount, shadowSeqCount)
        << "Sequence count for APID " << static_cast<U16>(apid) << " does not match shadow value."
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
        return; // Cannot get new sequence count if table is full - skip action
    }

    ComCfg::APID::T apid = testerState.getRandomUntrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 shadowSeqCount = testerState.shadow_getAndIncrementSeqCount(apid);
    ASSERT_EQ(seqCount, shadowSeqCount)
        << "Sequence count for APID " << static_cast<U16>(apid) << " does not match shadow value."
        << " Shadow: " << shadowSeqCount << ", Actual: " << seqCount;
}

bool ApidManagerTester::GetNewSeqCountTableFull::precondition(const ApidManagerTester& testerState) {
    return testerState.shadow_isTableFull == true;
}

void ApidManagerTester::GetNewSeqCountTableFull::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.getRandomUntrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    // Use local constexpr to potentially avoid ODR-use of ApidManager::SEQUENCE_COUNT_ERROR
    constexpr U16 sequenceCountErrorVal = ApidManager::SEQUENCE_COUNT_ERROR;
    ASSERT_EQ(seqCount, sequenceCountErrorVal)
        << "Expected SEQUENCE_COUNT_ERROR for untracked APID " << static_cast<U16>(apid)
        << ", but got " << seqCount;
    testerState.assertEvents_ApidTableFull_size(__FILE__, __LINE__, 1);
    testerState.assertEvents_ApidTableFull(__FILE__, __LINE__, 0, static_cast<U16>(apid));
}

bool ApidManagerTester::ValidateSeqCountOk::precondition(const ApidManagerTester& testerState) {
    return true;
}

void ApidManagerTester::ValidateSeqCountOk::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.getRandomTrackedApid();
    U16 shadow_expectedSeqCount = testerState.shadow_seqCounts[apid];
    testerState.invoke_to_validateApidSeqCountIn(0, apid, shadow_expectedSeqCount);
    testerState.shadow_validateApidSeqCount(apid, shadow_expectedSeqCount); // keep shadow state in sync

    testerState.assertEvents_UnexpectedSequenceCount_size(__FILE__, __LINE__, 0);
}

bool ApidManagerTester::ValidateSeqCountFailure::precondition(const ApidManagerTester& testerState) {
    return true;
}

void ApidManagerTester::ValidateSeqCountFailure::action(ApidManagerTester& testerState) {
    testerState.clearHistory();
    ComCfg::APID::T apid = testerState.getRandomTrackedApid();
    U16 shadow_expectedSeqCount = testerState.shadow_seqCounts.at(apid);
    U16 invalidSeqCount = (shadow_expectedSeqCount + 1) % (1 << 14); // Or any other value that's different, ensure wrap around

    // Invoke the port with the deliberately incorrect sequence count
    testerState.invoke_to_validateApidSeqCountIn(0, apid, invalidSeqCount);
    testerState.shadow_validateApidSeqCount(apid, invalidSeqCount); // keep shadow state in sync

    // Now, the event should be logged
    testerState.assertEvents_UnexpectedSequenceCount_size(__FILE__, __LINE__, 1);
    testerState.assertEvents_UnexpectedSequenceCount(__FILE__, __LINE__, 0, invalidSeqCount, shadow_expectedSeqCount);

}

};  // namespace CCSDS
};  // namespace Svc
