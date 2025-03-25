@ Loads, validates and runs a sequence
# prio: before sm sig and CANCEL
async command RUN(
                      fileName: string size FileNameStringSize @< The name of the sequence file
                      $block: BlockState @< Return command status when complete or not
                    ) \
    opcode 0 priority 7 assert

@ Loads and validates a sequence
# prio: before sm sig and CANCEL
async command VALIDATE(
                      fileName: string size FileNameStringSize @< The name of the sequence file
                  ) \
    opcode 1 priority 7 assert

@ Must be called after VALIDATE. Runs the sequence that was validated.
# prio: before sm sig and CANCEL
async command RUN_VALIDATED(
                            $block: BlockState @< Return command status when complete or not
                           ) \
    opcode 2 priority 7 assert

@ Cancels a running or validated sequence. After running CANCEL, the sequencer
@ should return to IDLE
# less prio than sm sig, but higher than everything else
async command CANCEL() \
    opcode 3 priority 8 assert