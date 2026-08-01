;; Reads param 7 into an out-of-bounds pointer. spacewasm_mem_write of the value
;; fails -> HostFunctionInvalidPointer(PARAMETER) + stmtResponse_failure.
(module
  (import "fprime_v1" "prm" (func $prm (param i64 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 7
    i32.const 10000  ;; OOB value_ptr
    i32.const 4
    call $prm
    drop))
