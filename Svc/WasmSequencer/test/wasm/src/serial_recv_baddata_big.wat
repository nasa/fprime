;; Invokes blocking serial_recv on port index 2 with a valid actual_size_ptr but an
;; out-of-bounds data_ptr, and a message large enough (40 bytes) to require the multi-chunk
;; copy path. The size write to actual_size_ptr succeeds, but the FIRST full 32-byte chunk
;; copy to the OOB data_ptr fails -> HostFunctionInvalidPointer(SERIAL_RECV,
;; ERR_MEM_OUT_OF_BOUNDS) -> hostResponseFailure -> IDLE. This exercises the full-chunk
;; write-failure branch (distinct from the final partial-chunk branch).
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2      ;; port index
    i32.const 10000  ;; OOB data_ptr
    i32.const 64     ;; data_size (>= the 40-byte message, so it is not rejected as too big)
    i32.const 0      ;; actual_size_ptr (valid)
    i32.const 0      ;; block_type = BLOCKING
    call $serial_recv
    drop))
