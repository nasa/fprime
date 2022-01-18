module Svc {


  @ A rate group active component with input and output scheduler ports
  active component ActiveRateGroup {

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ The rate group cycle input
    async input port CycleIn: Svc.Cycle drop

    @ Scheduler output port to rate group members
    output port RateGroupMemberOut: [ActiveRateGroupOutputPorts] Sched

    @ Ping input port for health
    async input port PingIn: Ping

    @ Ping output port for health
    output port PingOut: Ping

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Informational event that rate group has started
    event RateGroupStarted \
      severity diagnostic \
      id 0 \
      format "Rate group started."

    @ Warning event that rate group has had a cycle slip
    event RateGroupCycleSlip(
                              cycle: U32 @< The cycle where the cycle occurred
                            ) \
      severity warning high \
      id 1 \
      format "Rate group cycle slipped on cycle {}"

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

    @ Event port for emitting events
    event port Log

    @ Event port for emitting text events
    text event port LogText

    @ A port for getting the time
    time get port Time

    @ A port for emitting telemetry
    telemetry port Tlm

  }

}
