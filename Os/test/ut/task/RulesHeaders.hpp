

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__

#include "STest/STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "TaskRules.hpp"
#include "Os/Task.hpp"
#include "Os/Mutex.hpp"

namespace Os {
namespace Test {
namespace Task {

struct RunNotification {
    Os::Mutex m_lock;
    bool m_notification = false;
};

void run_test(void* argument) {
    RunNotification& run_notification = *reinterpret_cast<RunNotification*>(argument);
    run_notification.m_lock.lock();
    run_notification.m_notification = true;
    run_notification.m_lock.unlock();
}


struct Tester {
    Tester() : m_arguments("MyTask", &run_test, &m_notification) {}

  private:
    Os::Task::Arguments m_arguments;
    Os::Task::State m_state = Os::Task::State::NOT_STARTED;
    Os::Task m_task;
    RunNotification m_notification;


#include "TaskRules.hpp"
};

}
}
}


#endif
