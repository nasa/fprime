;; Module whose `start` function traps at instantiation (unreachable).
;; Exercises STARTING -> invokeStartOfLastModule -> SPACEWASM_RUN_TRAP ->
;; startError -> reportInvokeFailure + EXECUTION_ERROR responses -> IDLE.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $start
    unreachable)
  (func $main)
  (start $start))
