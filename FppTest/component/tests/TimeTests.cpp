// ======================================================================
// \title  TimeTests.cpp
// \author T. Chieu
// \brief  cpp file for time tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Time/Time.hpp"
#include "STest/Pick/Pick.hpp"
#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Time test
// ----------------------------------------------------------------------

void Tester ::testTime() {
    Fw::Time time(STest::Pick::any(), STest::Pick::any());
    Fw::Time zero_time(TB_NONE, 0, 0);
    Fw::Time result;

    this->setTestTime(time);

    result = component.getTime();
    ASSERT_EQ(result, zero_time);

    this->connectTimeGetOut();
    ASSERT_TRUE(component.isConnected_timeGetOut_OutputPort(0));

    result = component.getTime();
    ASSERT_EQ(result, time);

    this->connectSpecialPortsSerial();
    ASSERT_TRUE(component.isConnected_timeGetOut_OutputPort(0));

    result = component.getTime();
    ASSERT_EQ(result, time);
}
