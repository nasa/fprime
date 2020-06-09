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
#include <time.h>
#include <Os/Task.hpp>
#include <Fw/Types/EightyCharString.hpp>

namespace Utils {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  LockGuardTester ::
    LockGuardTester(void)
  {
  }

  LockGuardTester ::
    ~LockGuardTester(void) 
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
    TaskData* data = (TaskData*)ptr;
    LockGuard guard(data->mutex);
    data->i++;
  }

  void LockGuardTester ::
    testLocking(void) 
  {
    TaskData data;
    data.i = 0;
    Os::Task testTask;
    Fw::EightyCharString name("TestTask");

    {
      LockGuard guard(data.mutex);
      Os::Task::TaskStatus stat = testTask.start(name,12,100,10*1024,taskMethod,(void*) &data);
      ASSERT_EQ(stat, Os::Task::TASK_OK);
      Os::Task::delay(100);
      ASSERT_EQ(data.i, 0);
    }
    Os::Task::delay(100);
    {
      LockGuard guard(data.mutex);
      ASSERT_EQ(data.i, 1);
    }
  }


  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void LockGuardTester ::
    initComponents(void) 
  {
  }

} // end namespace Utils
