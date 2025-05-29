// ======================================================================
// \title  ApidManagerTester.cpp
// \author chammard
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

void ApidManagerTester ::toDo() {
    // TODO
}

bool ApidManagerTester::GetExistingSeqCount::precondition(const ApidManagerTester& testerState) {
    return true; // Can always get existing sequence count
}

void ApidManagerTester::GetExistingSeqCount::action(ApidManagerTester& testerState) {
    ComCfg::APID::T apid = testerState.getRandomTrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 shadownSeqCount = testerState.shadow_getAndIncrementSeqCount(apid);
    ASSERT_EQ(seqCount, shadownSeqCount)
        << "Sequence count for APID " << static_cast<U16>(apid) << " does not match shadow value."
        << " Shadow: " << shadownSeqCount << ", Actual: " << seqCount;
}


bool ApidManagerTester::GetNewSeqCountOk::precondition(const ApidManagerTester& testerState) {
    return testerState.shadow_isTableFull == false;
}

void ApidManagerTester::GetNewSeqCountOk::action(ApidManagerTester& testerState) {
    bool isTableFull = testerState.shadow_seqCounts.size() >= testerState.component.MAX_TRACKED_APIDS;
    if (isTableFull) {
        testerState.shadow_isTableFull = true;
        return; // Cannot get new sequence count if table is full - skip action
    }

    ComCfg::APID::T apid = testerState.getRandomUntrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 shadownSeqCount = testerState.shadow_getAndIncrementSeqCount(apid);
    ASSERT_EQ(seqCount, shadownSeqCount)
        << "Sequence count for APID " << static_cast<U16>(apid) << " does not match shadow value."
        << " Shadow: " << shadownSeqCount << ", Actual: " << seqCount;
}

bool ApidManagerTester::GetNewSeqCountTableFull::precondition(const ApidManagerTester& testerState) {
    return testerState.shadow_isTableFull == true;
}

void ApidManagerTester::GetNewSeqCountTableFull::action(ApidManagerTester& testerState) {
    ComCfg::APID::T apid = testerState.getRandomUntrackedApid();
    U16 seqCount = testerState.invoke_to_getApidSeqCountIn(0, apid, 0);
    ASSERT_EQ(seqCount, testerState.component.SEQUENCE_COUNT_ERROR)
        << "Expected SEQUENCE_COUNT_ERROR for untracked APID " << static_cast<U16>(apid)
        << ", but got " << seqCount;
    testerState.assertEvents_ApidTableFull_size(__FILE__, __LINE__, 1);
    testerState.assertEvents_ApidTableFull(__FILE__, __LINE__, 0, static_cast<U16>(apid));
}

}  // namespace CCSDS
}  // namespace Svc
