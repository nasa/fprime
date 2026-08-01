;; Reads parameter 7 into guest memory. prm(id:i64, value_ptr, value_size) -> i32(valid).
;; Exercises PARAMETER host fn -> getParam_out -> mem_write(value) -> resume_value.
(module
  (import "fprime_v1" "prm"
    (func $prm (param i64 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 7    ;; param id
    i32.const 0    ;; value_ptr
    i32.const 4    ;; value_size
    call $prm
    drop))
