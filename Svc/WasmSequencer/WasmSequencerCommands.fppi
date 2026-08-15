@ Run a Wasm module main function on it's own in the interpreter
@ This command is short-hand for:
@ 1. LOAD_NAME [fileName] ""
@ 2. INVOKE "" "main"
@ 3. CONTINUE
@
@ If $block == Svc.BlockState.BLOCK this command will wait for completion.
async command RUN(
  fileName: string size FileNameStringSize @< The name of the sequence file
  @ Block until sequence has finished running
  @ Binary arguments to pass to the sequence
  $block: Svc.BlockState
  @ Optional arguments to execute the sequence with
  @ Depending on the sequence being loaded these arguments may differ
  seqArgs: Svc.SeqArgs
) \
  opcode 0

@ Loads and validates a WebAssembly module into the store.
@ This command loads the module with a empty name meaning only a single module may be loaded.
@ To allow multiple modules, use the `LOAD_NAME` command instead.
async command LOAD(
  fileName: string size FileNameStringSize @< The name of the sequence file
) \
  opcode 1

@ Load and validate a WebAssembly module into the store. This module is given a name so that
@ it's exports can be referenced by other modules.
async command LOAD_NAME(
  fileName: string size FileNameStringSize @< The name of the sequence file
  name: string size 16                     @< WebAssembly module name, must not conflict with previously loaded modules
) \
  opcode 2

@ Invoke a main function from a loaded module
async command INVOKE(
  $module: string size 16 @< Name of the module to invoke a function from
  $block: Svc.BlockState  @< Block until sequence has finished running
  seqArgs: Svc.SeqArgs    @< Arguments to invalid sequence entrypoint with
) \
  opcode 3

@ Wait for the interpreter to finish and return it's result as a CmdResponse
async command WAIT opcode 4

@ Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE
@ This completely clears the store.
@ Cancelling during LOADING will trigger a fail response in the reader and
@ return to IDLE mode.
sync command CANCEL opcode 5

@ Pauses the execution of the sequencer, just before it is about to start spinning.
@ This simply pends a pause flag that will be take before the sequence engine starts
@ up again.
@
@ This command completes immediately
async command PAUSE opcode 6

@ Resume the sequence from a paused state
async command CONTINUE opcode 7
