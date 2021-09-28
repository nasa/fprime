// ======================================================================
// \title  RateLimiterTester.hpp
// \author vwong
// \brief  cpp file for RateLimiter test harness implementation class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "RateLimiterTester.hpp"
#include <ctime>

namespace Utils {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  RateLimiterTester ::
    RateLimiterTester()
  {
  }

  RateLimiterTester ::
    ~RateLimiterTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void RateLimiterTester ::
    testCounterTriggering()
  {
    U32 testCycles[] = {0, 5, 50, 832};
    for (U32 i = 0; i < FW_NUM_ARRAY_ELEMENTS(testCycles); i++) {
      const U32 cycles = testCycles[i];

      // triggers at the beginning
      RateLimiter limiter(cycles, 0);
      ASSERT_TRUE(limiter.trigger());
      limiter.reset();

      // does not trigger if skipped
      if (cycles > 0) {
        limiter.setCounter(1);
        ASSERT_FALSE(limiter.trigger());
        limiter.reset();
      }

      // test number of times triggered
      const U32 numIter = 10000;
      U32 triggerCount = 0;
      for (U32 iter = 0; iter < numIter; iter++) {
        bool shouldTrigger = (cycles == 0) || (iter % cycles == 0);
        bool triggered = limiter.trigger();
        ASSERT_EQ(shouldTrigger, triggered) << " for cycles " << cycles << " at " << iter;
        triggerCount += triggered;
      }
      if (cycles > 0) {
        U32 expectedCount = (numIter / cycles) + (numIter % cycles > 0);
        ASSERT_EQ(triggerCount, expectedCount);
      }
    }
  }

  void RateLimiterTester ::
    testTimeTriggering()
  {
    U32 testCycles[] = {0, 5, 50, 832};
    for (U32 i = 0; i < FW_NUM_ARRAY_ELEMENTS(testCycles); i++) {
      const U32 cycles = testCycles[i];
      Fw::Time timeCyclesTime(cycles, 0);

      // triggers at the beginning
      RateLimiter limiter(0, cycles);
      ASSERT_TRUE(limiter.trigger(Fw::Time::zero()));
      limiter.reset();

      // does not trigger if skipped
      if (cycles > 0) {
        limiter.setTime(Fw::Time(1,0));
        ASSERT_FALSE(limiter.trigger(Fw::Time::zero()));
        limiter.reset();
      }

      // test number of times triggered
      const U32 numIter = 100000;
      Fw::Time curTime(0, 0);
      Fw::Time nextTriggerTime(0, 0);
      srand(0x30931842);
      for (U32 iter = 0; iter < numIter; iter++) {
        curTime.add(0, (rand() % 5 + 1) * 100000);
        bool shouldTrigger = (cycles == 0) || (curTime >= nextTriggerTime);
        bool triggered = limiter.trigger(curTime);
        ASSERT_EQ(shouldTrigger, triggered) << " for cycles " << cycles << " at " << curTime.getSeconds() << "." << curTime.getUSeconds();
        if (triggered) {
          nextTriggerTime = Fw::Time::add(curTime, timeCyclesTime);
        }
      }
    }
  }

  void RateLimiterTester ::
    testCounterAndTimeTriggering()
  {
    RateLimiter limiter;

    U32 testCounterCycles[] = {37, 981, 4110};
    U32 testTimeCycles[] = {12, 294, 1250};
    for (U32 i = 0; i < (FW_NUM_ARRAY_ELEMENTS(testCounterCycles) * FW_NUM_ARRAY_ELEMENTS(testTimeCycles)); i++) {
      const U32 counterCycles = testCounterCycles[i % FW_NUM_ARRAY_ELEMENTS(testCounterCycles)];
      const U32 timeCycles = testTimeCycles[i / FW_NUM_ARRAY_ELEMENTS(testCounterCycles)];
      Fw::Time timeCyclesTime(timeCycles, 0);
      limiter.setCounterCycle(counterCycles);
      limiter.setTimeCycle(timeCycles);
      limiter.reset();

      // triggers at the beginning
      RateLimiter limiter(counterCycles, timeCycles);
      ASSERT_TRUE(limiter.trigger(Fw::Time::zero()));
      limiter.reset();

      // test trigger locations
      const U32 numIter = 100000; // each iter is 0.1 seconds
      Fw::Time curTime(0, 0);
      U32 lastTriggerIter = 0;
      Fw::Time nextTriggerTime(0, 0);
      srand(0x28E1ACC2);
      for (U32 iter = 0; iter < numIter; iter++) {
        curTime.add(0, (rand() % 5 + 1) * 100000);
        bool shouldTrigger = ((iter-lastTriggerIter) % counterCycles == 0) || (curTime >= nextTriggerTime);
        bool triggered = limiter.trigger(curTime);
        ASSERT_EQ(shouldTrigger, triggered) << " for cycles " << counterCycles << "/" << timeCycles << " at " << iter << "/" << curTime.getSeconds() << "." << curTime.getUSeconds();
        if (triggered) {
          nextTriggerTime = Fw::Time::add(curTime, timeCyclesTime);
          lastTriggerIter = iter;
        }
      }
    }
  }


  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void RateLimiterTester ::
    initComponents()
  {
  }

} // end namespace Utils
