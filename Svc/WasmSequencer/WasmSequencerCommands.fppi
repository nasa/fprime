@ Run a Wasm module main function on it's own in the interpreter
@ This command first resets the store (discarding any modules previously staged
@ with LOAD), then is short-hand for:
@ 1. LOAD [fileName] ""
@ 2. INVOKE "" "main"
@ 3. CONTINUE
@
@ If $block == Svc.BlockState.BLOCK this command will wait for completion.
async command RUN(
    fileName: string size FileNameStringSize  @< The name of the sequence file
    @ Block until sequence has finished running
    @ Binary arguments to pass to the sequence
    $block: Svc.BlockState
    @ Optional arguments to execute the sequence with
    @ Depending on the sequence being loaded these arguments may differ
    seqArgs: Svc.SeqArgs
) \
    opcode 0

@ Loads and validates a WebAssembly module into the store under the given name.
@ Naming the module lets its exports be referenced by other modules and lets
@ INVOKE, GLOBAL_GET and GLOBAL_SET address it. Use an empty name for a single,
@ standalone module. The name must not conflict with a previously loaded module.
async command LOAD(
    fileName: string size FileNameStringSize        @< The name of the sequence file
    name: string size Wasm.MODULE_NAME_STRING_SIZE  @< WebAssembly module name (empty for a single unnamed module)
) \
    opcode 1

@ Invoke a main function from a loaded module
async command INVOKE(
    $module: string size Wasm.MODULE_NAME_STRING_SIZE  @< Name of the module to invoke a function from
    $block: Svc.BlockState                             @< Block until sequence has finished running
    seqArgs: Svc.SeqArgs                               @< Arguments to invalid sequence entrypoint with
) \
    opcode 2

@ Wait for the interpreter to finish and return it's result as a CmdResponse
async command WAIT opcode 3

@ Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE
@ This completely clears the store.
sync command CANCEL opcode 4

@ Pauses the execution of the sequencer, just before it is about to start spinning.
@ This simply pends a pause flag that will be taken before the sequence engine starts
@ up again.
@
@ This command completes immediately
async command PAUSE opcode 5

@ Resume the sequence from a paused state
async command CONTINUE opcode 6

@ Set a global variable to a given i32 value.
@ Command fails if the module is not found, global is not exported, mutable, or of i32 type
async command GLOBAL_SET_I32(
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE  @< Name of the module to set global for
    name: string size Wasm.GLOBAL_NAME_STRING_SIZE        @< Name of the global
    value: I32                                            @< Value to set global to
) \
    opcode 7

@ Set a global variable to a given i64 value.
@ Command fails if the module is not found, global is not exported, mutable, or of i64 type
async command GLOBAL_SET_I64(
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE  @< Name of the module to set global for
    name: string size Wasm.GLOBAL_NAME_STRING_SIZE        @< Name of the global
    value: I64                                            @< Value to set global to
) \
    opcode 8

@ Set a global variable to a given f32 value.
@ Command fails if the module is not found, global is not exported, mutable, or of f32 type
async command GLOBAL_SET_F32(
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE  @< Name of the module to set global for
    name: string size Wasm.GLOBAL_NAME_STRING_SIZE        @< Name of the global
    value: F32                                            @< Value to set global to
) \
    opcode 9

@ Set a global variable to a given f64 value.
@ Command fails if the module is not found, global is not exported, mutable, or of f64 type
async command GLOBAL_SET_F64(
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE  @< Name of the module to set global for
    name: string size Wasm.GLOBAL_NAME_STRING_SIZE        @< Name of the global
    value: F64                                            @< Value to set global to
) \
    opcode 10

@ Get the current value of a global variable and emit an event
@ Command fails if the module is not found, global is not exported, mutable, or of f64 type
async command GLOBAL_GET(
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE  @< Name of the module to set global for
    name: string size Wasm.GLOBAL_NAME_STRING_SIZE        @< Name of the global
) \
    opcode 11
