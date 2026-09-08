;; Reads telemetry channel 42 into guest memory and VALIDATES the round trip.
;; tlm(id:i64, time_ptr, time_size, value_ptr, value_size) -> i32(valid)
(module
  (import "fprime_v1" "tlm"
    (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 42   ;; channel id
    i32.const 0    ;; time_ptr
    i32.const 11   ;; time_size == SERIALIZED_SIZE
    i32.const 16   ;; value_ptr
    i32.const 4    ;; value_size
    call $tlm
    ;; stack: valid(i32). Require Fw::TlmValid::VALID (0).
    i32.const 0
    i32.ne
    if
      unreachable
    end
    ;; Read the value bytes back and require the exact canned pattern.
    i32.const 16
    i32.load
    i32.const 0x44332211
    i32.ne
    if
      unreachable
    end))
