;; Invokes the serial output host function on port index 1 with a fixed 8-byte pattern
;; in guest memory.
;; serial_send(index:i32, data_ptr:i32, data_size:i32) -> (void)
;; serial_out is fire-and-forget: the host copies the payload out, emits it on
;; serialOut[index], and resumes the interpreter immediately (no reply awaited).
;; The test asserts the exact guest bytes round-trip verbatim into the serialOut buffer.
(module
  (import "fprime_v1" "serial_send" (func $serial_send (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  ;; 8-byte known pattern DE AD BE EF 01 02 03 04 at offset 0
  (data (i32.const 0) "\de\ad\be\ef\01\02\03\04")
  (export "main" (func $main))
  (func $main
    i32.const 1   ;; port index
    i32.const 0   ;; data_ptr
    i32.const 8   ;; data_size
    call $serial_send))
