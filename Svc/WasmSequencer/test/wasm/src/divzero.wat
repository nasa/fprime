;; Integer divide by zero -> TRAP DIVIDE_BY_ZERO.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 1
    i32.const 0
    i32.div_u
    drop))
