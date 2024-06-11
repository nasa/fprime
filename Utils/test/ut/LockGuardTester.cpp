// ======================================================================
// \title  LockGuardTester.hpp
// \author vwong
// \brief  cpp file for LockGuard test harness implementation class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "LockGuardTester.hpp"
#include <ctime>
#include <Os/Task.hpp>

namespace Utils {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LockGuardTester ::
    LockGuardTester()
  {
  }

  LockGuardTester ::
    ~LockGuardTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  struct TaskData {
    Os::Mutex mutex;
    int i;
  };
  void taskMethod(void* ptr)
  {
    TaskData* data = static_cast<TaskData*>(ptr);
    LockGuard guard(data->mutex);
    data->i++;
  }

  void LockGuardTester ::
    testLocking()
  {
    TaskData data;
    data.i = 0;
    Os::Task testTask;
    Os::Task::Status stat;
    Os::TaskString name("TestTask");

    {
      LockGuard guard(data.mutex);
      Os::Task::Arguments arguments(name, taskMethod, &data);
      stat = testTask.start(arguments);
      ASSERT_EQ(stat, Os::Task::OP_OK);
      Os::Task::delay(Fw::Time(0, 100));
      ASSERT_EQ(data.i, 0);
    }
    Os::Task::delay(Fw::Time(0, 100));
    {
      LockGuard guard(data.mutex);
      ASSERT_EQ(data.i, 1);
    }
    stat = testTask.join();
    ASSERT_EQ(stat, Os::Task::OP_OK);
  }


  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void LockGuardTester ::
    initComponents()
  {
  }

} // end namespace Utils
