@ the number of seconds to wait before giving up
@ on a host function/command. if <= 0 or greater than U32 max, never time out.
@ accuracy of this timeout is determined by the rate group driving this
@ component. it will be rounded up
param HOST_FUNCTION_TIMEOUT_SECS: F32 default 0

@ Number of Wasm instructions to execute per interpreter cycle.
@ Larger numbers will effect the responsiveness of the interpreter to
@ `PAUSE` commands. The interpreter will always pause before commands.
@
@ Larger numbers make the interpreter faster because the engine doesn't
@ need to feed itself through a queue to fuel more cycles.
param INSTRUCTION_FUEL: FwSizeType default Wasm.DEFAULT_INSTRUCTION_FUEL

@ A literal prefix prepended to every requested sequence file path before
@ resolution following the rules of Os::File::open. When non-empty it
@ also acts as a containment boundary: a requested name with a ".." component is
@ rejected rather than being allowed to escape the prefix.
param SEQ_BASE_DIR: string size FileNameStringSize \
    default Wasm.DEFAULT_SEQ_BASE_DIR
