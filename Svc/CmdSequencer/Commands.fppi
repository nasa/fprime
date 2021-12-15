

@ Run a command sequence file
async command CS_RUN(
                      fileName: string size 240 @< The name of the sequence file
                      $block: BlockState @< Return command status when complete or not
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

@ Wait for sequences that are running to finish. Allow user to run multiple seq files in SEQ_NO_BLOCK mode then wait for them to finish before allowing more seq run request.
async command CS_JOIN_WAIT \
  opcode 7
