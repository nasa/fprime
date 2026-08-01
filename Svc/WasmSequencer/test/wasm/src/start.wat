;; Module with a `start` function (runs at instantiation) and a `main`.
;; Exercises STARTING -> invokeStartOfLastModule (finished) -> RUN_CHECK.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $start)
  (func $main)
  (start $start))
