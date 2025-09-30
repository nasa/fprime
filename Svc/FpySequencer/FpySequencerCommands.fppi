@ Loads, validates and runs a sequence
# prio: lower than sm sig and CANCEL
async command RUN(
                      fileName: string size FileNameStringSize @< The name of the sequence file
                      $block: BlockState @< Return command status when complete or not
                    ) \
    opcode 0 priority 7 assert

@ Loads and validates a sequence
# prio: lower than sm sig and CANCEL
async command VALIDATE(
    fileName: string size FileNameStringSize @< The name of the sequence file
) \
    opcode 1 priority 7 assert

@ Must be called after VALIDATE. Runs the sequence that was validated.
# prio: lower than sm sig and CANCEL
async command RUN_VALIDATED(
    $block: BlockState @< Return command status when complete or not
) \
    opcode 2 priority 7 assert

@ Cancels a running or validated sequence. After running CANCEL, the sequencer
@ should return to IDLE
# less prio than sm sig, but higher than everything else
async command CANCEL() \
    opcode 3 priority 8 assert

@ Sets the breakpoint which will pause the execution of the sequencer when
@ reached, until unpaused by the CONTINUE command. Will pause just before 
@ dispatching the specified statement. This command is valid in all states. Breakpoint
@ settings are cleared after a sequence ends execution.
async command SET_BREAKPOINT(
    stmtIdx: U32 @< The statement index to pause execution before.
    breakOnce: bool @< Whether or not to break only once at this breakpoint
) \
    opcode 4 priority 7 assert

@ Pauses the execution of the sequencer, just before it is about to dispatch the next statement,
@ until unpaused by the CONTINUE command. This command is only valid in the RUNNING state.
@ Breakpoint settings are cleared after a sequence ends execution.
async command BREAK(
    breakOnce: bool @< Whether or not to break only once at this breakpoint
) \
    opcode 5 priority 7 assert

@ Continues the execution of the sequence after it has been paused. This command
@ is only valid in the RUNNING.PAUSED state.
async command CONTINUE() \
    opcode 6 priority 7 assert

@ Clears the breakpoint, but does not continue executing the sequence. This command
@ is valid in all states. This happens automatically when a sequence ends execution.
async command CLEAR_BREAKPOINT() \
    opcode 7 priority 7 assert

@ Steps to the next 
async command STEP() \
    opcode 8 priority 7 assert