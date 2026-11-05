;; Blocking serial_recv of a 40-byte message on port index 2, exercising the multi-chunk
;; copy path in dequeueSerialAndResume (one full 32-byte chunk + an 8-byte partial chunk).
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
;;
;; The test injects a 40-byte ramp 01 02 03 ... 28 (byte i == i+1). The guest asserts:
;;   * status == 0 (OK),
;;   * *actual_size_ptr == 40,
;;   * the first 4 payload bytes at data_ptr == LE 0x04030201 (proves payload starts AT
;;     data_ptr, not data_ptr+4), and
;;   * the last payload byte data_ptr[39] == 40 (0x28) (proves the full multi-chunk copy).
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2    ;; port index
    i32.const 16   ;; data_ptr
    i32.const 64   ;; data_size (buffer capacity, larger than the 40-byte message)
    i32.const 0    ;; actual_size_ptr
    i32.const 0    ;; block_type = BLOCKING
    call $serial_recv
    ;; status must be 0 (OK)
    i32.const 0
    i32.ne
    if
      unreachable
    end

    ;; *actual_size_ptr == 40
    i32.const 0
    i32.load
    i32.const 40
    i32.ne
    if
      unreachable
    end

    ;; First 4 payload bytes at data_ptr (offset 16) == LE 0x04030201.
    i32.const 16
    i32.load
    i32.const 0x04030201
    i32.ne
    if
      unreachable
    end

    ;; Last payload byte data_ptr[39] (offset 55) == 40 (0x28).
    i32.const 55
    i32.load8_u
    i32.const 40
    i32.ne
    if
      unreachable
    end))
