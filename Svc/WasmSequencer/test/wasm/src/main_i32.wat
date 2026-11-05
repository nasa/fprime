;; Module whose `main` has the [] -> i32 signature (the other main form the
;; sequencer accepts alongside [] -> []). Returns a non-zero value to prove the
;; return value does not affect success: a normal interpreter finish is a
;; success regardless of the returned i32 (only fprime.exit(code!=0) / panic /
;; trap fail a sequence). Exercises validateModuleMain's "", "i" branch and the
;; SPACEWASM_RUN_FINISHED result-fetch path in spin.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main (result i32)
    i32.const 42))
