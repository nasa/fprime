;; Invokes serial_async with a payload size (300) larger than MAX_SERIAL_PORT_SIZE (256).
;; The host rejects it with a trap (BufferTooLarge) before any port invocation.
;; serial_async(index:i32, data_ptr:i32, data_size:i32, return_ptr:i32, return_size:i32) -> i32
(module
  (import "fprime_v1" "serial_async"
    (func $serial_async (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2     ;; port index
    i32.const 0     ;; data_ptr
    i32.const 300   ;; data_size (> MAX_SERIAL_PORT_SIZE)
    i32.const 16    ;; return_ptr
    i32.const 4     ;; return_size
    call $serial_async
    drop))
