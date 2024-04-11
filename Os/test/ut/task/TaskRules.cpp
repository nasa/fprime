

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
    printf("--> Rule: %s \n", this->name);
    state.m_task.start(state.m_arguments);
    do {

    } while ();
}

