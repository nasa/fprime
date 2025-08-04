module Svc {
    @ A rate group active component that schedules output port calls at pre-defined start times
    active component ActivePhaser {

        # ----------------------------------------------------------------------
        # General Ports
        # ----------------------------------------------------------------------

        @ The rate group cycle input
        async input port CycleIn: Svc.Cycle drop

        @ Scheduler output port to rate group members
        output port PhaserMemberOut: [ActiveRateGroupOutputPorts] Sched

        @ An internal port for sending data of type T
        internal port Tick drop

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ Warning event that rate group has had a missed deadline
        event MissedDeadline(
                                p: FwIndexType @< Port that is delayed
                                start: U32 @< Start of execution window
                                length: U32 @< Length of the execution window
                                ticks: U32 @< Time in ticks the deadline was late
                            ) \
          severity warning high \
          id 0 \
          format "Port {} schedule at {} for {} ticks was long by {} ticks" \
          throttle 5
        
        # ----------------------------------------------------------------------
        # Telemetry channels
        # ----------------------------------------------------------------------

        @ Max execution time rate group
        telemetry RgMaxTime: U32 id 0 update on change \
        format "{} us"

        @ Cycle slips for rate group
        telemetry RgCycleSlips: U32 id 1 update on change

        # ----------------------------------------------------------------------
        # Special ports
        # ----------------------------------------------------------------------
        
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

    }
}