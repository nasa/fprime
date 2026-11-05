;; Calls the `cmd` host function with a fixed 8-byte pattern in guest memory.
;; The test does NOT require valid F´ command encoding; it asserts that the exact
;; guest bytes round-trip verbatim into the cmdOut ComBuffer (after the packet
;; descriptor prefix). `cmd` pauses -> COMMAND dispatch -> AWAITING_RESPONSE, and the
;; test feeds a cmdResponseIn to resume (spacewasm_resume_value).
(module
  (import "fprime_v1" "cmd" (func $cmd (param i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  ;; 8-byte known pattern DE AD BE EF 01 02 03 04 at offset 0
  (data (i32.const 0) "\de\ad\be\ef\01\02\03\04")
  (export "main" (func $main))
  (func $main
    i32.const 0   ;; buf_ptr
    i32.const 8   ;; buf_size
    call $cmd
    drop))
