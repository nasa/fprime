// Check parameter values against observable post-inactivity transaction lifetime.
#include "CfdpManagerTester.hpp"
#include "Fw/Test/UnitTest.hpp"

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void CfdpManagerTester::testParamPostInactivitySendRetriesDefault() {
    this->clearHistory();
    Fw::ParamValid valid = Fw::ParamValid::INVALID;
    EXPECT_EQ(this->component.paramGet_PostInactivitySendRetries(valid), 3U);
    EXPECT_EQ(valid.e, Fw::ParamValid::DEFAULT);
    EXPECT_EQ(this->component.getPostInactivitySendRetriesParam(), 3U);
}

void CfdpManagerTester::testParamPostInactivitySendRetriesSetGet() {
    const U8 values[] = {0, 1, 5, 255};
    for (U8 value : values) {
        this->clearHistory();
        this->paramSet_PostInactivitySendRetries(value, Fw::ParamValid::VALID);
        this->paramSend_PostInactivitySendRetries(0, 0);
        Fw::ParamValid valid = Fw::ParamValid::INVALID;
        EXPECT_EQ(this->component.paramGet_PostInactivitySendRetries(valid), value);
        EXPECT_EQ(valid.e, Fw::ParamValid::VALID);
        EXPECT_EQ(this->component.getPostInactivitySendRetriesParam(), value);
    }
}

void CfdpManagerTester::testPostInactivitySendRetryBudget(U8 retries, bool recoverBuffer) {
    this->clearHistory();
    this->paramSet_PostInactivitySendRetries(retries, Fw::ParamValid::VALID);
    this->paramSend_PostInactivitySendRetries(0, 0);
    this->clearHistory();

    constexpr U8 channel = 0;
    const char* const source = "test/ut/output/retry.bin";
    const char* const destination = "/ground/retry.bin";
    FwSizeType actualSize = 0;
    ASSERT_NO_FATAL_FAILURE(this->createAndVerifyTestFile(source, 1, actualSize));
    TransactionSetup setup;
    ASSERT_NO_FATAL_FAILURE(this->setupTxTransaction(source, destination, channel, TEST_GROUND_EID, Class::CLASS_2,
                                                     TEST_PRIORITY, TxnState::TXN_STATE_S2, setup));
    // Reach the pending FIN-ACK through the normal command and PDU interfaces.
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_TRUE(setup.txn->m_flags.tx.send_eof);
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FALSE(setup.txn->m_flags.tx.send_eof);
    this->sendAckPdu(channel, this->component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum,
                     FileDirective::FILE_DIRECTIVE_END_OF_FILE, 0, ConditionCode::CONDITION_CODE_NO_ERROR,
                     AckTxnStatus::ACK_TXN_STATUS_ACTIVE);
    this->component.doDispatch();
    this->sendFinPdu(channel, this->component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum,
                     ConditionCode::CONDITION_CODE_NO_ERROR, FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE,
                     FinFileStatus::FIN_FILE_STATUS_RETAINED);
    this->component.doDispatch();
    ASSERT_EQ(setup.txn->m_state, TxnState::TXN_STATE_HOLD);
    ASSERT_TRUE(setup.txn->m_flags.tx.send_fin_ack);
    ASSERT_EQ(setup.txn->m_flags.com.post_inactivity_send_retries, 0U);

    // Fault injection accelerates only the timer and denies allocation. The
    // retry counter, send flag and queue membership are changed by production code.
    setup.txn->m_inactivity_timer.setTimer(1);
    this->setFailBufferAllocation(true);
    this->m_bufferAllocationCalls = 0;
    const U32 failedTicks = recoverBuffer ? 1U : static_cast<U32>(retries) + 1U;
    for (U32 tick = 0; tick < failedTicks; ++tick) {
        this->clearHistory();
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
        ASSERT_from_dataOut_SIZE(0);
        EXPECT_EQ(this->m_bufferAllocationCalls, tick + 1U);
        Transaction* const current = this->findTransaction(channel, setup.expectedSeqNum);
        if (tick < retries) {
            ASSERT_NE(current, nullptr);
            EXPECT_TRUE(current->m_flags.com.inactivity_fired);
            EXPECT_TRUE(current->m_flags.tx.send_fin_ack);
            EXPECT_EQ(current->m_flags.com.post_inactivity_send_retries, tick + 1U);
        } else {
            EXPECT_EQ(current, nullptr) << "The configured retry budget must bound the transaction lifetime";
        }
    }

    if (recoverBuffer) {
        ASSERT_GT(retries, 1U);
        this->setFailBufferAllocation(false);
        this->clearHistory();
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
        ASSERT_from_dataOut_SIZE(1);
        ASSERT_NO_FATAL_FAILURE(
            this->verifyFinAckPdu(0, this->component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum));
        EXPECT_EQ(this->m_bufferAllocationCalls, 2U);
        this->clearHistory();
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
        EXPECT_EQ(this->findTransaction(channel, setup.expectedSeqNum), nullptr);
        ASSERT_from_dataOut_SIZE(0);
    }
    this->setFailBufferAllocation(false);
    this->cleanupTestFile(source);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

TEST(PostInactivityRetries, DefaultValue) {
    COMMENT("The default post-inactivity retry budget is three and is marked DEFAULT.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamPostInactivitySendRetriesDefault();
}

TEST(PostInactivityRetries, SetGetBoundaries) {
    COMMENT("Changing the retry parameter preserves zero, non-default values, and the U8 maximum.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testParamPostInactivitySendRetriesSetGet();
}

TEST(PostInactivityRetries, PendingSendBudget) {
    REQUIREMENT("CFDP-009");
    COMMENT("Observe allocation attempts and transaction recycling for several configured retry budgets.");
    const U8 budgets[] = {0, 1, 3, 5, 255};
    for (U8 budget : budgets) {
        Svc::Ccsds::Cfdp::CfdpManagerTester tester;
        ASSERT_NO_FATAL_FAILURE(tester.testPostInactivitySendRetryBudget(budget, false));
    }
}

TEST(PostInactivityRetries, BufferRecoveryCompletesPendingSend) {
    REQUIREMENT("CFDP-009");
    COMMENT("A buffer becoming available before retry exhaustion permits the pending FIN-ACK.");
    Svc::Ccsds::Cfdp::CfdpManagerTester tester;
    tester.testPostInactivitySendRetryBudget(5, true);
}
