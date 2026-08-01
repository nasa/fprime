;; Calls prm with value_size > FW_PARAM_BUFFER_MAX_SIZE (506) -> BufferTooLarge + TRAP.
(module
  (import "fprime_v1" "prm"
    (func $prm (param i64 i32 i32) (result i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 7
    i32.const 0
    i32.const 600   ;; > 506
    call $prm
    drop))
