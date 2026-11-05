;; Reads parameter 7 into guest memory.
;; prm(id:i64, value_ptr, value_size) -> i32(valid)
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
    ;; stack: valid(i32). Require Fw::ParamValid::VALID (1).
    i32.const 1
    i32.ne
    if
      unreachable
    end
    ;; Read the value bytes back and require the exact canned pattern.
    i32.const 0
    i32.load
    i32.const 0xDDCCBBAA
    i32.ne
    if
      unreachable
    end))
