;; Calls time with time_size > Fw::Time::SERIALIZED_SIZE (11). The handler rejects
;; it up front with BufferTooLarge(TIME, 16, 11) -> TRAP (HOST trap reason), so
;; nothing is written into guest memory and the sequence never pauses.
(module
  (import "fprime_v1" "time" (func $time (param i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0    ;; time_ptr (valid)
    i32.const 16   ;; time_size > SERIALIZED_SIZE (11)
    call $time))
