;; Module with a `start` function (runs at instantiation) and a `main`.
;; Exercises the has-start path: START_CHECK/START_CHECK_PENDING_MAIN ->
;; invokeStart -> START_INVOKE_CHECK(_PENDING_CHAIN) -> RUNNING_START(_PENDING_MAIN)
;; -> engine finishes successfully -> READY (LOAD) or chains into invokeMain ->
;; MAIN_INVOKE_CHECK -> RUNNING_MAIN (RUN).
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $start)
  (func $main)
  (start $start))
