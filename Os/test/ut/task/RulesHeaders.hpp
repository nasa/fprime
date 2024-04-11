

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__

#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "Os/Task.hpp"
#include "Os/Mutex.hpp"
#include <vector>

namespace Os {
namespace Test {
namespace Task {



struct TestTaskInfo {
    //! Test task life-cycle stages
    enum Lifecycle {
        BEGINNING = 0, //!< The initial stage of the task
        MIDDLE = 1, //!< The task function has taken hold
        END = 2, //!< The task has been asked to exit
        UNSET = -1,
        JOINED = -2, //!< Joining task has successfully joined
    };

    Lifecycle m_stage = Lifecycle::BEGINNING;
    Os::Mutex m_lock;
    Os::Task::State m_state = Os::Task::State::NOT_STARTED; //!< Shadow state of the task
    Os::Task m_task; //!< Task under test
    std::shared_ptr<TestTaskInfo> m_other = nullptr;
    bool m_signal = false;

    ~TestTaskInfo();

    //! Atomically step through lifecycle stages
    void step();

    //! Signal a step through lifecycle stage
    void signal();

    //! Get lifecycle stage
    Lifecycle stage();

    //! Stop and join thread
    void stop();


    //! Standard task implementation which waits at each lifecycle stage
    static void standard_task(void* argument);

    //! Joining task implementation which waits at each lifecycle stage
    static void joining_task(void* argument);
};

struct Tester {
  private:
    static constexpr FwSizeType MAX_THREAD_COUNT = 100;

    std::vector<std::shared_ptr<TestTaskInfo>> m_tasks;

  public:
#include "TaskRules.hpp"
};

}
}
}


#endif
