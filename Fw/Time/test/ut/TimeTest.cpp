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

    // Try various operations

    time1.set(1000,1000);
    time2.set(4000,2000);

    // test add


    // test subtract

    // normal subtract
    Fw::Time time3 = Fw::Time::sub(time2,time1);
    ASSERT_EQ(time3.m_timeBase,TB_NONE);
    ASSERT_EQ(time3.m_timeContext,0);
    ASSERT_EQ(time3.m_seconds,3000);
    ASSERT_EQ(time3.m_useconds,1000);

    // rollover subtract
    time1.set(1,999999);
    time2.set(2,000001);
    time3 = Fw::Time::sub(time2,time1);
    ASSERT_EQ(time3.m_timeBase,TB_NONE);
    ASSERT_EQ(time3.m_timeContext,0);
    EXPECT_EQ(time3.m_seconds,0);
    EXPECT_EQ(time3.m_useconds,2);

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

