;; Invokes blocking serial_recv on port index 2 with an out-of-bounds actual_size_ptr.
;; The test injects a valid 4-byte message; when the host dequeues it, the first
;; spacewasm_mem_write (of the message size to actual_size_ptr) fails ->
;; HostFunctionInvalidPointer(SERIAL_RECV, ERR_MEM_OUT_OF_BOUNDS) -> hostResponseFailure
;; -> IDLE. Guest memory is 64 bytes; ptr 10000 is OOB.
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2      ;; port index
    i32.const 16     ;; data_ptr (valid)
    i32.const 8      ;; data_size
    i32.const 10000  ;; OOB actual_size_ptr
    i32.const 0      ;; block_type = BLOCKING
    call $serial_recv
    drop))
