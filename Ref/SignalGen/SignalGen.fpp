module Ref {

  queued component SignalGen {

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------
   
    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------
   
    include "Telemetry.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------
   
    include "Events.fppi"

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------
    
    @ Time get port
    time get port timeCaller

    @ Command registration port
    command reg port cmdRegOut

    @ Command received port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Text event port
    text event port logTextOut

    @ Event port
    event port logOut

    @ Telemetry port
    telemetry port tlmOut

    # ----------------------------------------------------------------------
    # General Port
    # ----------------------------------------------------------------------
    
    sync input port schedIn: Svc.Sched

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
