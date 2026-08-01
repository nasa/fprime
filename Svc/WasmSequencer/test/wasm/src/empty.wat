;; Minimal module: exports `main` that returns immediately.
;; Exercises the nominal load -> start(none) -> RUN_CHECK -> run -> SequenceSucceeded path.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main))
