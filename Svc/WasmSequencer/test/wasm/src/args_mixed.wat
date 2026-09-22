;; Reads a FULL SequenceArgumentsMaxSize (12-byte) argument payload holding a mixed
;; structure and VALIDATES every field round-trips verbatim.
;; args(value_ptr, value_size) -> i32(bytes_written)
;;
;; The host copies exactly SeqArgs.get_size() (== 12) bytes. The guest lays the payload
;; out as a packed, little-endian mixed struct:
;;   offset 0 : U32 a = 0x11223344   (bytes 44 33 22 11)
;;   offset 4 : U16 b = 0x5566       (bytes 66 55)
;;   offset 6 : U8  c = 0x77
;;   offset 7 : U8  d = 0x88
;;   offset 8 : U32 e = 0x99AABBCC   (bytes CC BB AA 99)
;; = 12 bytes total, exactly filling the argument buffer.
;;
;; The guest requests a larger buffer (32), pre-poisons the byte just past the args
;; (offset 12) with 0xEE, then asserts (via `unreachable`):
;;   * the returned byte count == 12,
;;   * each field reads back at its offset with the expected width/value, and
;;   * value[12] is STILL 0xEE (the host copied exactly 12 bytes, no stack leak).
;; A clean SequenceSucceeded proves the mixed-type payload survived the host->guest
;; round trip at the buffer's maximum size.
(module
  (import "fprime_v1" "args"
    (func $args (param i32 i32) (result i32)))
  (memory 128 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison the byte past the 12-byte payload so a leak/overcopy is visible.
    i32.const 12
    i32.const 0xEE
    i32.store8
    i32.const 0    ;; value_ptr
    i32.const 32   ;; value_size >> actual (12)
    call $args
    ;; stack: bytes_written. Require exactly 12.
    i32.const 12
    i32.ne
    if
      unreachable
    end
    ;; U32 a @0 == 0x11223344
    i32.const 0
    i32.load
    i32.const 0x11223344
    i32.ne
    if
      unreachable
    end
    ;; U16 b @4 == 0x5566
    i32.const 4
    i32.load16_u
    i32.const 0x5566
    i32.ne
    if
      unreachable
    end
    ;; U8 c @6 == 0x77
    i32.const 6
    i32.load8_u
    i32.const 0x77
    i32.ne
    if
      unreachable
    end
    ;; U8 d @7 == 0x88
    i32.const 7
    i32.load8_u
    i32.const 0x88
    i32.ne
    if
      unreachable
    end
    ;; U32 e @8 == 0x99AABBCC
    i32.const 8
    i32.load
    i32.const 0x99AABBCC
    i32.ne
    if
      unreachable
    end
    ;; The poison byte past the args survived: exactly 12 bytes were copied.
    i32.const 12
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
