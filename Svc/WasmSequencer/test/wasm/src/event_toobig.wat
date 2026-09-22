;; Emits an ACTIVITY_HI event with a 250-byte message. The host clamps the
;; length to FW_LOG_STRING_MAX_SIZE (200) -- exercises the truncation branch in
;; WasmSequencerHost::wasmEvent. Guest memory is 256 1-byte pages (fits the
;; 2048-byte guest pool); 250 'A' bytes live at offset 0.
(module
  (import "fprime_v1" "event" (func $event (param i32 i32 i32)))
  (memory 256 (pagesize 1))
  (export "memory" (memory 0))
  (data (i32.const 0) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
  (export "main" (func $main))
  (func $main
    i32.const 5    ;; ACTIVITY_HI
    i32.const 0    ;; msg_ptr
    i32.const 250  ;; msg_size > 200 -> truncated to 200
    call $event))
