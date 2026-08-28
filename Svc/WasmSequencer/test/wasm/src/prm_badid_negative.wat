;; Calls prm() with a negative parameter id. FwPrmIdType is unsigned (U32), so a
;; negative i64 id cannot be represented; the host must reject it with
;; HostFunctionInvalidId rather than silently truncating/aliasing another parameter.
;; prm(id:i64, value_ptr, value_size) -> i32(valid)
(module
  (import "fprime_v1" "prm"
    (func $prm (param i64 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const -1   ;; param id: negative, does not fit FwPrmIdType (U32)
    i32.const 0    ;; value_ptr
    i32.const 4    ;; value_size
    call $prm
    drop))
