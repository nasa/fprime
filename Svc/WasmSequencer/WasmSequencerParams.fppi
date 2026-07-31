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

@ the base directory relative to which sequence file paths are resolved.
@ a '/' separator is always inserted between this base dir and the input
@ sequence file path before resolution occurs following the rules of
@ Os::File::open.
param SEQ_BASE_DIR: string size FileNameStringSize default Fpy.DEFAULT_SEQ_BASE_DIR
