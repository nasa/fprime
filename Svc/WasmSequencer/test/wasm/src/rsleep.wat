;; Sleeps for 1 second (1_000_000 us) relative, then returns.
;; Exercises RSLEEP host fn -> pending timer -> checkTimers/checkShouldWake wake path.
(module
  (import "fprime_v1" "rsleep" (func $rsleep (param i64)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 1000000
    call $rsleep))
