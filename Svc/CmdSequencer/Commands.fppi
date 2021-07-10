@ Run a command sequence file
async command CS_RUN(
                      fileName: string size 240 @< The name of the sequence file
                    ) \
  opcode 0

@ Validate a command sequence file
async command CS_VALIDATE(
                           fileName: string size 240 @< The name of the sequence file
                         ) \
  opcode 1

@ Cancel a command sequence
async command CS_CANCEL \
  opcode 2

@ Start running a command sequence
async command CS_START \
  opcode 3

@ Perform one step in a command sequence. Valid only if CmdSequencer is in MANUAL run mode.
async command CS_STEP \
  opcode 4

@ Set the run mode to AUTO.
async command CS_AUTO \
  opcode 5

@ Set the run mode to MANUAL.
async command CS_MANUAL \
  opcode 6
