;; A long counted busy-loop (like loop.wat) that ALSO exports a mutable global.
;; With a small INSTRUCTION_FUEL parameter the loop spans many SPINNING ->
;; (interpreterOutOfFuel) -> PAUSE_CHECK cycles, letting a test hold the engine in
;; SPINNING / PAUSED (children of RUNNING) while it dispatches GLOBAL_GET / GLOBAL_SET.
;; This proves the global commands act on the live store mid-sequence and are not
;; rejected by the controller (unlike RUN/LOAD/INVOKE, which return BUSY).
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (global (export "g_i32") (mut i32) (i32.const 42))
  (export "main" (func $main))
  (func $main
    (local $i i32)
    i32.const 100000
    local.set $i
    (loop $l
      local.get $i
      i32.const 1
      i32.sub
      local.tee $i
      br_if $l)))
