module Svc {
    @ A component for retrying message delivery on failure
    active component ComRetry {
        import Svc.Framer

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        @ TODO
        async input port TODO: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32
    }
}