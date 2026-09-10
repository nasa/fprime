;; Reads sequence arguments when none were supplied (SeqArgs.get_size() == 0).
;; args(value_ptr, value_size) -> i32(bytes_written). With empty args the host writes
;; nothing and returns 0. The guest poisons value_ptr, asserts the return is 0, and
;; asserts the poison byte is untouched (no write happened at all).
(module
  (import "fprime_v1" "args"
    (func $args (param i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    ;; Poison value_ptr (offset 0).
    i32.const 0
    i32.const 0xEE
    i32.store8
    i32.const 0    ;; value_ptr
    i32.const 64   ;; value_size (ample room)
    call $args
    ;; Require 0 bytes written.
    i32.const 0
    i32.ne
    if
      unreachable
    end
    ;; The poison byte survived: the host wrote nothing.
    i32.const 0
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
