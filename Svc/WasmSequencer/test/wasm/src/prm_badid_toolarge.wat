;; Calls prm() with a parameter id one past the top of FwPrmIdType's (U32) range.
;; Casting this directly would silently wrap to id 0 and alias a different,
;; valid parameter; the host must reject it with HostFunctionInvalidId instead.
;; prm(id:i64, value_ptr, value_size) -> i32(valid)
(module
  (import "fprime_v1" "prm"
    (func $prm (param i64 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 0x100000000  ;; param id: 2^32, one past U32_MAX
    i32.const 0    ;; value_ptr
    i32.const 4    ;; value_size
    call $prm
    drop))
