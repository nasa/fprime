

#include "RulesHeaders.hpp"


// ------------------------------------------------------------------------------------------------------
// Rule:  Start
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Task::Tester::Start::Start() :
        STest::Rule<Os::Test::Task::Tester>("Start") {
}


bool Os::Test::Task::Tester::Start::precondition(
        const Os::Test::Task::Tester &state //!< The test state
) {
    return state.m_state == Os::Task::State::NOT_STARTED;
}


void Os::Test::Task::Tester::Start::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: Start \n");
    TaskString name("StartRuleTask");
    Os::Task::Arguments arguments(name, &run_test, &state.m_notification);
    state.m_task.start(arguments);

    bool done = false;
    FwSizeType i = 0;
    for (i = 0; i < 100 and not done; i++) {
        Os::Task::delay(Fw::Time(0, 1000));
        state.m_notification.m_lock.lock();
        done = state.m_notification.m_notification;
        state.m_notification.m_lock.unlock();

        if (state.m_task.isCooperative()) {
            // TODO: run
        }
    }
    ASSERT_LT(i, 100) << "Task did not run within 100ms";

}

