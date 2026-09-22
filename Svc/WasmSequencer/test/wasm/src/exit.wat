;; Calls fprime_v1.exit(1). The host maps exit to a TRAP (guest program terminates).
(module
  (import "fprime_v1" "exit" (func $exit (param i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 1
    call $exit))
