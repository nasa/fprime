;; Emits a single ACTIVITY_HI (severity id 5) guest event with a fixed message.
;; Exercises the EVENT host function -> dispatchPendingHostFunction -> log_ACTIVITY_HI_LogActivityHi.
(module
  (import "fprime_v1" "event" (func $event (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "hello wasm")   ;; 10 bytes at offset 0
  (export "main" (func $main))
  (func $main
    i32.const 5    ;; FPRIME_EVENT_ACTIVITY_HI
    i32.const 0    ;; msg_ptr
    i32.const 10   ;; msg_size
    call $event))
