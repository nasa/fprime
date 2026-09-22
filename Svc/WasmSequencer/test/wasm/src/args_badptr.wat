;; Reads args into an out-of-bounds pointer. The buffer is large enough (so
;; dispatchArgs' BufferTooSmall check passes), but writeGuestMemory's
;; spacewasm_mem_write fails -> HostFunctionInvalidPointer(ARGS) ->
;; interpreter_sendSignal_hostResponseFailure -> ExitReason::HOST_FAILURE ->
;; SequenceHostFailure.
(module
  (import "fprime_v1" "args"
    (func $args (param i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 10000  ;; OOB value_ptr
    i32.const 64     ;; ample declared size (passes the too-small guard)
    call $args
    drop))
