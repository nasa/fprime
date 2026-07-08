module Svc {

  @ A rate group passive component with input and output scheduler ports
  passive component PassiveRateGroup {

    @ The rate group cycle input
    sync input port CycleIn: Cycle

    @ Scheduler output port to rate group members
    output port RateGroupMemberOut: [PassiveRateGroupOutputPorts] Sched

    @ Max execution time of rate group cycle
    telemetry MaxCycleTime: U32 update on change format "{} us"

    @ Execution time of current cycle
    telemetry CycleTime: U32 format "{} us"

    @ Count of number of cycles
    telemetry CycleCount: U32

    event port Log

    text event port LogText

    @ Emitted when per-component timing is enabled and a member sets a new max execution time
    event ComponentMaxTimeUpdated(maxTimeUs: U32) \
      severity activity high \
      format "Rate group member new max execution time: {} us"

    # Standard ports
    @ A port for getting the time
    time get port Time

    @ A port for emitting telemetry
    telemetry port Tlm

  }

}
