;; Valid module that exports a function named `other` but NOT `main`.
;; INVOKE/RUN -> find_export_func("main") fails -> invoke failure (ModuleInvokeFailed).
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "other" (func $other))
  (func $other))
