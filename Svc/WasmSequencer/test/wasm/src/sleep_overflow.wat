;; Requests a relative sleep whose microsecond value, divided into seconds,
;; overflows the U32 seconds field of Fw::Time. wasmRsleep rejects it up front
;; with SleepDurationTooLarge -> TRAP (HOST trap reason); the sequence fails.
;; 0x7FFFFFFFFFFFFFFF us / 1e6 >> U32_MAX.
(module
  (import "fprime_v1" "rsleep" (func $rsleep (param i64)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 0x7FFFFFFFFFFFFFFF
    call $rsleep))
