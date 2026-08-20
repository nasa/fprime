;; Invokes serial_recv with a port index (5) at/above MAX_SERIAL_PORTS (5), i.e. out of
;; range. The host rejects it with a trap in the host function before parking to wait.
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 5    ;; port index (out of range)
    i32.const 16   ;; data_ptr
    i32.const 8    ;; data_size
    i32.const 0    ;; actual_size_ptr
    i32.const 0    ;; block_type = BLOCKING
    call $serial_recv
    drop))
