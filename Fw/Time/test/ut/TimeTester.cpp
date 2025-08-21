#include "TimeTester.hpp"
#include <iostream>

namespace Fw {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TimeTester::TimeTester() {}

TimeTester::~TimeTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TimeTester::test_InstantiateTest() {
    Fw::Time time(TimeBase::TB_NONE, 1, 2);
    ASSERT_EQ(time.getTimeBase(), TimeBase::TB_NONE);
    ASSERT_EQ(time.getContext(), 0);
    ASSERT_EQ(time.getSeconds(), 1);
    ASSERT_EQ(time.getUSeconds(), 2);
    std::cout << time << std::endl;
}

void TimeTester::test_MathTest() {
    Fw::Time time1;
    Fw::Time time2;

    // Comparison
    time1.set(1000, 1000);
    time2.set(1000, 1000);
    ASSERT_TRUE(time1 == time2);
    ASSERT_TRUE(time1 >= time2);
    ASSERT_TRUE(time1 <= time2);

    time1.set(1000, 1000);
    time2.set(2000, 1000);
    ASSERT_TRUE(time1 != time2);
    ASSERT_TRUE(time1 < time2);
    ASSERT_TRUE(time1 <= time2);

    time1.set(2000, 1000);
    time2.set(1000, 1000);
    ASSERT_TRUE(time1 > time2);
    ASSERT_TRUE(time1 >= time2);

    // Addition
    time1.set(1000, 1000);
    time2.set(4000, 2000);
    Fw::Time time_sum = Fw::Time::add(time1, time2);
    ASSERT_EQ(time_sum.getSeconds(), 5000);
    ASSERT_EQ(time_sum.getUSeconds(), 3000);

    // Normal subtraction
    time1.set(1000, 1000);
    time2.set(4000, 2000);
    Fw::Time time3 = Fw::Time::sub(time2, time1);
    ASSERT_EQ(time3.getTimeBase(), TimeBase::TB_NONE);
    ASSERT_EQ(time3.getContext(), 0);
    ASSERT_EQ(time3.getSeconds(), 3000);
    ASSERT_EQ(time3.getUSeconds(), 1000);

    // Rollover subtraction
    time1.set(1, 999999);
    time2.set(2, 000001);
    time3 = Fw::Time::sub(time2, time1);
    ASSERT_EQ(time3.getTimeBase(), TimeBase::TB_NONE);
    ASSERT_EQ(time3.getContext(), 0);
    EXPECT_EQ(time3.getSeconds(), 0);
    EXPECT_EQ(time3.getUSeconds(), 2);

    // Addition - context is the same
    time1.set(1000, 0);
    time2.set(2000, 500);
    time1.setTimeContext(2);
    time2.setTimeContext(2);
    time_sum = Fw::Time::add(time1, time2);
    EXPECT_EQ(time_sum.getContext(), 2);
    EXPECT_EQ(time_sum.getSeconds(), 3000);
    EXPECT_EQ(time_sum.getUSeconds(), 500);

    // Addition - context differs
    time1.set(1000, 0);
    time2.set(2000, 500);
    time1.setTimeContext(1);
    time2.setTimeContext(2);
    time_sum = Fw::Time::add(time1, time2);
    EXPECT_EQ(time_sum.getContext(), 0);
    EXPECT_EQ(time_sum.getSeconds(), 3000);
    EXPECT_EQ(time_sum.getUSeconds(), 500);

    // Sub - context is the same
    time1.set(3000, 0);
    time2.set(1500, 0);
    time1.setTimeContext(3);
    time2.setTimeContext(3);
    time_sum = Fw::Time::sub(time1, time2);
    EXPECT_EQ(time_sum.getContext(), 3);
    EXPECT_EQ(time_sum.getSeconds(), 1500);

    // Sub - context differs
    time1.set(3000, 0);
    time2.set(1500, 0);
    time1.setTimeContext(3);
    time2.setTimeContext(2);
    time_sum = Fw::Time::sub(time1, time2);
    EXPECT_EQ(time_sum.getContext(), 0);
    EXPECT_EQ(time_sum.getSeconds(), 1500);
}

void TimeTester::test_CopyTest() {
    Fw::Time time0;

    // make time that's guaranteed to be different from default
    Fw::Time time1((time0.getTimeBase() != TimeBase::TB_NONE ? TimeBase::TB_NONE : TimeBase::TB_PROC_TIME),
                   static_cast<FwTimeContextStoreType>(time0.getContext() + 1), time0.getSeconds() + 1,
                   time0.getUSeconds() + 1);

    // copy construction
    Fw::Time time2 = time1;
    ASSERT_EQ(time1.getSeconds(), time2.getSeconds());
    ASSERT_EQ(time1.getUSeconds(), time2.getUSeconds());
    ASSERT_EQ(time1.getTimeBase(), time2.getTimeBase());
    ASSERT_EQ(time1.getContext(), time2.getContext());

    // assignment operator
    Fw::Time time3;
    time3 = time1;
    ASSERT_EQ(time1.getSeconds(), time3.getSeconds());
    ASSERT_EQ(time1.getUSeconds(), time3.getUSeconds());
    ASSERT_EQ(time1.getTimeBase(), time3.getTimeBase());
    ASSERT_EQ(time1.getContext(), time3.getContext());

    // set method
    Fw::Time time4;
    time4.set(time1.getTimeBase(), time1.getContext(), time1.getSeconds(), time1.getUSeconds());
    ASSERT_EQ(time1.getSeconds(), time3.getSeconds());
    ASSERT_EQ(time1.getUSeconds(), time3.getUSeconds());
    ASSERT_EQ(time1.getTimeBase(), time3.getTimeBase());
    ASSERT_EQ(time1.getContext(), time3.getContext());
}

void TimeTester::test_ZeroTimeEquality() {
    Fw::Time time(TimeBase::TB_PROC_TIME, 1, 2);
    ASSERT_NE(time, Fw::ZERO_TIME);
    Fw::Time time2;
    ASSERT_EQ(time2, Fw::ZERO_TIME);
}

}  // namespace Fw
