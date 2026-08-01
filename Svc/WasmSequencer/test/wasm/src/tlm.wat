;; Reads telemetry channel 42 into guest memory.
;; tlm(id:i64, time_ptr, time_size, value_ptr, value_size) -> i32(valid)
;; time_size MUST equal Fw::Time::SERIALIZED_SIZE (11). value_size <= FW_TLM_BUFFER_MAX_SIZE.
;; Exercises TELEMETRY host fn -> getTlmChan_out -> mem_write(time)+mem_write(value) -> resume_value.
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
    drop))
