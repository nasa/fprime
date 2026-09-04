;; Invokes blocking serial_recv on port index 2 with a valid actual_size_ptr but an
;; out-of-bounds data_ptr. The test injects a valid 4-byte message; the size write to
;; actual_size_ptr succeeds, but the subsequent payload copy to the OOB data_ptr fails in
;; the final (partial) chunk -> HostFunctionInvalidPointer(SERIAL_RECV, ERR_MEM_OUT_OF_BOUNDS)
;; -> hostResponseFailure -> IDLE. Guest memory is 64 bytes; data_ptr 10000 is OOB.
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
    i32.const 64     ;; data_size (large enough that the 4-byte message is not rejected)
    i32.const 0      ;; actual_size_ptr (valid)
    i32.const 0      ;; block_type = BLOCKING
    call $serial_recv
    drop))
