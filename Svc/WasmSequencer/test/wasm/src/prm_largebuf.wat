;; Calls prm with value_size > FW_PARAM_BUFFER_MAX_SIZE (506)
;; Host function writes a parameter of size 4
;; Test checks that 4 bytes were written
(module
  (import "fprime_v1" "prm"
    (func $prm (param i64 i32 i32) (result i32)))
  (memory 128 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison value_ptr+4 (offset 4) with a known non-zero byte so a leak is visible.
    i32.const 4
    i32.const 0xEE
    i32.store8
    i64.const 7    ;; param id
    i32.const 0    ;; value_ptr
    i32.const 64   ;; value_size >> actual (4)
    call $prm
    ;; Require Fw::ParamValid::VALID (1).
    i32.const 1
    i32.ne
    if
      unreachable
    end
    ;; The 4 real value bytes round-tripped.
    i32.const 0
    i32.load
    i32.const 0xDDCCBBAA
    i32.ne
    if
      unreachable
    end
    ;; The poison byte past the value survived: nothing was written there.
    i32.const 4
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
