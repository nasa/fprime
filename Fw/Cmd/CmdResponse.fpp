module Fw {

  enum CommandResponse {
    OK = 0 @< Command successfully executed
    INVALID_OPCODE = 1 @< Invalid opcode dispatched
    VALIDATION_ERROR = 2 @< Command failed validation
    FORMAT_ERROR = 3 @< Command failed to deserialize
    EXECUTION_ERROR = 4 @< Command had execution error
    BUSY = 5 @< Component busy
  }

  @ Command port
  port CmdResponse(
                    opCode: FwOpcodeType @< Command Op Code
                    cmdSeq: U32 @< Command Sequence
                    response: CommandResponse @< The command response argument
                  )

}
