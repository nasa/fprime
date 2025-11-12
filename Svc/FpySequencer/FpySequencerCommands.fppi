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
@ dispatching the specified directive. This command is valid in all states. Breakpoint
@ settings are cleared after a sequence ends execution.
async command SET_BREAKPOINT(
    stmtIdx: U32 @< The directive index to pause execution before.
    breakOnce: bool @< Whether or not to break only once at this breakpoint
) \
    opcode 4 priority 7 assert

@ Pauses the execution of the sequencer, just before it is about to dispatch the next directive,
@ until unpaused by the CONTINUE command, or stepped by the STEP command. This command is only valid 
@ substates of the RUNNING state that are not RUNNING.PAUSED.
async command BREAK() \
    opcode 5 priority 7 assert

@ Continues the automatic execution of the sequence after it has been paused. If a breakpoint is still
@ set, it may pause again on that breakpoint. This command is only valid in the RUNNING.PAUSED state.
async command CONTINUE() \
    opcode 6 priority 7 assert

@ Clears the breakpoint, but does not continue executing the sequence. This command
@ is valid in all states. This happens automatically when a sequence ends execution.
async command CLEAR_BREAKPOINT() \
    opcode 7 priority 7 assert

@ Dispatches and awaits the result of the next directive, or ends the sequence if no more directives remain. Returns
@ to the RUNNING.PAUSED state if the directive executes successfully. This command is only valid in the RUNNING.PAUSED state.
async command STEP() \
    opcode 8 priority 7 assert

@ Sets the value of a flag. See Fpy.FlagId docstrings for info on each flag. 
@ This command is only valid in the RUNNING state.
async command SET_FLAG(
    flag: Fpy.FlagId, value: bool
) \
    opcode 9 priority 7 assert

@ Writes the contents of the stack to a file. This command is only valid in the RUNNING.PAUSED state.
async command DUMP_STACK_TO_FILE(
    fileName: string size FileNameStringSize @< The name of the output file
) \
    opcode 10 priority 7 assert