;; Calls serial_async with a valid data pointer but an out-of-bounds return pointer.
;; The request is emitted on serialOut and the interpreter blocks; when a reply arrives,
;; serialReply_handler's spacewasm_mem_write to the return pointer fails ->
;; HostFunctionInvalidPointer(ASYNC_PORT) -> stmtResponse_failure -> IDLE.
;; Guest memory is 64 bytes; return ptr 10000 is OOB.
(module
  (import "fprime_v1" "serial_async"
    (func $serial_async (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "\11\22\33\44")
  (export "main" (func $main))
  (func $main
    i32.const 2      ;; port index
    i32.const 0      ;; data_ptr (valid)
    i32.const 4      ;; data_size
    i32.const 10000  ;; OOB return_ptr
    i32.const 4      ;; return_size
    call $serial_async
    drop))
