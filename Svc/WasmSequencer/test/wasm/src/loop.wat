;; A long counted busy-loop (no host calls) that eventually returns.
;; With a small INSTRUCTION_FUEL parameter this spans many SPINNING -> OUT_OF_FUEL ->
;; PAUSE_CHECK cycles, letting the test land PAUSE/CANCEL mid-run deterministically.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    (local $i i32)
    i32.const 1000
    local.set $i
    (loop $l
      local.get $i
      i32.const 1
      i32.sub
      local.tee $i
      br_if $l)))
