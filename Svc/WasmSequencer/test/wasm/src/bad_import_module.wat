;; Imports from module "fprime" -- the host module is named "fprime_v1".
;; Host module not found at load -> ModuleLoadFailed. (This is the mistake the stale
;; root-level example.wasm/test.wasm make; captured here as a negative test.)
(module
  (import "fprime" "event" (func $event (param i32 i32 i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "x")
  (export "main" (func $main))
  (func $main
    i32.const 5
    i32.const 0
    i32.const 1
    call $event))
