module Ref {

  @ Component sending a math operation
  queued component MathReceiver {

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Port for receiving the math operation
    async input port mathIn: MathOp

    @ Port for returning the math result
    output port mathOut: MathResult

    @ The rate group scheduler input
    sync input port SchedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Command receive
    command recv port cmdIn

    @ Command registration
    command reg port cmdRegOut

    @ Command response
    command resp port cmdResponseOut

    @ Event
    event port eventOut

    @ Parameter get
    param get port prmGetOut

    @ Parameter set
    param set port prmSetOut

    @ Telemetry
    telemetry port tlmOut

    @ Text event
    text event port textEventOut

    @ Time get
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # Commands 
    # ----------------------------------------------------------------------

    @ Set operation multiplication factor1
    async command SET_FACTOR1(
                               val: F32 @< The first factor
                             ) \
      opcode 0

    @ Clear the event throttle
    async command CLEAR_EVENT_THROTTLE \
      opcode 1

    # ----------------------------------------------------------------------
    # Events 
    # ----------------------------------------------------------------------

    @ Operation factor 1
    event SET_FACTOR1(
                       val: F32 @< The factor value
                     ) \
      severity activity high \
      id 0 \
      format "Factor 1: {f}" \
      throttle 3

    @ Updated factor 2
    event UPDATED_FACTOR2(
                           val: F32 @< The factor value
                         ) \
      severity activity high \
      id 1 \
      format "Factor 2 updated to: {f}"

    @ Math operation performed
    event OPERATION_PERFORMED(
                               val: MathOp @< The operation
                             ) \
      severity activity high \
      id 2 \
      format "Operation performed: {}"

    @ Event throttle cleared
    event THROTTLE_CLEARED \
      severity activity high \
      id 3 \
      format "Event throttle cleared"

    # ----------------------------------------------------------------------
    # Parameters 
    # ----------------------------------------------------------------------

    @ A test parameter
    param factor2: F32 default 1.0 id 0 \
      set opcode 10 \
      save opcode 11

    # ----------------------------------------------------------------------
    # Telemetry 
    # ----------------------------------------------------------------------

    @ The operation
    telemetry OPERATION: MathOp id 0

    @ The number of SET_FACTOR1 commands
    telemetry FACTOR1S: U32 id 1

    @ Factor 1 value
    telemetry FACTOR1: F32 id 2

    @ Factor 2 value
    telemetry FACTOR2: F32 id 3

  }

}
