// ======================================================================
// \title  TestState.hpp
// \brief  Shadow state model for ModeManager rule-based testing
//
// The shadow is a minimal model of what the component's current mode
// should be after any sequence of rule actions. Rule preconditions read
// it; rule actions update it in lockstep with the assertions made
// against the real component.
// ======================================================================

#ifndef Svc_ModeManager_ModeManagerTestState_HPP
#define Svc_ModeManager_ModeManagerTestState_HPP

#include "Svc/ModeManager/ModeManager.hpp"

namespace Svc {

class ModeManagerTestState {
  public:
    //! Mirrors the component's current mode
    Mode shadow_currentMode = Mode::STARTUP;
};

}  // namespace Svc

#endif
