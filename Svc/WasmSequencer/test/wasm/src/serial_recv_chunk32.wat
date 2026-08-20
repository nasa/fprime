;; Blocking serial_recv of a message whose payload is exactly CHUNK_SIZE (32 bytes) on port
;; index 2. This is the boundary case for the chunked copy loop: the full-chunk loop must
;; consume the whole 32-byte payload, leaving zero bytes for the partial-chunk branch (a
;; strict '<' loop bound would instead leave exactly 32 and trip the partial-chunk assert).
;; serial_recv(index, data_ptr, data_size, actual_size_ptr, block_type) -> i32(status)
;;
;; The test injects a 32-byte ramp 01 02 ... 20. The guest asserts status == 0,
;; *actual_size_ptr == 32, the first 4 bytes at data_ptr == LE 0x04030201, and the last
;; payload byte data_ptr[31] == 32 (0x20).
(module
  (import "fprime_v1" "serial_recv"
    (func $serial_recv (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 2    ;; port index
    i32.const 16   ;; data_ptr
    i32.const 64   ;; data_size
    i32.const 0    ;; actual_size_ptr
    i32.const 0    ;; block_type = BLOCKING
    call $serial_recv
    i32.const 0
    i32.ne
    if
      unreachable
    end

    ;; *actual_size_ptr == 32
    i32.const 0
    i32.load
    i32.const 32
    i32.ne
    if
      unreachable
    end

    ;; First 4 payload bytes at data_ptr == LE 0x04030201.
    i32.const 16
    i32.load
    i32.const 0x04030201
    i32.ne
    if
      unreachable
    end

    ;; Last payload byte data_ptr[31] (offset 47) == 32 (0x20).
    i32.const 47
    i32.load8_u
    i32.const 32
    i32.ne
    if
      unreachable
    end))
