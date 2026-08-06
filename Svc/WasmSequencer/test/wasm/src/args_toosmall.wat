;; Requests value_size 2, smaller than the 4-byte args the host holds. Writing the
;; args would overrun the guest's declared buffer, so the host rejects it at dispatch:
;; BufferTooSmall(ARGS, 2, 4) -> stmtResponse_failure -> SequenceFailed, without
;; writing anything to guest memory.
(module
  (import "fprime_v1" "args"
    (func $args (param i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0    ;; value_ptr
    i32.const 2    ;; value_size < actual (4)
    call $args
    drop))
