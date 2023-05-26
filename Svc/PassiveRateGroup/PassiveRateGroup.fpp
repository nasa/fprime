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

    # Standard ports
    @ A port for getting the time
    time get port Time

    @ A port for emitting telemetry
    telemetry port Tlm

  }

}
