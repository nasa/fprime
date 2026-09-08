;; Imports a function that does not exist in the fprime_v1 host module.
;; Import resolution fails at load -> ModuleLoadFailed.
(module
  (import "fprime_v1" "does_not_exist" (func $missing (param i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0
    call $missing))
