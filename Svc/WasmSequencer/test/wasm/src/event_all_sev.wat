;; Emits one guest event at each F´ severity (1=FATAL .. 7=DIAGNOSTIC).
;; Exercises every branch of the severity switch in dispatchPendingHostFunction (EVENT).
;; Each event pauses the interpreter; the state machine resumes between calls.
(module
  (import "fprime_v1" "event" (func $event (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "sev")
  (export "main" (func $main))
  (func $main
    (local $sev i32)
    i32.const 1
    local.set $sev
    (loop $l
      local.get $sev   ;; severity
      i32.const 0      ;; msg_ptr
      i32.const 3      ;; msg_size
      call $event
      local.get $sev
      i32.const 1
      i32.add
      local.tee $sev
      i32.const 7
      i32.le_u
      br_if $l)))
