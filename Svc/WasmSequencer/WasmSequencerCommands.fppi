@ Run a Wasm module main function on it's own in the interpreter
@ This command is short-hand for:
@ 1. LOAD_NAME [fileName] ""
@ 2. INVOKE "" "main"
@ 3. CONTINUE
@ 
@ If $block == Svc.BlockState.BLOCK this command will wait for complemention.
async command RUN(
                      fileName: string size FileNameStringSize @< The name of the sequence file
                      $block: Svc.BlockState @< Return command status when complete or not
                    )

@ Wait for the interpreter to finish and return it's result as a CmdResponse
async command WAIT

# async command RUN_ARGS(
#                       fileName: string size FileNameStringSize @< The name of the sequence file
#                       $block: Svc.BlockState @< Return command status when complete or not
#                       buffer: Svc.SeqArgs @< Arguments to pass to the sequencer
#                     ) \
#     opcode 1 priority 7 assert

@ Loads and validates a WebAssembly module into the store.
@ This command loads the module with a empty name meaning only a single module may be loaded.
@ To allow multiple modules, use the `LOAD_NAME` command instead.
async command LOAD(
    fileName: string size FileNameStringSize @< The name of the sequence file
)

@ Load and validate a WebAssembly module into the store. This module is given a name so that
@ it's exports can be referenced by other modules.
async command LOAD_NAME(
    fileName: string size FileNameStringSize @< The name of the sequence file
    name: string size 16 @< WebAssembly module name, must not conflict with previously loaded modules
)

@ Invoke a main function from a loaded module
async command INVOKE(
    $module: string size 16, @< Name of the module to invoke a function from
    $block: Svc.BlockState
)

# @ Loads and validates a sequence with arguments
# # prio: lower than sm sig and CANCEL
# async command VALIDATE_ARGS(
#     fileName: string size FileNameStringSize @< The name of the sequence file
#     buffer: Svc.SeqArgs @< Arguments to pass to the sequencer
# ) \
#     opcode 3 priority 7 assert

@ Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE
@ This completely clears the store.
@ Cancelling during LOADING will trigger a fail response in the reader and
@ return to IDLE mode.
sync command CANCEL

@ Pauses the execution of the sequencer, just before it is about to dispatch the next directive,
@ until unpaused by the CONTINUE command, or stepped by the STEP command. This command is only valid 
@ substates of the RUNNING state that are not RUNNING.PAUSED.
async command PAUSE

@ Dump a stack trace in events
async command TRACE

@ 
async command CONTINUE($block: Svc.BlockState)
