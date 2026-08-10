;; Invokes serial_async with a port index (5) at/above MAX_SERIAL_PORTS (5), i.e. out
;; of range. The host rejects it with a trap before any port invocation.
;; serial_async(index:i32, data_ptr:i32, data_size:i32, return_ptr:i32, return_size:i32) -> i32
(module
  (import "fprime_v1" "serial_async"
    (func $serial_async (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "\11\22\33\44")
  (export "main" (func $main))
  (func $main
    i32.const 5    ;; port index (out of range)
    i32.const 0    ;; data_ptr
    i32.const 4    ;; data_size
    i32.const 16   ;; return_ptr
    i32.const 4    ;; return_size
    call $serial_async
    drop))
