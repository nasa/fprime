;; Requests value_size 64, larger than the 4-byte telemetry value the host returns.
(module
  (import "fprime_v1" "tlm"
    (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 128 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison value_ptr+4 (offset 20) with a known non-zero byte so a leak is visible.
    i32.const 20
    i32.const 0xEE
    i32.store8
    i64.const 42   ;; channel id
    i32.const 0    ;; time_ptr
    i32.const 11   ;; time_size == SERIALIZED_SIZE
    i32.const 16   ;; value_ptr
    i32.const 64   ;; value_size >> actual (4)
    call $tlm
    ;; Require Fw::TlmValid::VALID (0).
    i32.const 0
    i32.ne
    if
      unreachable
    end
    ;; The 4 real value bytes round-tripped.
    i32.const 16
    i32.load
    i32.const 0x44332211
    i32.ne
    if
      unreachable
    end
    ;; The poison byte past the value survived: nothing was written there.
    i32.const 20
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
