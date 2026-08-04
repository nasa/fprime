// ======================================================================
// \title  Ping.cpp
// \brief  Rule implementation for the pingIn/pingOut health-check pair
//
// Unrelated to mode state, exercised interleaved with the mode rules so
// a mode transition mid-flight can't corrupt the ping round trip (MM-020).
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Ping.Check
// ----------------------------------------------------------------------

bool ModeManagerTester ::Ping__Check__precondition() const {
    return true;
}

void ModeManagerTester ::Ping__Check__action() {
    this->clearHistory();

    const U32 key = STest::Pick::any();
    this->invoke_to_pingIn(0, key);
    this->dispatchExactly(1);  // ping never touches the state machine

    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, key);
}

}  // namespace Svc
