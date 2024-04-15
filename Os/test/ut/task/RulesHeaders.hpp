

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
    };
    static FwSizeType s_task_count;

    Lifecycle m_stage = Lifecycle::UNSET;
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

    //! Start the thread
    void start(Os::Task::taskRoutine routine);

    //! Standard task implementation which waits at each lifecycle stage
    static void standard_task(void* argument);

    //! Joining task implementation which waits at each lifecycle stage
    static void joining_task(void* argument);
};

struct Tester : public Os::TaskRegistry {
  private:
    static constexpr U32 MAX_THREAD_COUNT = 100;
    static constexpr U32 MAX_DELAY_MICRO_SECONDS = 10000;

    std::vector<std::shared_ptr<TestTaskInfo>> m_tasks;
    Os::Task* m_last_task = nullptr;
    std::vector<Os::Task*> m_all_tasks;

  public:
    //! Constructor
    Tester();
    virtual ~Tester();

    //! Get the current tester
    static Tester* getCurrentRegistry();

    //! Invoke cooperative tasks when looping
    void invokeRoutines();

    //! Add task to test registry
    //! \param task: task to add
    void addTask(Os::Task* task) override;

    //! Remove task to test registry
    //! \param task: task to add
    void removeTask(Os::Task* task) override;

    static Tester* s_current_registry;

  public:
#include "TaskRules.hpp"
};

}
}
}


#endif
