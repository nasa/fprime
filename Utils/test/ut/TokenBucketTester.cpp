// ======================================================================
// \title  TokenBucketTester.hpp
// \author vwong
// \brief  cpp file for TokenBucket test harness implementation class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "TokenBucketTester.hpp"
#include <ctime>

namespace Utils {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  TokenBucketTester ::
    TokenBucketTester()
  {
  }

  TokenBucketTester ::
    ~TokenBucketTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void TokenBucketTester ::
    testTriggering()
  {
    const U32 interval = 1000000;
    U32 testMaxTokens[] = {1, 5, 50, 832};
    for (U32 i = 0; i < FW_NUM_ARRAY_ELEMENTS(testMaxTokens); i++) {
      const U32 maxTokens = testMaxTokens[i];
      TokenBucket bucket(interval, maxTokens);

      // can activate maxTokens times in a row
      for (U32 j = 0; j < maxTokens; j++) {
        bool triggered = bucket.trigger(Fw::Time(0, 0));
        ASSERT_TRUE(triggered);
        ASSERT_EQ(bucket.getTokens(), maxTokens - j - 1);
      }

      // replenish
      bucket.replenish();

      Fw::Time time(0, 0);
      const U32 attempts = maxTokens * 5;
      Fw::Time attemptInterval(0, interval / 4);
      U32 triggerCount = 0;
      for (U32 attempt = 0; attempt < attempts; attempt++) {
        triggerCount += bucket.trigger(time);
        time = Fw::Time::add(time, attemptInterval);
      }
      U32 expected = maxTokens + (attempts - 1) / 4;
      ASSERT_EQ(expected, triggerCount);
    }
  }

  void TokenBucketTester ::
    testReconfiguring()
  {
    U32 initialInterval = 1000000;
    U32 initialMaxTokens = 5;

    TokenBucket bucket(initialInterval, initialMaxTokens);
    ASSERT_EQ(bucket.getReplenishInterval(), initialInterval);
    ASSERT_EQ(bucket.getMaxTokens(), initialMaxTokens);
    ASSERT_EQ(bucket.getTokens(), initialMaxTokens);

    // trigger
    bucket.trigger(Fw::Time(0, 0));
    ASSERT_EQ(bucket.getTokens(), initialMaxTokens-1);

    // replenished, then triggered
    bucket.trigger(Fw::Time(1, 0));
    ASSERT_EQ(bucket.getTokens(), initialMaxTokens-1);

    // set new interval, can't replenish using old interval
    U32 newInterval = 2000000;
    bucket.setReplenishInterval(newInterval);
    ASSERT_EQ(bucket.getReplenishInterval(), newInterval);
    ASSERT_TRUE(bucket.trigger(Fw::Time(2, 0)));
    ASSERT_EQ(bucket.getTokens(), initialMaxTokens-2);

    // set new max tokens, replenish up to new max
    U32 newMaxTokens = 10;
    bucket.setMaxTokens(newMaxTokens);
    ASSERT_EQ(bucket.getMaxTokens(), newMaxTokens);
    ASSERT_TRUE(bucket.trigger(Fw::Time(20, 0)));
    ASSERT_EQ(bucket.getTokens(), newMaxTokens-1);

    // set new rate, replenish quickly
    while (bucket.trigger(Fw::Time(0,0)));
    bucket.setReplenishInterval(1000000);
    U32 newRate = 2;
    bucket.setReplenishRate(newRate);
    ASSERT_EQ(bucket.getReplenishRate(), newRate);
    ASSERT_TRUE(bucket.trigger(Fw::Time(21, 0)));
    ASSERT_EQ(bucket.getTokens(), 1);
  }

  void TokenBucketTester ::
    testInitialSettings()
  {
    U32 interval = 1000000;
    U32 maxTokens = 5;
    U32 rate = 2;
    U32 startTokens = 2;
    Fw::Time startTime(5,0);

    TokenBucket bucket(interval, maxTokens, rate, startTokens, startTime);
    ASSERT_NE(bucket.getTokens(), maxTokens);
    ASSERT_EQ(bucket.getTokens(), startTokens);
    ASSERT_EQ(bucket.getReplenishRate(), rate);

    for (U32 i = 0; i < startTokens; i++) {
      bool triggered = bucket.trigger(Fw::Time(0,0));
      ASSERT_TRUE(triggered);
    }
    ASSERT_FALSE(bucket.trigger(Fw::Time(0,0)));
  }


  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void TokenBucketTester ::
    initComponents()
  {
  }

} // end namespace Utils
