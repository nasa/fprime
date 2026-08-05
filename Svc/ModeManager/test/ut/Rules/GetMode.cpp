// ======================================================================
// \title  GetMode.cpp
// \author Yukti Vijay
// \brief  Rule implementation for the getCurrentMode query port
//
// getCurrentMode is guarded (synchronous with the component's mutex), so no
// doDispatch() is needed -- the call returns before this action
// continues.
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// GetMode.Query
// ----------------------------------------------------------------------

bool ModeManagerTester ::GetMode__Query__precondition() const {
    return true;
}

void ModeManagerTester ::GetMode__Query__action() {
    this->clearHistory();

    const Mode returned = this->invoke_to_getCurrentMode(0);

    ASSERT_EQ(returned.e, this->shadow.shadow_currentMode.e)
        << "getCurrentMode returned a mode that does not match the last known transition";
}

}  // namespace Svc
