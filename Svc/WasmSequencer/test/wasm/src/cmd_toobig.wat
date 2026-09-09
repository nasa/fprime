;; Calls `cmd` with len that (with the packet descriptor) exceeds FW_COM_BUFFER_MAX_SIZE (512)
;; -> BufferTooLarge + TRAP. The buffer contents are irrelevant (rejected before read).
(module
  (import "fprime_v1" "cmd" (func $cmd (param i32 i32) (result i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0
    i32.const 600   ;; > 512
    call $cmd
    drop))
