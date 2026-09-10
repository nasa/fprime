;; Calls fprime_v1.panic(7). The host maps panic to a TRAP.
(module
  (import "fprime_v1" "panic" (func $panic (param i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 7
    call $panic))
