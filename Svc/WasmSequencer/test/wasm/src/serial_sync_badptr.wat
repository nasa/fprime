;; Calls serial_sync with an out-of-bounds data pointer. spacewasm_mem_read of the
;; payload fails -> HostFunctionInvalidPointer(SYNC_PORT) + stmtResponse_failure
;; -> report_seqFailed -> IDLE. Guest memory is 64 bytes; ptr 10000 is OOB.
(module
  (import "fprime_v1" "serial_sync" (func $serial_sync (param i32 i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0      ;; port index
    i32.const 10000  ;; OOB data_ptr
    i32.const 8      ;; data_size (valid size, bad pointer)
    call $serial_sync))
