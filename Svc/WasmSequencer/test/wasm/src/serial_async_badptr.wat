;; Calls serial_async with an out-of-bounds data pointer. spacewasm_mem_read of the
;; payload fails -> HostFunctionInvalidPointer(ASYNC_PORT) + stmtResponse_failure
;; -> report_seqFailed -> IDLE, without ever emitting on serialOut. Guest memory is
;; 64 bytes; ptr 10000 is OOB.
(module
  (import "fprime_v1" "serial_async"
    (func $serial_async (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2      ;; port index
    i32.const 10000  ;; OOB data_ptr
    i32.const 4      ;; data_size (valid size, bad pointer)
    i32.const 16     ;; return_ptr
    i32.const 4      ;; return_size
    call $serial_async
    drop))
