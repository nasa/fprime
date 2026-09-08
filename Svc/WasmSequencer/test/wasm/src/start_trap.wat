;; Module whose `start` function traps at instantiation (unreachable).
;; invokeStart succeeds (call setup is fine); the engine begins running the start
;; function and traps while spinning. Exercises START_CHECK(_PENDING_MAIN) ->
;; invokeStart -> START_INVOKE_CHECK(_PENDING_CHAIN) -> RUNNING_START(_PENDING_MAIN)
;; -> engineFinished -> INTERPRETER_STATUS (interpreterSucceeded == false) ->
;; reportModuleStartFailed -> respond_ERROR -> IDLE.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $start
    unreachable)
  (func $main)
  (start $start))
