;; Invokes serial_send on port index 3 with a 4-byte payload. Used by tests that rewire
;; serialOut[3] (e.g. leave it disconnected, or point it at a failing input port) to
;; exercise the host's connected/serialization error handling.
;; serial_send(index:i32, data_ptr:i32, data_size:i32) -> (void)
(module
  (import "fprime_v1" "serial_send" (func $serial_send (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "\de\ad\be\ef")
  (export "main" (func $main))
  (func $main
    i32.const 3   ;; port index
    i32.const 0   ;; data_ptr
    i32.const 4   ;; data_size
    call $serial_send))
