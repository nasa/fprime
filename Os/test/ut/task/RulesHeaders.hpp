

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__

#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "Os/Task.hpp"
#include "Os/Mutex.hpp"

namespace Os {
namespace Test {
namespace Task {

struct RunNotification {
    Os::Mutex m_lock;
    bool m_notification = false;
};

void run_test(void* argument);

struct Tester {
  private:
    Os::Task::State m_state = Os::Task::State::NOT_STARTED;
    Os::Task m_task;
    RunNotification m_notification;

  public:
#include "TaskRules.hpp"
};

}
}
}


#endif
