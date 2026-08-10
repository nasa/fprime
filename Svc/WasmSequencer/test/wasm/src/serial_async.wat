;; Invokes the asynchronous serial port host function on port index 2 and VALIDATES
;; the reply round trip.
;; serial_async(index:i32, data_ptr:i32, data_size:i32, return_ptr:i32, return_size:i32) -> i32(bytes_written)
;; The reply port MUST be connected: the host blocks the interpreter until a reply is
;; delivered on serialReply[index], writing it to return_ptr.
;;
;; The guest sends a 4-byte pattern, pre-poisons return_ptr+4 with 0xEE, then after the
;; blocking call asserts (via `unreachable`) that the returned byte count == 4, the 4-byte
;; reply landed at return_ptr, and the poison byte past it survived.
(module
  (import "fprime_v1" "serial_async"
    (func $serial_async (param i32 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  ;; 4-byte request pattern 11 22 33 44 at offset 0
  (data (i32.const 0) "\11\22\33\44")
  (export "main" (func $main))
  (func $main
    ;; Poison return_ptr+4 (offset 20) with a known non-zero byte so a leak is visible.
    i32.const 20
    i32.const 0xEE
    i32.store8
    i32.const 2    ;; port index
    i32.const 0    ;; data_ptr
    i32.const 4    ;; data_size
    i32.const 16   ;; return_ptr
    i32.const 4    ;; return_size
    call $serial_async
    ;; stack: bytes_written. Require exactly 4.
    i32.const 4
    i32.ne
    if
      unreachable
    end
    ;; The 4-byte reply round-tripped into return_ptr (LE 0x44332211).
    i32.const 16
    i32.load
    i32.const 0x44332211
    i32.ne
    if
      unreachable
    end
    ;; The poison byte past the reply survived: nothing was written there.
    i32.const 20
    i32.load8_u
    i32.const 0xEE
    i32.ne
    if
      unreachable
    end))
