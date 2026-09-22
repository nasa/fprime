;; A minimal module that reserves some guest linear memory. Loaded after another module to push the
;; guest bump allocator past that module's memory (so the earlier module is no longer the last
;; allocation and cannot grow in place). Only ever LOADed, never invoked.
(module
  (memory 32 (pagesize 1))           ;; 32 bytes of guest linear memory, 1-byte pages
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main)                       ;; no-op
)
