module Ref {

  @ Component for receiving and performing a math operation
  queued component MathReceiver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Port for receiving the math operation
    async input port mathOpIn: MathOp

    @ Port for returning the math result
    output port mathResultOut: MathResult

    @ The rate group scheduler input
    sync input port schedIn: Svc.Sched

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
    # Parameters
    # ----------------------------------------------------------------------

    @ The multiplier in the math operation
    param FACTOR: F32 default 1.0 id 0 \
      set opcode 10 \
      save opcode 11

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Factor updated
    event FACTOR_UPDATED(
                          val: F32 @< The factor value
                        ) \
      severity activity high \
      id 0 \
      format "Factor updated to {f}" \
      throttle 3

    @ Math operation performed
    event OPERATION_PERFORMED(
                               val: MathOp @< The operation
                             ) \
      severity activity high \
      id 1 \
      format "{} operation performed"

    @ Event throttle cleared
    event THROTTLE_CLEARED \
      severity activity high \
      id 2 \
      format "Event throttle cleared"

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Clear the event throttle
    async command CLEAR_EVENT_THROTTLE \
      opcode 0

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ The operation
    telemetry OPERATION: MathOp id 0

    @ Multiplication factor
    telemetry FACTOR: F32 id 1

  }

}