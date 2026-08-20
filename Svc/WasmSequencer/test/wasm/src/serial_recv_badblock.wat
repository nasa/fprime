;; Invokes serial_recv with an invalid block_type value (256). 256 is out of range for the
;; Os::QueueBlockingType enum (BLOCKING=0, NONBLOCKING=1). The host must reject it with a
;; trap (InvalidBlockingTypeValue) rather than truncating it to its low byte (0/BLOCKING).
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2      ;; port index (valid)
    i32.const 16     ;; data_ptr
    i32.const 8      ;; data_size
    i32.const 0      ;; actual_size_ptr
    i32.const 256    ;; block_type = invalid (low byte 0 masquerades as BLOCKING)
    call $serial_recv
    drop))
