;; Requests the current time with an out-of-bounds time_ptr. time_size ==
;; SERIALIZED_SIZE (11) so wasmTime's size guards pass and it pauses; but
;; dispatchTime's writeGuestMemory fails -> HostFunctionInvalidPointer(TIME) ->
;; interpreter_sendSignal_hostResponseFailure -> ExitReason::HOST_FAILURE ->
;; SequenceHostFailure.
(module
  (import "fprime_v1" "time" (func $time (param i32 i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 10000  ;; OOB time_ptr (memory is only 64 bytes)
    i32.const 11     ;; time_size == SERIALIZED_SIZE
    call $time))
