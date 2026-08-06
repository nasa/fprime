;; Reads the sequence arguments into guest memory and VALIDATES the round trip.
;; args(value_ptr, value_size) -> i32(bytes_written)
;; The host copies exactly SeqArgs.get_size() bytes and returns that count, so the
;; guest requesting a larger buffer (64) than the actual args (4) is fine.
;;
;; The test injects the 4-byte pattern CA FE BA BE. The guest pre-poisons the byte
;; past the args with 0xEE, then asserts (via `unreachable`):
;;   * the returned byte count == 4,
;;   * value[0..4) == the injected pattern (LE 0xBEBAFECA), and
;;   * value[4] is STILL 0xEE, i.e. the host did not copy stack bytes past the args.
;; A clean SequenceSucceeded proves the args survived the host->guest round trip and
;; no host stack leaked into the sandbox.
(module
  (import "fprime_v1" "args"
    (func $args (param i32 i32) (result i32)))
  (memory 128 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison value_ptr+4 (offset 4) with a known non-zero byte so a leak is visible.
    i32.const 4
    i32.const 0xEE
    i32.store8
    i32.const 0    ;; value_ptr
    i32.const 64   ;; value_size >> actual (4)
    call $args
    ;; stack: bytes_written. Require exactly 4.
    i32.const 4
    i32.ne
    if
      unreachable
    end
    ;; The 4 injected arg bytes round-tripped.
    i32.const 0
    i32.load
    i32.const 0xBEBAFECA
    i32.ne
    if
      unreachable
    end
    ;; The poison byte past the args survived: nothing was written there.
    i32.const 4
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
