;; Calls tlm() with a negative channel id. FwChanIdType is unsigned (U32), so a
;; negative i64 id cannot be represented; the host must reject it with
;; HostFunctionInvalidId rather than silently truncating/aliasing another channel.
;; tlm(id:i64, time_ptr, time_size, value_ptr, value_size) -> i32(valid)
(module
  (import "fprime_v1" "tlm"
    (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const -1   ;; channel id: negative, does not fit FwChanIdType (U32)
    i32.const 0    ;; time_ptr
    i32.const 11   ;; time_size == SERIALIZED_SIZE
    i32.const 16   ;; value_ptr
    i32.const 4    ;; value_size
    call $tlm
    drop))
