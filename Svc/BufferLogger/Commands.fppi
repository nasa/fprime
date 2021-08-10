@ Open a new log file with specified name; also resets unique file counter to 0
async command BL_OpenFile(
                           file: string size 40
                         ) \
  opcode 0x00

@ Close the currently open log file, if any
async command BL_CloseFile \
  opcode 0x01

enum LogState {
  LOGGING_ON = 0
  LOGGING_OFF = 1
}

@ Sets the volatile logging state
async command BL_SetLogging(
                             state: LogState
                           ) \
  opcode 0x02

@ Flushes the current open log file to disk; a no-op with fprime's unbuffered file I/O, so always returns success
async command BL_FlushFile \
  opcode 0x03
