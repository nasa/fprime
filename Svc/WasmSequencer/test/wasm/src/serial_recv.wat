;; Invokes the blocking serial receive host function on port index 2 and VALIDATES the
;; message round trip into guest memory.
;; serial_recv(index:i32, data_ptr:i32, data_size:i32, actual_size_ptr:i32, block_type:i32)
;;   -> i32(status)   ;; 0 == OK, 1 == EMPTY (nonblocking only)
;;
;; The host blocks the interpreter until a message is delivered on serialIn[index], then
;; copies the payload to data_ptr and writes the byte count (little endian) to
;; actual_size_ptr. The test injects a 4-byte message AA BB CC DD.
;;
;; The guest pre-poisons the byte just before data_ptr and the byte just past the 4-byte
;; payload with 0xEE, then asserts (via `unreachable`):
;;   * the returned status == 0 (OK),
;;   * data_ptr[0..4) == the injected payload (LE 0xDDCCBBAA) -- i.e. written AT data_ptr,
;;   * *actual_size_ptr == 4 (little endian),
;;   * the poison byte BEFORE data_ptr survived (nothing written at data_ptr-1), and
;;   * the poison byte AFTER the payload survived (nothing written at data_ptr+4).
;; A clean SequenceSucceeded proves the payload landed exactly at data_ptr with no shift.
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison data_ptr-1 (offset 15) so a low shift is visible.
    i32.const 15
    i32.const 0xEE
    i32.store8
    ;; Poison data_ptr+4 (offset 20) so a high overflow / +4 shift is visible.
    i32.const 20
    i32.const 0xEE
    i32.store8

    i32.const 2    ;; port index
    i32.const 16   ;; data_ptr
    i32.const 8    ;; data_size (guest buffer capacity, larger than the 4-byte message)
    i32.const 0    ;; actual_size_ptr
    i32.const 0    ;; block_type = BLOCKING
    call $serial_recv
    ;; stack: status. Require exactly 0 (OK).
    i32.const 0
    i32.ne
    if
      unreachable
    end

    ;; The 4 payload bytes round-tripped verbatim at data_ptr (LE 0xDDCCBBAA).
    i32.const 16
    i32.load
    i32.const 0xDDCCBBAA
    i32.ne
    if
      unreachable
    end

    ;; actual_size_ptr holds the byte count 4 (little endian U32 at offset 0).
    i32.const 0
    i32.load
    i32.const 4
    i32.ne
    if
      unreachable
    end

    ;; The poison byte BEFORE data_ptr survived: nothing was written at data_ptr-1.
    i32.const 15
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end

    ;; The poison byte past the payload survived: nothing was written at data_ptr+4.
    i32.const 20
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
