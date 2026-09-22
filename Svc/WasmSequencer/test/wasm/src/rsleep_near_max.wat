;; Relative sleep whose whole-seconds part == U32_MAX: passes the wasmRsleep gate
;; (micros / 1e6 == U32_MAX, not > U32_MAX) but overflows the U32 seconds field of
;; Fw::Time when added to a non-zero current time. Exercises the overflow-safe
;; deadline clamp in dispatchRelativeSleep (the guest must NOT wake early).
(module
  (import "fprime_v1" "rsleep" (func $rsleep (param i64)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 4294967295000000
    call $rsleep))
