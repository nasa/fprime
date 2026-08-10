;; Invokes serial_sync with a payload size (300) larger than MAX_SERIAL_PORT_SIZE (256).
;; The host rejects it with a trap (BufferTooLarge) before any port invocation.
;; serial_sync(index:i32, data_ptr:i32, data_size:i32) -> (void)
(module
  (import "fprime_v1" "serial_sync" (func $serial_sync (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0     ;; port index
    i32.const 0     ;; data_ptr
    i32.const 300   ;; data_size (> MAX_SERIAL_PORT_SIZE)
    call $serial_sync))
