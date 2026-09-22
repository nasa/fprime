;; Regression module for the stale-m_exitReason bug.
;; A mutable global tracks how many times `main` has been invoked on this store:
;;   * 1st invoke: exit(0) -> clean success -> component returns to READY WITHOUT a
;;     store reset, leaving the host exit disposition set to HOST_EXIT.
;;   * 2nd invoke: `unreachable` -> a genuine interpreter trap. If the stale
;;     HOST_EXIT/code-0 disposition is not reset per-invoke, spin() misreads this
;;     trap as a clean interpreterFinished (success) instead of a trap.
(module
  (import "fprime_v1" "exit" (func $exit (param i32)))
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (global $calls (mut i32) (i32.const 0))
  (func $main
    global.get $calls
    i32.eqz
    if
      ;; First invocation: record it and cleanly exit(0).
      i32.const 1
      global.set $calls
      i32.const 0
      call $exit
    end
    ;; Second (and later) invocation: genuine trap.
    unreachable))
