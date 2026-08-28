;; Requests value_size 2, smaller than the 4-byte parameter value the host returns.
;; dispatchParameter rejects it after pausing: BufferTooSmall(PARAMETER, 2, 4) ->
;; interpreter_sendSignal_hostResponseFailure -> ExitReason::HOST_FAILURE -> SequenceHostFailure,
;; without writing anything to guest memory.
(module
  (import "fprime_v1" "prm"
    (func $prm (param i64 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 7    ;; param id
    i32.const 0    ;; value_ptr
    i32.const 2    ;; value_size < actual (4)
    call $prm
    drop))
