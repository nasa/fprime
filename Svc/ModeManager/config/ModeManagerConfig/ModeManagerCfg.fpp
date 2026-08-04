# ======================================================================
# ModeManagerCfg.fpp
#
# PROJECT CONFIGURATION. Override this file to define your mission's
# modes and requesters.
#
# See Svc/ModeManager/docs/sdd.md for the full contract that an
# override must satisfy.
# ======================================================================

module Svc {

    @ Spacecraft modes. ModeMachine must declare one state per member.
    enum Mode {
        STARTUP
        IDLE
        SCIENCE
        SAFE
    }

    @ Where a transition request originated
    enum ModeRequestSource {
        GROUND
        COMPONENT
    }

    @ Which component requested a transition. Each value MUST equal the
    @ index of that component's requestMode port.
    enum Requester {
        NONE = 0     @< Used when the source is GROUND
        THERMAL = 1
        PAYLOAD = 2
        FDIR = 3
    }

    @ Provenance of a transition request. Stamped by ModeManager from the
    @ entry point used; never supplied by the caller.
    struct ModeRequest {
        source: ModeRequestSource
        requester: Requester
    }

    @ Which policy decided a transition
    enum ModePolicySource {
        DEFAULT
        EXTERNAL
    }

}

module ModeManagerCfg {

    @ Size of the requestMode port array
    constant NUM_REQUESTERS = 4

    @ Size of the modeChanged port array
    constant NUM_SUBSCRIBERS = 8

}
