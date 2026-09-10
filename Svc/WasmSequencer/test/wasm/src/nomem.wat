;; A valid module that owns NO linear memory (no memory section).
(module
  (func (export "main") (result i32)
    i32.const 0))
