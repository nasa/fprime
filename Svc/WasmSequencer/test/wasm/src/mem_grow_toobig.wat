;; memory.grow that cannot be satisfied by the guest pool must fail GRACEFULLY: spacewasm's
;; realloc callback (guestRealloc) returns null, and memory.grow yields -1 to the guest without
;; trapping. main asserts it got -1 and then exits cleanly (SequenceSucceeded).
(module
  (memory 1 (pagesize 1))            ;; 1 byte initial, 1-byte pages
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; grow by 4096 bytes -> 4097 total, which exceeds the guest pool (2048 B by default), so the
    ;; guest bump allocator cannot satisfy it. The grow must return -1 rather than trap.
    (if (i32.ne (memory.grow (i32.const 4096)) (i32.const -1))
      (then unreachable))            ;; a grow past the pool must return -1, not succeed
  )
)
