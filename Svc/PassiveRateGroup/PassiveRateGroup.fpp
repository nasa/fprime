module Svc {

  @ A rate group passive component with input and output scheduler ports
  passive component PassiveRateGroup {

    @ The rate group cycle input
    sync input port CycleIn: [1] Cycle

    @ Scheduler output port to rate group members
    output port RateGroupMemberOut: [$PassiveRateGroupOutputPorts] Sched

  }

}
