;; Emits an event with an out-of-bounds message pointer. spacewasm_mem_read of
;; the message fails -> HostFunctionInvalidPointer(EVENT) + stmtResponse_failure.
(module
  (import "fprime_v1" "event" (func $event (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 5      ;; ACTIVITY_HI
    i32.const 10000  ;; OOB msg_ptr
    i32.const 10     ;; msg_size (valid, bad pointer)
    call $event))
