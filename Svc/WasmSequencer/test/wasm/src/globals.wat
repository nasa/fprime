;; Module exporting one mutable global of each WebAssembly 1.0 value type, plus a
;; const (immutable) global. Backs the GLOBAL_SET_* / GLOBAL_GET set/get, type
;; mismatch, and mutability unit tests. The global commands operate directly on
;; the loaded store, so no main execution is required; the empty main keeps the
;; module runnable for good measure.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (global (export "g_i32") (mut i32) (i32.const 100))
  (global (export "g_i64") (mut i64) (i64.const 1000))
  (global (export "g_f32") (mut f32) (f32.const 1.5))
  (global (export "g_f64") (mut f64) (f64.const 2.5))
  (global (export "c_i32") i32 (i32.const 7))
  (export "main" (func $main))
  (func $main))
