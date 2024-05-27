module Ref {

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

  array SignalSet = [4] F32 format "{f}"

  enum SignalType {
    TRIANGLE
    SQUARE
    SINE
    NOISE
  }

  @ A component for generating periodic signals
  queued component SignalGen {

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    sync input port schedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Data product get port
    product get port productGetOut

    @ Data product request port
    product request port productRequestOut

    @ Data product receive port
    async product recv port productRecvIn

    @ Data product send port
    product send port productSendOut

    @ Time get port
    time get port timeCaller

    @ Command registration port
    command reg port cmdRegOut

    @ Command received port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Telemetry port
    telemetry port tlmOut

    @ Signal generation data product record
    product record DataRecord: SignalInfo id 0

    @ Data product container
    product container DataContainer id 0 default priority 10


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
    include "../Interfaces/EventsInterface.fppi"


  }

}
