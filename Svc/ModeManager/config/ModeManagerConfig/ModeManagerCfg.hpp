// ======================================================================
// ModeManagerCfg.hpp
//
// PROJECT CONFIGURATION. The default transition policy for
// Svc::ModeManager.
//
// AS SHIPPED, THIS PERMITS EVERY TRANSITION. Svc::ModeManager asserts no
// policy of its own, so a deployment that neither overrides this file nor
// connects a policy component to the checkTransition port has an
// unrestricted mode manager: any mode to any mode, from any requester.
//
// To impose mission rules, either:
//   1. Override this file via register_fprime_config(
//        ... CONFIGURATION_OVERRIDES ...), or
//   2. Connect a policy component to checkTransition, which takes
//      precedence over this function whenever it is connected.
//
// See Svc/ModeManager/docs/sdd.md section 4.3.
// ======================================================================

#ifndef SVC_MODEMANAGER_CONFIG_MODEMANAGERCFG_HPP
#define SVC_MODEMANAGER_CONFIG_MODEMANAGERCFG_HPP

#include "Svc/ModeManager/config/ModeManagerConfig/ModeEnumAc.hpp"
#include "Svc/ModeManager/config/ModeManagerConfig/ModeRequestSerializableAc.hpp"

namespace ModeManagerCfg {

//! Default transition policy.
//!
//! Consulted only when the checkTransition output port is unconnected.
//! Returns true if the transition from `current` to `target`, requested by
//! `req`, is permitted.
//!
//! The shipped implementation permits everything. A restricting override
//! looks like:
//!
//!     if (target == Svc::Mode::SAFE) {
//!         return current != Svc::Mode::SAFE;
//!     }
//!     switch (current) {
//!         case Svc::Mode::STARTUP:
//!             return (target == Svc::Mode::IDLE) &&
//!                    (req.get_source() == Svc::ModeRequestSource::GROUND);
//!         ...
//!     }
//!
inline bool defaultPolicy(
    Svc::Mode::T /* current */,
    Svc::Mode::T /* target */,
    const Svc::ModeRequest& /* req */
) {
    // Unrestricted. See the file header and sdd.md section 4.3.
    return true;
}

}  // namespace ModeManagerCfg

#endif  // SVC_MODEMANAGER_CONFIG_MODEMANAGERCFG_HPP
