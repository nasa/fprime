module Svc {

    @ Request a transition to a given mode. Provenance is stamped by
    @ ModeManager from the port index; the caller cannot supply it.
    port ModeRequestPort(
        target: Mode @< The requested mode
    )

    @ Query the current mode
    port GetMode -> Mode

    @ Notify a subscriber that the mode changed
    port ModeChanged(
        fromMode: Mode @< The previous mode
        toMode: Mode @< The new mode
    )

    @ Ask an external policy whether a transition is permitted
    port CheckTransition(
        current: Mode @< The current mode
        target: Mode @< The requested mode
        req: ModeRequest @< Who requested it
    ) -> Fw.Success

    @ Holds the current spacecraft mode, applies a configurable transition
    @ policy, and notifies subscribers on change.
    @
    @ Performs no fault detection, isolation, or response. It accepts a
    @ request to enter a mode; it never decides that a mode is warranted.
    active component ModeManager {

        # ----------------------------------------------------------------------
        # State machine
        # ----------------------------------------------------------------------

        @ Mode state machine. Defined in the config module; see
        @ docs/sdd.md for the contract an override must satisfy.
        state machine instance modeSm: ModeMachine

        # ----------------------------------------------------------------------
        # Mode interface
        # ----------------------------------------------------------------------

        @ Transition requests from other components. The port index
        @ identifies the requester. Index 0 is reserved: Requester::NONE
        @ (value 0) means "no component, this came from ground", so no
        @ real component may be wired there. A request arriving on index 0
        @ is rejected without ever being turned into a ModeRequest.
        async input port requestMode: [ModeManagerCfg.NUM_REQUESTERS] ModeRequestPort

        @ Current mode query. Guarded because it executes in the caller's
        @ thread and reads state this component's thread may be writing.
        guarded input port getMode: GetMode

        @ Mode change notification to subscribers
        output port modeChanged: [ModeManagerCfg.NUM_SUBSCRIBERS] ModeChanged

        @ Optional external transition policy. When unconnected, the
        @ default policy in the config module decides.
        output port checkTransition: CheckTransition

        # ----------------------------------------------------------------------
        # Health
        # ----------------------------------------------------------------------

        @ Ping input port
        async input port pingIn: Svc.Ping

        @ Ping output port
        output port pingOut: Svc.Ping

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        @ Leave STARTUP and begin operations
        async command START

        @ Request a transition to the given mode
        async command REQUEST_MODE(
            target: Mode @< The requested mode
        )

        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------

        @ The current spacecraft mode
        telemetry CurrentMode: Mode

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ A mode transition completed
        event ModeTransitioned(
            fromMode: Mode @< The previous mode
            toMode: Mode @< The new mode
            source: ModeRequestSource @< Ground or component
            requester: Requester @< Which component, if any
        ) \
          severity activity high \
          format "Mode {} -> {} (requested by {}/{})"

        @ A transition request was rejected by the active policy
        event TransitionRejected(
            current: Mode @< The unchanged current mode
            requested: Mode @< The mode that was requested
            source: ModeRequestSource @< Ground or component
            requester: Requester @< Which component, if any
            decidedBy: ModePolicySource @< Default or external policy
        ) \
          severity warning low \
          format "Transition {} -> {} rejected (from {}/{}, decided by {})"

        # ----------------------------------------------------------------------
        # Standard ports
        # ----------------------------------------------------------------------

        @ Time get port
        time get port timeCaller

        @ Command registration port
        command reg port cmdRegOut

        @ Command receive port
        command recv port cmdIn

        @ Command response port
        command resp port cmdResponseOut

        @ Event port
        event port logOut

        @ Text event port
        text event port logTextOut

        @ Telemetry port
        telemetry port tlmOut

    }

}
