@ the number of seconds to wait before giving up
@ on a directive or command. if <= 0 or greater than U32 max, never time out.
@ accuracy of this timeout is determined by the rate group driving this
@ component. it will be rounded up
param STATEMENT_TIMEOUT_SECS: F32 default 0

@ Number of Wasm instructions to execute per interpreter cycle.
@ Larger numbers will effect the responsiveness of the interpreter to
@ `PAUSE` commands. The interpreter will always pause before commands.
@
@ Larger numbers make the interpreter faster because the engine doesn't
@ need to feed itself through a queue to fuel more cycles.
param INSTRUCTION_FUEL: FwSizeType default 1000

@ The maximum number of Wasm modules allowed to load into the sequencer's store.
@
@ If this sequencer does not have enough memory to allocate this store, the sequencer
@ will enter into an invalid state and the parameter must be lowered to enter back into a
@ valid state.
@
@ When this parameter is set, the store must be reallocated via a `CANCEL` command.
param MAX_MODULES: U8 default 4
