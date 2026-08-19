;; Module whose `main` reads a mutable exported global and adds 10 to it. Backs
;; the "a GLOBAL_SET value survives into execution" and "GLOBAL_GET reflects a
;; guest mutation" round trips: LOAD keeps the store, so a GLOBAL_SET followed by
;; INVOKE lets main observe the set value, and a subsequent GLOBAL_GET observes
;; main's write-back. (RUN resets the store, so the persist test must use INVOKE.)
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (global $g (export "g_i32") (mut i32) (i32.const 0))
  (export "main" (func $main))
  (func $main
    global.get $g
    i32.const 10
    i32.add
    global.set $g))
