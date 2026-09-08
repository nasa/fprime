;; Reads the current F´ system time into guest memory.
;; time(time_ptr, time_size) -> ()
;; time_size == SERIALIZED_SIZE (11) so wasmTime's size guards pass; dispatchTime
;; serializes Fw::Time into [time_ptr] and resumes -> hostResume.
;; A clean SequenceSucceeded proves the TIME host function round trip.
(module
  (import "fprime_v1" "time" (func $time (param i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 0    ;; time_ptr (valid)
    i32.const 11   ;; time_size == SERIALIZED_SIZE
    call $time))
