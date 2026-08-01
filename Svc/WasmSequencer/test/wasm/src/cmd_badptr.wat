;; Calls `cmd` with an out-of-bounds buffer pointer. spacewasm_mem_read of the
;; command bytes fails -> HostFunctionInvalidPointer(COMMAND) + stmtResponse_failure
;; -> report_seqFailed -> IDLE. Guest memory is 64 bytes; ptr 10000 is OOB.
(module
  (import "fprime_v1" "cmd" (func $cmd (param i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 10000  ;; OOB buf_ptr
    i32.const 8       ;; buf_size (valid size, bad pointer)
    call $cmd
    drop))
