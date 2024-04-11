// ======================================================================
// \title Os/test/ut/task/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Task.hpp"
#include "Os/test/ut/task/RulesHeaders.hpp"

namespace Os {
namespace Test {
namespace Task {
    void run_test(void *argument) {
        RunNotification &run_notification = *reinterpret_cast<RunNotification *>(argument);
        run_notification.m_lock.lock();
        run_notification.m_notification = true;
        run_notification.m_lock.unlock();
    }
}
}
}

// Ensure that open mode changes work reliably
TEST(Functionality, StartTask) {
    Os::Test::Task::Tester tester;
    Os::Test::Task::Tester::Start rule;
    rule.apply(tester);
}


