module Ref {

  @ Component for sending a math operation
  active component MathSender {

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Port for sending the operation request
    output port mathOut: Ref.MathOp

    @ Port for receiving the result
    async input port mathIn: Ref.MathResult

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Time get port
    time get port timeGetOut

    @ Telemetry port
    telemetry port tlmOut

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseIn

    @ Event port
    event port eventOut

    @ Text event port
    text event port textEventOut

    # ----------------------------------------------------------------------
    # Commands 
    # ----------------------------------------------------------------------

    @ Do a math operation
    async command DO_MATH(
                           val1: F32 @< The first value
                           op: MathOp @< The operation to perform
                           val2: F32 @< The second value
                         ) \
      opcode 0

    # ----------------------------------------------------------------------
    # Events 
    # ----------------------------------------------------------------------

    @ Math command received
    event COMMAND_RECV(
                        val1: F32 @< The val1 argument
                        op: MathOp @< The requested operation
                        val2: F32 @< The val2 argument
                      ) \
      severity activity low \
      id 0 \
      format "Math Cmd Recvd: {f} {} {f}"

    @ Received math result
    event RESULT(
                  result: F32 @< The math result
                ) \
      severity activity high \
      id 1 \
      format "Math result is {f}"

    # ----------------------------------------------------------------------
    # Telemetry 
    # ----------------------------------------------------------------------

    @ The first value
    telemetry VAL1: F32 id 0

    @ The second value
    telemetry VAL2: F32 id 1

    @ The operation
    telemetry OP: MathOp id 2

    @ The result
    telemetry RES: F32 id 3

  }

}
