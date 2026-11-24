;; A `main` holding 40 operands live on the validator's operand stack at once.
;; Loads under the tester's standard decoder limits but is rejected once
;; `decoderOperands` is configured below that.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    i32.const 1
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop
    drop))
