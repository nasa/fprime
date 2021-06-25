module Ref {

  queued component SignalGen {

    
    include "Commands.fppi"

    
    include "Telemetry.fppi"

    
    include "Events.fppi"

    time get port timeCaller

    command reg port cmdRegOut

    command recv port cmdIn

    sync input port schedIn: Svc.Sched

    text event port logTextOut

    event port logOut

    command resp port cmdResponseOut

    telemetry port tlmOut

  }

  struct SignalInfo {
    $type: Ref.SignalType
    history: Ref.SignalSet
    pairHistory: Ref.SignalPairSet
  }


  struct SignalPair {
    $time: F32 format "{f}"
    value: F32 format "{f}"
  }


  array SignalPairSet = [4] Ref.SignalPair


  array SignalSet = [4] F32 default [
                                      0.0
                                      0.0
                                      0.0
                                      0.0
                                    ] format "{f}"


  enum SignalType {
    TRIANGLE = 0
    SQUARE = 1
    SINE = 2
    NOISE = 3
  }

}
