;; Sleeps until an absolute time (10 s from epoch), then returns.
;; Exercises ASLEEP host fn -> absolute pending timer.
(module
  (import "fprime_v1" "asleep" (func $asleep (param i64)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 10000000
    call $asleep))
