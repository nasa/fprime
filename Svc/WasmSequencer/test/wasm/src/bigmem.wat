;; Declares more linear memory than the guest pool can back.
;; GUEST_MEMORY_SIZE is 2048 bytes; 4096 1-byte pages exceeds it -> guest allocator
;; returns null -> spacewasm_load_module fails -> ModuleLoadFailed, return to IDLE.
(module
  (memory 4096 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main))
