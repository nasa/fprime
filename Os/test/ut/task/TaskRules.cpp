

#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Types/String.hpp"


void wait_for_state_with_timeout(Os::Test::Task::TestTaskInfo& info, const Os::Test::Task::TestTaskInfo::Lifecycle& stage, const FwSizeType delay_ms) {
    // Loop waiting for transition in preparation for join (with timeout)
    FwSizeType i = 0;
    for (i = 0; i < (delay_ms * 10); i++) {
        Os::Task::delay(Fw::Time(0, 100));
        if (info.stage() == stage) {
            break;
        }
        // Run cooperative multitasking when necessary
        if (info.m_task.isCooperative()) {
            // TODO: run
        }
    }
    ASSERT_LT(i, (delay_ms * 10)) << "Task did not run within " << delay_ms << "ms";
    ASSERT_EQ(info.stage(), stage) << "Task lifecycle inconsistent";
}

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
    return state.m_tasks.size() < Os::Test::Task::Tester::MAX_THREAD_COUNT;
}


void Os::Test::Task::Tester::Start::action(
        Os::Test::Task::Tester &state //!< The test state
) {
    printf("--> Rule: Start \n");

    std::shared_ptr<TestTaskInfo> new_task = std::make_shared<TestTaskInfo>();
    state.m_tasks.push_back(new_task);


    Fw::String name("StartRuleTask");
    Os::Task::Arguments arguments(name, &TestTaskInfo::standard_task, &(*new_task));
    new_task->m_task.start(arguments);
    // Ensure it starts in the correct state
    ASSERT_EQ(new_task->stage(), TestTaskInfo::Lifecycle::BEGINNING);

    // Poke the task into the MIDDLE state
    new_task->signal();
    wait_for_state_with_timeout(*new_task, TestTaskInfo::MIDDLE, 100);
}


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  Join
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Test::Task::Tester::Join::Join() :
        STest::Rule<Os::Test::Task::Tester>("Join")
  {
  }


  bool Os::Test::Task::Tester::Join::precondition(
            const Os::Test::Task::Tester& state //!< The test state
        ) 
  {
      return not state.m_tasks.empty();
  }

  
  void Os::Test::Task::Tester::Join::action(
            Os::Test::Task::Tester& state //!< The test state
        ) 
  {
      printf("--> Rule: Join\n");
      TestTaskInfo joiner_task;
      const U32 random_index = STest::Pick::lowerUpper(0, state.m_tasks.size() - 1);

      // Pop the middle
      std::shared_ptr<TestTaskInfo> other_task = state.m_tasks[random_index];
      state.m_tasks.erase(state.m_tasks.begin() + random_index);

      // Set the other argument
      joiner_task.m_other = other_task;

      Fw::String name("JoinTask");
      Os::Task::Arguments arguments(name, &TestTaskInfo::joining_task, &joiner_task);
      joiner_task.m_task.start(arguments);

      // Ensure it starts in the correct state for joining
      ASSERT_EQ(other_task->stage(), TestTaskInfo::Lifecycle::MIDDLE);

      // Poke the task into the MIDDLE state
      joiner_task.signal();
      wait_for_state_with_timeout(joiner_task, TestTaskInfo::MIDDLE, 100);

      // Make the other task move from MIDDLE state to end state and wait for JOINED state
      other_task->signal();
      wait_for_state_with_timeout(joiner_task, TestTaskInfo::END, 100);
  }