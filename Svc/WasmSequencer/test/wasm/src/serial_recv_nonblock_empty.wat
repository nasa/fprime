;; Invokes serial_recv NONBLOCKING on port index 3 while its queue is empty. The host does
;; not block; it resumes the interpreter immediately with status 1 (EMPTY).
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
;;
;; The guest asserts (via `unreachable`) that the returned status == 1 (EMPTY) and that
;; nothing was written into the data buffer (the poison byte survives). A clean finish
;; proves the empty-nonblocking fast path resumes without touching guest memory.
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison data_ptr (offset 16) so any spurious write is visible.
    i32.const 16
    i32.const 0xEE
    i32.store8

    i32.const 3    ;; port index (empty queue)
    i32.const 16   ;; data_ptr
    i32.const 8    ;; data_size
    i32.const 0    ;; actual_size_ptr
    i32.const 1    ;; block_type = NONBLOCKING
    call $serial_recv
    ;; stack: status. Require exactly 1 (EMPTY).
    i32.const 1
    i32.ne
    if
      unreachable
    end

    ;; The data buffer was not touched: poison byte survives.
    i32.const 16
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
