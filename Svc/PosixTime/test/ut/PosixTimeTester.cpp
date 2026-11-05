// ----------------------------------------------------------------------
// PosixTime/test/ut/Tester.cpp
// ----------------------------------------------------------------------

#include <strings.h>
#include <cstdio>
#include <limits>

#include <STest/Pick/Pick.hpp>

#include "PosixTimeTester.hpp"

#define INSTANCE 0

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

PosixTimeTester ::PosixTimeTester(const char* const compName)
    : PosixTimeGTestBase(compName, 0), component("PosixTime") {
    this->init();
    this->component.init(INSTANCE);
    this->connect_to_timeGetPort(0, this->component.get_timeGetPort_InputPort(0));
}

PosixTimeTester ::~PosixTimeTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void PosixTimeTester ::getTime() {
    Fw::Time time;
    this->invoke_to_timeGetPort(0, time);
    ASSERT_EQ(time.getTimeBase(), TimeBase::TB_WORKSTATION_TIME);
    ASSERT_EQ(time.getContext(), 0);
    ASSERT_GT(time.getSeconds(), 0U);
    ASSERT_GE(time.getUSeconds(), 0U);
    ASSERT_LE(time.getUSeconds(), 999999U);
}

void PosixTimeTester ::getTimeWithContext() {
    // Pick a nonzero context so that it is distinguishable from the default
    const U32 upper = std::numeric_limits<FwTimeContextStoreType>::max();
    const FwTimeContextStoreType context = static_cast<FwTimeContextStoreType>(STest::Pick::lowerUpper(1, upper));
    this->component.setTimeContext(context);
    Fw::Time time;
    this->invoke_to_timeGetPort(0, time);
    ASSERT_EQ(time.getTimeBase(), TimeBase::TB_WORKSTATION_TIME);
    ASSERT_EQ(time.getContext(), context);
    ASSERT_GT(time.getSeconds(), 0U);
    ASSERT_GE(time.getUSeconds(), 0U);
    ASSERT_LE(time.getUSeconds(), 999999U);
}

}  // namespace Svc
