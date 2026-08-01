;; Calls tlm with value_size > FW_TLM_BUFFER_MAX_SIZE (506) -> BufferTooLarge + TRAP.
(module
  (import "fprime_v1" "tlm"
    (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 42
    i32.const 0
    i32.const 11
    i32.const 16
    i32.const 600   ;; > 506
    call $tlm
    drop))
