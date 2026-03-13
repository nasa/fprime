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

void Fw::TimeTester::test_InstantiateFromFloatTest() {
    Fw::Time time(static_cast<F64>(1.000002));
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

void Fw::TimeTester::test_FloatOperations() {
    Fw::Time time;

    // Set float
    time.set(1000.002000);
    EXPECT_EQ(1000, time.getSeconds());
    EXPECT_EQ(2000, time.getUSeconds());

    // Parse seconds and microseconds
    EXPECT_EQ(1234, Fw::Time::parseSeconds(1234.567890));
    EXPECT_EQ(567890, Fw::Time::parseUSeconds(1234.567890));
    // Rounding check
    EXPECT_EQ(1000000, Fw::Time::parseUSeconds(0.99999999999999));
    EXPECT_EQ(0, Fw::Time::parseUSeconds(0.00000011111111));

    // Add float value
    time.add(2000.004000);
    EXPECT_EQ(time.getSeconds(), 3000);
    EXPECT_EQ(time.getUSeconds(), 6000);
    time.add(0.99999999999999);
    EXPECT_EQ(time.getSeconds(), 3001);
    EXPECT_EQ(time.getUSeconds(), 6000);
    time.add(0.00000011111111);
    EXPECT_EQ(time.getSeconds(), 3001);
    EXPECT_EQ(time.getUSeconds(), 6000);

    // Add assign float value
    time += 1000.005000;
    EXPECT_EQ(time.getSeconds(), 4001);
    EXPECT_EQ(time.getUSeconds(), 11000);

    // Assign float value
    time = 1234.567890;
    EXPECT_EQ(time.getSeconds(), 1234);
    EXPECT_EQ(time.getUSeconds(), 567890);

    // Convert to F64
    EXPECT_EQ(static_cast<F64>(time), 1234.567890);

    // Assert negative value
    ASSERT_DEATH_IF_SUPPORTED(Fw::Time::parseSeconds(-1.0), "Assert:.*Time\\.cpp");
    ASSERT_DEATH_IF_SUPPORTED(Fw::Time::parseUSeconds(-1.0), "Assert:.*Time\\.cpp");
    ASSERT_DEATH_IF_SUPPORTED(time.add(-1.0), "Assert:.*Time\\.cpp");
    ASSERT_DEATH_IF_SUPPORTED(time.operator=(-1.0), "Assert:.*Time\\.cpp");
    ASSERT_DEATH_IF_SUPPORTED(time.operator+=(-1.0), "Assert:.*Time\\.cpp");
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

void TimeTester::test_TimeToTimeValue() {
    U32 seconds = 5;
    U32 useconds = 500000;
    TimeBase timeBase = TimeBase::TB_WORKSTATION_TIME;
    FwTimeContextStoreType context = 3;

    Fw::Time time(timeBase, context, seconds, useconds);

    Fw::TimeValue time_value = time.asTimeValue();

    // Alter the original
    time.set(TimeBase::TB_NONE, 0, 0, 0);

    ASSERT_EQ(time_value.get_timeBase(), timeBase);
    ASSERT_EQ(time_value.get_timeContext(), context);
    ASSERT_EQ(time_value.get_seconds(), seconds);
    ASSERT_EQ(time_value.get_useconds(), useconds);
}

}  // namespace Fw
