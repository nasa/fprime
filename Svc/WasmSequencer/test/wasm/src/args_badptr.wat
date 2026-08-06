;; Reads args into an out-of-bounds pointer. The buffer is large enough (so the
;; BufferTooSmall guard passes), but spacewasm_mem_write of the args fails ->
;; HostFunctionInvalidPointer(ARGS) + stmtResponse_failure -> SequenceFailed.
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
