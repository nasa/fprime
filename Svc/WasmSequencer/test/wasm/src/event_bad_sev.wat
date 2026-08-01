;; Calls event with an invalid severity id (99) -> HostFunctionInvalidSeverity + TRAP HOST.
(module
  (import "fprime_v1" "event" (func $event (param i32 i32 i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "x")
  (export "main" (func $main))
  (func $main
    i32.const 99   ;; invalid severity
    i32.const 0
    i32.const 1
    call $event))
