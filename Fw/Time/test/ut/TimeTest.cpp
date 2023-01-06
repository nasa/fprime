/*
 * TimeTest.cpp
 *
 *  Created on: Apr 22, 2016
 *      Author: tcanham
 */

#include <Fw/Time/Time.hpp>
#include <iostream>
#include <gtest/gtest.h>

TEST(TimeTestNominal,InstantiateTest) {

    Fw::Time time(TB_NONE,1,2);
    ASSERT_EQ(time.m_timeBase,TB_NONE);
    ASSERT_EQ(time.m_timeContext,0);
    ASSERT_EQ(time.m_seconds,1);
    ASSERT_EQ(time.m_useconds,2);
    std::cout << time << std::endl;
}

TEST(TimeTestNominal,MathTest) {

    Fw::Time time1;
    Fw::Time time2;

    // Comparison
    time1.set(1000,1000);
    time2.set(1000,1000);
    ASSERT_TRUE(time1 == time2);
    ASSERT_TRUE(time1 >= time2);
    ASSERT_TRUE(time1 <= time2);

    time1.set(1000,1000);
    time2.set(2000,1000);
    ASSERT_TRUE(time1 != time2);
    ASSERT_TRUE(time1 < time2);
    ASSERT_TRUE(time1 <= time2);

    time1.set(2000,1000);
    time2.set(1000,1000);
    ASSERT_TRUE(time1 > time2);
    ASSERT_TRUE(time1 >= time2);

    // Addition
    time1.set(1000,1000);
    time2.set(4000,2000);
    Fw::Time time_sum = Fw::Time::add(time1,time2);
    ASSERT_EQ(time_sum.m_seconds,5000);
    ASSERT_EQ(time_sum.m_useconds,3000);

    // Normal subtraction
    time1.set(1000,1000);
    time2.set(4000,2000);
    Fw::Time time3 = Fw::Time::sub(time2,time1);
    ASSERT_EQ(time3.m_timeBase,TB_NONE);
    ASSERT_EQ(time3.m_timeContext,0);
    ASSERT_EQ(time3.m_seconds,3000);
    ASSERT_EQ(time3.m_useconds,1000);

    // Rollover subtraction
    time1.set(1,999999);
    time2.set(2,000001);
    time3 = Fw::Time::sub(time2,time1);
    ASSERT_EQ(time3.m_timeBase,TB_NONE);
    ASSERT_EQ(time3.m_timeContext,0);
    EXPECT_EQ(time3.m_seconds,0);
    EXPECT_EQ(time3.m_useconds,2);

}

TEST(TimeTestNominal,CopyTest) {

    Fw::Time time0;

    // make time that's guaranteed to be different from default
    Fw::Time time1(
        (time0.getTimeBase() != TB_NONE ? TB_NONE : TB_PROC_TIME),
        time0.getContext()+1,
        time0.getSeconds()+1,
        time0.getUSeconds()+1
    );

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

TEST(TimeTestNominal,ZeroTimeEquality) {
    Fw::Time time(TB_PROC_TIME,1,2);
    ASSERT_NE(time, Fw::ZERO_TIME);
    Fw::Time time2;
    ASSERT_EQ(time2, Fw::ZERO_TIME);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

