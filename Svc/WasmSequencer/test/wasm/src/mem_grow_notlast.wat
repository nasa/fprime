;; When this module's linear memory is NOT the last allocation in the guest bump pool, memory.grow
;; cannot grow it in place and must return -1. main asserts it received -1 (so this module only
;; succeeds in the not-last scenario) and exits cleanly.
(module
  (memory 1 (pagesize 1))            ;; 1 byte of guest linear memory, 1-byte pages
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; grow must FAIL (return -1) because a later-loaded module allocated guest memory after us.
    (if (i32.ne (memory.grow (i32.const 63)) (i32.const -1))
      (then unreachable))
  )
)
