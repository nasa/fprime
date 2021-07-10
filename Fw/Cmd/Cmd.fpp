module Fw {

  type CmdArgBuffer

  @ Command registration port
  port CmdReg(
               opCode: FwOpcodeType @< Command Op Code
             )

  @ Port for sending commands
  port Cmd(
            opCode: FwOpcodeType @< Command Op Code
            cmdSeq: U32 @< Command Sequence
            ref args: CmdArgBuffer @< Buffer containing arguments
          )

  @ Enum representing a command response
  enum CmdResponse {
    OK = 0 @< Command successfully executed
    INVALID_OPCODE = 1 @< Invalid opcode dispatched
    VALIDATION_ERROR = 2 @< Command failed validation
    FORMAT_ERROR = 3 @< Command failed to deserialize
    EXECUTION_ERROR = 4 @< Command had execution error
    BUSY = 5 @< Component busy
  }

  @ Port for sending command responses
  port CmdResponse(
                    opCode: FwOpcodeType @< Command Op Code
                    cmdSeq: U32 @< Command Sequence
                    response: CmdResponse @< The command response argument
                  )

}
