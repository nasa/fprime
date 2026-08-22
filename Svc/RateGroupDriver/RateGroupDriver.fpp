module Svc {

  @ A rate group driver component with input and output cycle ports
  passive component RateGroupDriver {

    @ Cycle input to the rate group driver
    sync input port CycleIn: Cycle

    @ Cycle output from the rate group driver
    output port CycleOut: [RateGroupDriverRateGroupPorts] Cycle

  }

}
