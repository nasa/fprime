;; Minimal module: exports `main` that returns immediately.
;; Exercises the nominal no-start RUN path: LOADING_TO_RUN ->
;; MAIN_CHECK_PENDING_START_AND_MAIN -> invokeMain -> MAIN_INVOKE_CHECK ->
;; RUNNING_MAIN -> SequenceSucceeded. Also used as a LOAD+INVOKE fixture.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main))
