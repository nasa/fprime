;; Imports fprime_v1.event with the wrong signature.
;; The real signature is (param i32 i32 i32); here it is (param i64).
;; Import type mismatch at load -> ModuleLoadFailed.
(module
  (import "fprime_v1" "event" (func $event (param i64)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 0
    call $event))
