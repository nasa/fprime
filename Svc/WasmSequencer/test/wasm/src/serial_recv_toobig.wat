;; Invokes blocking serial_recv on port index 2 with a small guest buffer (data_size = 4).
;; The test then injects an 8-byte message on serialIn[2]. Because the message is larger
;; than the guest buffer, dequeueSerialAndResume logs BufferTooSmall and drops the
;; oversized message from the queue (so it can't poison later serial_recv calls) without
;; ever resuming the interpreter -- it stays paused in AWAITING_RESPONSE until the
;; host-function timeout eventually fails the sequence. The guest never resumes cleanly,
;; so the code after the call is unreachable in practice.
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2    ;; port index
    i32.const 16   ;; data_ptr
    i32.const 4    ;; data_size (guest buffer smaller than the 8-byte message)
    i32.const 0    ;; actual_size_ptr
    i32.const 0    ;; block_type = BLOCKING
    call $serial_recv
    drop))
