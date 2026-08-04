// ======================================================================
// ModeList.inc
//
// PROJECT CONFIGURATION. Maps each requestable mode to the ModeMachine
// signal that enters it.
//
//     MODE_ENTRY(<Svc::Mode member>, <ModeMachine signal>)
//
// One line per member of Svc::Mode that may be requested. Note that
// STARTUP has no entry: it is the initial state and nothing transitions
// *to* it. A request to enter a mode with no entry here is rejected as
// an unsupported target, not asserted on -- a ground operator can name
// any valid enum member, including STARTUP.
//
// WHY THIS IS AN INCLUDE AND NOT A HEADER: the generated
// modeSm_sendSignal_* functions are protected members of the component
// base class, so only Svc::ModeManager's own member functions may call
// them. Project-supplied code therefore cannot be an ordinary function;
// it has to be expanded textually inside a member.
//
// Omitting a line for a mode that should be requestable makes that mode
// unreachable at runtime with no build-time diagnostic. See
// Svc/ModeManager/docs/sdd.md section 4.1.
// ======================================================================

MODE_ENTRY(IDLE,    goToIdle)
MODE_ENTRY(SCIENCE, goToScience)
MODE_ENTRY(SAFE,    goToSafe)
