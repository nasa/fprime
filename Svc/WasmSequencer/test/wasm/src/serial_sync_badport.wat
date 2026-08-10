;; Invokes serial_sync with a port index (5) at/above MAX_SERIAL_PORTS (5), i.e. out of
;; range. The host rejects it with a trap before any port invocation.
;; serial_sync(index:i32, data_ptr:i32, data_size:i32) -> (void)
(module
  (import "fprime_v1" "serial_sync" (func $serial_sync (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "\de\ad\be\ef")
  (export "main" (func $main))
  (func $main
    i32.const 5   ;; port index (out of range)
    i32.const 0   ;; data_ptr
    i32.const 4   ;; data_size
    call $serial_sync))
