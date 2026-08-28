;; Calls tlm() with a channel id one past the top of FwChanIdType's (U32) range.
;; Casting this directly would silently wrap to id 0 and alias a different,
;; valid channel; the host must reject it with HostFunctionInvalidId instead.
;; tlm(id:i64, time_ptr, time_size, value_ptr, value_size) -> i32(valid)
(module
  (import "fprime_v1" "tlm"
    (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 0x100000000  ;; channel id: 2^32, one past U32_MAX
    i32.const 0    ;; time_ptr
    i32.const 11   ;; time_size == SERIALIZED_SIZE
    i32.const 16   ;; value_ptr
    i32.const 4    ;; value_size
    call $tlm
    drop))
