# ======================================================================
# ModeMachine.fpp
#
# PROJECT CONFIGURATION. Override this file to define your mission's
# mode graph.
#
# CONTRACT -- the shipped ModeManager C++ requires all of:
#   * a state machine named exactly   ModeMachine
#   * an action named exactly         announce: Svc.ModeRequest
#   * one state per member of         Svc.Mode
#   * one signal per requestable mode, listed in ModeList.inc
#   * every state accepting every signal (see below)
#
# THIS GRAPH IS DELIBERATELY PERMISSIVE. Transition legality is decided
# by ModeManagerCfg::defaultPolicy, or by an external policy component
# on the checkTransition port, BEFORE any signal reaches this machine.
# A missing clause here silently drops a transition the policy already
# approved -- it does not reject it.
# ======================================================================

module Svc {

    @ Mode state machine. Records the current mode and announces changes.
    @ Enforces no policy of its own.
    state machine ModeMachine {

        @ STARTUP performs no output. The initial transition runs during
        @ init(), before topology connections exist.
        initial enter STARTUP

        @ Transition to IDLE
        signal goToIdle: ModeRequest

        @ Transition to SCIENCE
        signal goToScience: ModeRequest

        @ Transition to SAFE
        signal goToSafe: ModeRequest

        @ Record the new mode, emit telemetry and the transition event,
        @ and notify all connected subscribers
        action announce: ModeRequest

        state STARTUP {
            on goToIdle    do { announce } enter IDLE
            on goToScience do { announce } enter SCIENCE
            on goToSafe    do { announce } enter SAFE
        }

        state IDLE {
            on goToIdle    do { announce } enter IDLE
            on goToScience do { announce } enter SCIENCE
            on goToSafe    do { announce } enter SAFE
        }

        state SCIENCE {
            on goToIdle    do { announce } enter IDLE
            on goToScience do { announce } enter SCIENCE
            on goToSafe    do { announce } enter SAFE
        }

        state SAFE {
            on goToIdle    do { announce } enter IDLE
            on goToScience do { announce } enter SCIENCE
            on goToSafe    do { announce } enter SAFE
        }

    }

}
