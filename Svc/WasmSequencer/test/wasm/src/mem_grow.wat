;; memory.grow success path: the module's linear memory is the sole (and therefore last)
;; allocation in the guest bump pool, so guestRealloc grows it in place. main asserts the
;; grow reports the previous size (1 page) and that the newly-grown region is usable.
(module
  (memory 1 (pagesize 1))            ;; 1 byte of guest linear memory, 1-byte pages
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; grow by 63 pages (63 bytes) -> 64 bytes total. memory.grow returns the previous
    ;; size in pages (1) on success, or -1 on failure.
    (if (i32.ne (memory.grow (i32.const 63)) (i32.const 1))
      (then unreachable))            ;; grow must succeed and report old size 1

    ;; the grown region must be usable: store a byte near the new top and read it back.
    (i32.store8 (i32.const 60) (i32.const 0xAB))
    (if (i32.ne (i32.load8_u (i32.const 60)) (i32.const 0xAB))
      (then unreachable))            ;; grown memory must be readable/writable
  )
)
