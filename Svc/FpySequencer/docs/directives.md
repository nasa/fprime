# Directives

* Format is DIRECTIVE_NAME (opcode).
* Arguments can either be "hardcoded", meaning they are present in the sequence binary after the opcode, or "stack", meaning they are popped off the stack at runtime.
* Directives can have a "stack result type", which is the type that they push to the stack after execution.

## WAIT_REL (1)
Sleeps for a relative duration from the current time.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| useconds  | U32      | stack  | Wait time in microseconds (must be less than a second) |
| seconds  | U32      | stack  | Wait time in seconds |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:** FPY-SEQ-007

## WAIT_ABS (2)
Sleeps until an absolute time.
| Arg Name      | Arg Type | Source | Description |
|---------------|----------|--------|-------------|
| useconds     | U32      | stack  | Microseconds |
| seconds      | U32      | stack  | Seconds |
| time_context | FwTimeContextStoreType       | stack  | Time context (user defined value, unused by Fpy) |
| time_base    | U16      | stack  | Time base |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:** FPY-SEQ-008

## GOTO (3)
Sets the index of the next directive to execute.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| dir_idx  | U32      | hardcoded | The statement index to execute next |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:**  FPY-SEQ-017

## IF (4)
Pops a byte off the stack. If the byte is not 0, proceed to the next directive, otherwise goto a hardcoded directive index.
 
| Arg Name             | Arg Type | Source     | Description |
|---------------------|----------|------------|-------------|
| false_goto_dir_index| U32      | hardcoded | Directive index to jump to if false |
| condition          | bool     | stack     | Condition to evaluate |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:** FPY-SEQ-001

## NO_OP (5)
Does nothing.

| Arg Name             | Arg Type | Source     | Description |
|---------------------|----------|------------|-------------|
|  N/A | | | |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:**  FPY-SEQ-018

## PUSH_TLM_VAL (6)
Pushes a telemetry value buffer to the stack.
| Arg Name     | Arg Type | Source     | Description |
|--------------|----------|------------|-------------|
| chan_id      | U32      | hardcoded | the tlm channel id to get the time of |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the telemetry value buffer |

**Requirement:**  FPY-SEQ-003

## PUSH_PRM (7)
Pushes a parameter buffer to the stack.
| Arg Name     | Arg Type | Source     | Description |
|--------------|----------|------------|-------------|
| prm_id       | U32      | hardcoded | the param id to get the value of |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the parameter buffer |

**Requirement:**  FPY-SEQ-004

## CONST_CMD (8)
Runs a command with a constant opcode and a constant byte array of arguments.
| Arg Name   | Arg Type | Source     | Description |
|------------|----------|------------|-------------|
| cmd_opcode | U32      | hardcoded | Command opcode |
| args       | bytes    | hardcoded | Command arguments |

| Stack Result Type | Description |
| ------------------|-------------|
| Fw.CmdResponse | The CmdResponse that the command returned |

**Requirement:**  FPY-SEQ-007, FPY-SEQ-008

## OR (9)
Performs an `or` between two booleans, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | bool     | stack  | Right operand |
| lhs      | bool     | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## AND (10)
Performs an `and` between two booleans, pushes result to stack.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | bool     | stack  | Right operand |
| lhs      | bool     | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## IEQ (11)
Compares two integers for equality, pushes result to stack. Doesn't differentiate between signed and unsigned.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## INE (12)
Compares two integers for inequality, pushes result to stack. Doesn't differentiate between signed and unsigned.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## ULT (13)
Performs an unsigned less than comparison on two unsigned integers, pushes result to stack
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## ULE (14)
Performs an unsigned less than or equal to comparison on two unsigned integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## UGT (15)
Performs an unsigned greater than comparison on two unsigned integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## UGE (16)
Performs an unsigned greater than or equal to comparison on two unsigned integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## SLT (17)
Performs a signed less than comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## SLE (18)
Performs a signed less than or equal to comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## SGT (19)
Performs a signed greater than comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## SGE (20)
Performs a signed greater than or equal to comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FEQ (21)
Compares two floats for equality, pushes result to stack. If neither is NaN and they are otherwise equal, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FNE (22)
Compares two floats for inequality, pushes result to stack. If either is NaN or they are not equal, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FLT (23)
Performs a less than comparison on two floats, pushes result to stack. If neither is NaN and the second < first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FLE (24)
Performs a less than or equal to comparison on two floats, pushes result to stack. If neither is NaN and the second <= first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FGT (25)
Performs a greater than comparison on two floats, pushes result to stack. If neither is NaN and the second > first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FGE (26)
Performs a greater than or equal to comparison on two floats, pushes result to stack. If neither is NaN and the second >= first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## NOT (27)
Performs a boolean not operation on a boolean, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | bool     | stack  | Value to negate |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:**  FPY-SEQ-002

## FPTOSI (28)
Converts a float to a signed integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Float to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |

**Requirement:**  FPY-SEQ-015

## FPTOUI (29)
Converts a float to an unsigned integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Float to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-015

## SITOFP (30)
Converts a signed integer to a float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Integer to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-015


## UITOFP (31)
Converts an unsigned integer to a float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U64      | stack  | Integer to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-015

## ADD (32)
Performs integer addition, pushes result to stack. Integers are handled with 2's complement representation.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64     | stack  | Right operand |
| lhs      | U64     | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-002

## SUB (33)
Performs integer subtraction, pushes result to stack. Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-002

## MUL (34)
Performs integer multiplication, pushes result to stack.  Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-002

## UDIV (35)
Performs unsigned integer division, pushes result to stack. A divisor of 0 will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-002

## SDIV (36)
Performs signed integer division, pushes result to stack. A divisor of 0 will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |

**Requirement:**  FPY-SEQ-002

## UMOD (37)
Performs unsigned integer modulo, pushes result to stack. A 0 divisor (rhs) will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-002

## SMOD (38)
Performs signed integer modulo, pushes result to stack. A 0 divisor (rhs) will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |

**Requirement:**  FPY-SEQ-002

## FADD (39)
Performs float addition, pushes result to stack. NaN, and infinity are handled consistently with C++ addition.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FSUB (40)
Performs float subtraction, pushes result to stack. NaN, and infinity are handled consistently with C++ subtraction.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FMUL (41)
Performs float multiplication, pushes result to stack. NaN, and infinity are handled consistently with C++ multiplication.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FDIV (42)
Performs float division, pushes result to stack. Zero divisors, NaN, and infinity are handled consistently with C++ division.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FPOW (43)
Performs float exponentiation, pushes result to stack. NaN and infinity values are handled consistently with C++ `std::pow`.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| exp      | F64      | stack  | Exponent value |
| base     | F64      | stack  | Base value |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FLOG (44)
Performs float logarithm, pushes result to stack. Negatives yield a DOMAIN_ERROR, NaN and infinity values are handled consistently with C++ `std::log`.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Value for logarithm |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FMOD (45)
Performs float modulo, pushes result to stack. A 0 divisor (rhs) will result in a DOMAIN_ERROR. A NaN will produce a NaN result or infinity as either argument yields NaN.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## FPTRUNC (47)
Truncates a 64-bit float to a 32-bit float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| F32 | The result |

**Requirement:**  FPY-SEQ-002

## FPEXT (46)
Extends a 32-bit float to a 64-bit float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F32      | stack  | Float to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

**Requirement:**  FPY-SEQ-002

## SIEXT_8_64 (48)
Sign-extends an 8-bit integer to a 64-bit integer, pushes result to stack.  Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U8       | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:** FPY-SEQ-015

## SIEXT_16_64 (49)
Sign-extends a 16-bit integer to a 64-bit integer, pushes result to stack. Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U16      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:** FPY-SEQ-015

## SIEXT_32_64 (50)
Sign-extends a 32-bit integer to a 64-bit integer, pushes result to stack. Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U32      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:** FPY-SEQ-015

## ZIEXT_8_64 (51)
Zero-extends an 8-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U8       | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-015

## ZIEXT_16_64 (52)
Zero-extends a 16-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U16      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-015

## ZIEXT_32_64 (53)
Zero-extends a 32-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U32      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |

**Requirement:**  FPY-SEQ-015

## ITRUNC_64_8 (54)
Truncates a 64-bit integer to an 8-bit integer, pushes result to stack. Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| U8 | The result |

**Requirement:**  FPY-SEQ-015

## ITRUNC_64_16 (55)
Truncates a 64-bit integer to a 16-bit integer, pushes result to stack. Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| I16 | The result |

**Requirement:** FPY-SEQ-015

## ITRUNC_64_32 (56)
Truncates a 64-bit integer to a 32-bit integer, pushes result to stack. Integers are handled with 2's complement representation.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| I32 | The result |

**Requirement:**  FPY-SEQ-015

## EXIT (57)
Pops a byte off the stack. If the byte == 0, end sequence as if it had finished nominally, otherwise exit the sequence and raise an event with an error code.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| success    | U8      | stack  | 0 if should exit without error |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:**  FPY-SEQ-016

## ALLOCATE (58)
Pushes a hard-coded count of 0x00-bytes to the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | StackSizeType | hardcoded  | Bytes to allocate |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | A series of 0 bytes of length `size` |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## STORE_REL_CONST_OFFSET (59)
Stores a value to a local variable at a compile-time-known offset relative to the current stack frame.

**Preconditions:**
- `len(stack) >= size`
- `stack_frame_start + lvar_offset >= 0`
- `stack_frame_start + lvar_offset + size <= len(stack)`

**Semantics:**
1. Let `value` be the top `size` bytes of the stack (big-endian, with the first byte at `stack[len(stack) - size]`).
2. Remove these `size` bytes from the stack.
3. Write `value` to `stack[stack_frame_start + lvar_offset .. stack_frame_start + lvar_offset + size)`.

**Error Conditions:**
- If `len(stack) < size`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `stack_frame_start + lvar_offset < 0`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `stack_frame_start + lvar_offset + size > len(stack)` (after pop): `STACK_ACCESS_OUT_OF_BOUNDS`

| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| lvar_offset | I32      | hardcoded  | Signed byte offset relative to `stack_frame_start`. Negative values access memory below the frame (e.g., function arguments). |
| size        | StackSizeType | hardcoded  | Number of bytes to store. |
| value       | bytes    | stack      | The value to store (popped from stack top). |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## LOAD_REL (60)
Loads a value from a local variable at a compile-time-known offset relative to the current stack frame, and pushes it to the stack.

**Preconditions:**
- `stack_frame_start + lvar_offset >= 0`
- `stack_frame_start + lvar_offset + size <= len(stack)`
- `len(stack) + size <= max_stack_size`

**Semantics:**
1. Let `addr = stack_frame_start + lvar_offset`.
2. Read `size` bytes from `stack[addr .. addr + size)`.
3. Push these bytes to the top of the stack (preserving byte order).

**Error Conditions:**
- If `stack_frame_start + lvar_offset < 0`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `stack_frame_start + lvar_offset + size > len(stack)`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `len(stack) + size > max_stack_size`: `STACK_OVERFLOW`

| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| lvar_offset | I32      | hardcoded  | Signed byte offset relative to `stack_frame_start`. Negative values access memory below the frame (e.g., function arguments pushed before CALL). |
| size        | StackSizeType | hardcoded  | Number of bytes to load. |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The `size` bytes read from the local variable location. |

**Requirement:** FPY-SEQ-009, FPY-SEQ-010

## PUSH_VAL (61)
Pushes a constant array of bytes to the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| val      | bytes    | hardcoded  | the byte array to push |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The byte array from the arg |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## DISCARD (62)
Discards bytes from the top of the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | StackSizeType | hardcoded  | Bytes to discard |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## MEMCMP (63)

Pops 2x `size` bytes off the stack.  Compares the first `size` bytes to the second `size` bytes with a byte-for-byte comparison pushing a boolean true when equal and false when unequal.

| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | StackSizeType | hardcoded  | Bytes to compare |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:** FPY-SEQ-019

## STACK_CMD (64)
Dispatches a command with arguments from the stack.
| Arg Name  | Arg Type | Source     | Description |
|-----------|----------|------------|-------------|
| args_size | StackSizeType | hardcoded  | Size of command arguments |

| Stack Result Type | Description |
| ------------------|-------------|
| Fw.CmdResponse | The CmdResponse that the command returned |

**Requirement:**  FPY-SEQ-010

## PUSH_TLM_VAL_AND_TIME (65)
Gets a telemetry channel and pushes its value, and then its time, onto the stack.
| Arg Name     | Arg Type | Source     | Description |
|--------------|----------|------------|-------------|
| chan_id      | U32      | hardcoded | the tlm channel id to get |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the telemetry value buffer |
| Fw.Time | The time tag of the telemetry value |

**Requirement:**  FPY-SEQ-010

## PUSH_TIME (66)
Pushes the current time, from the `timeCaller` port, to the stack.
| Stack Result Type | Description |
| ------------------|-------------|
| Fw.Time | The current time |

**Requirement:**  FPY-SEQ-010

## SET_FLAG (67)
Pops a bool off the stack, and sets a command sequencer flag from the value.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| flag_idx | U8 | hardcoded | Index of the flag to set |
| value | bool | stack | Value to set the flag to |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:**  FPY-SEQ-020

## GET_FLAG (68)
Gets a command sequencer flag and pushes its value as a U8 to the stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| flag_idx | U8 | hardcoded | Index of the flag to get |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The value of the flag |

**Requirement:**  FPY-SEQ-020

## GET_FIELD (69)
Pops an offset (StackSizeType) off the stack. Takes a hard-coded number of bytes from top of stack, and then inside of that a second array of hard-coded number of bytes. The second array is offset by the value previously popped off the stack, with offset 0 meaning the second array starts furthest down the stack. Leaves only the second array of bytes, deleting the surrounding bytes.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| parent_size | StackSizeType | hardcoded | Size of the struct |
| member_size | StackSizeType | hardcoded | Size of the field |
| offset | StackSizeType | stack | Offset of the field in the struct |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the field |

**Requirement:**  FPY-SEQ-019

## PEEK (70)
Pops a StackSizeType `offset` off the stack, then a StackSizeType `byteCount`. Let `top` be the top of the stack. Takes the region starting at `top - offset - byteCount` and going to `top - offset`, and pushes this region to the top of the stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| offset | StackSizeType | stack | Offset from top at which to peek |
| byteCount | StackSizeType | stack | Number of bytes to peek at, starting at offset, and going downwards in the stack |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The peeked bytes |

**Requirement:**  FPY-SEQ-009

## STORE_REL (71)
Stores a value to a local variable at a runtime-determined offset relative to the current stack frame.

**Preconditions:**
- `len(stack) >= size + sizeof(I32)` (value bytes + offset)
- After popping offset: `stack_frame_start + lvar_offset >= 0`
- After popping offset: `stack_frame_start + lvar_offset + size <= len(stack)`

**Semantics:**
1. Pop an I32 `lvar_offset` from the stack (big-endian).
2. Let `value` be the top `size` bytes of the remaining stack.
3. Remove these `size` bytes from the stack.
4. Let `addr = stack_frame_start + lvar_offset`.
5. Write `value` to `stack[addr .. addr + size)`.

**Error Conditions:**
- If `len(stack) < size + sizeof(I32)`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `stack_frame_start + lvar_offset < 0`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `stack_frame_start + lvar_offset + size > len(stack)` (after popping offset, before popping value): `STACK_ACCESS_OUT_OF_BOUNDS`

| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| size        | StackSizeType | hardcoded  | Number of bytes to store. |
| lvar_offset | I32      | stack      | Signed byte offset relative to `stack_frame_start`. |
| value       | bytes    | stack      | The value to store (below the offset on stack). |

**Requirement:**  FPY-SEQ-009

## CALL (72)
Performs a function call. Pops the target directive index from the stack, saves the return address and current frame pointer to the stack, then transfers control to the target.

**Preconditions:**
- `len(stack) >= sizeof(U32)` (for target address)
- `len(stack) + sizeof(U32) + sizeof(StackSizeType) <= max_stack_size` (space for return address and frame pointer)
- `0 <= target <= statement_count` (validated before jump)

**Semantics (in order):**
1. Pop a U32 `target` from the stack (big-endian).
2. Let `return_addr = next_dir_idx` (the index of the instruction that would execute after this CALL).
3. Set `next_dir_idx = target`.
4. Push `return_addr` as a U32 (big-endian) to the stack.
5. Push `stack_frame_start` as a StackSizeType (big-endian) to the stack.
6. Set `stack_frame_start = len(stack)` (the new frame begins immediately after the saved frame pointer).

**Stack Layout After CALL:**
```
[... function arguments ...][return_addr (sizeof(U32) bytes)][saved_frame_ptr (sizeof(StackSizeType) bytes)]
                                                                                                            ^
                                                                                      stack_frame_start ────┘
```

**Error Conditions:**
- If `len(stack) < sizeof(U32)`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `len(stack) + sizeof(U32) + sizeof(StackSizeType) > max_stack_size`: `STACK_OVERFLOW`
- If `target > statement_count`: `STMT_OUT_OF_BOUNDS`

**Note:** Function arguments must be pushed to the stack before the target address. The callee accesses arguments using negative `lvar_offset` values in LOAD_REL (e.g., `lvar_offset = -(STACK_FRAME_HEADER_SIZE + arg_size)` where `STACK_FRAME_HEADER_SIZE = sizeof(U32) + sizeof(StackSizeType)`).

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| target   | U32 | stack | Directive index to jump to. |

**Requirement:**  FPY-SEQ-009

## RETURN (73)
Returns from a function call. Restores the caller's execution context and optionally returns a value.

**Preconditions:**
- `len(stack) >= return_val_size` (for return value, if any)
- `len(stack) >= stack_frame_start` (sanity check)
- After truncating to frame: `len(stack) >= sizeof(StackSizeType) + sizeof(U32)` (saved frame pointer + return address)
- After restoring frame: `len(stack) >= call_args_size` (to discard arguments)

**Semantics (in order):**
1. If `return_val_size > 0`: Copy the top `return_val_size` bytes from the stack as `return_value`.
2. Truncate the stack to `stack_frame_start` (discard all local variables allocated in this frame).
3. Pop a StackSizeType as `saved_frame_ptr` (big-endian).
4. Pop a U32 as `return_addr` (big-endian).
5. Set `stack_frame_start = saved_frame_ptr`.
6. Set `next_dir_idx = return_addr`.
7. Discard `call_args_size` bytes from the stack (the function arguments pushed by the caller).
8. If `return_val_size > 0`: Push `return_value` to the stack.

**Stack Transformation:**
```
Before RETURN:
[... caller locals ...][args (call_args_size)][ret_addr (sizeof(U32))][saved_fp (sizeof(StackSizeType))][... callee locals ...][return_value]
                                                                                                        ^                       ^
                                                                                         stack_frame_start                      stack top

After RETURN:
[... caller locals ...][return_value]
^                                    ^
stack_frame_start (restored)         stack top
```

**Error Conditions:**
- If `len(stack) < return_val_size`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `stack_frame_start > len(stack)`: `FRAME_START_OUT_OF_BOUNDS` (corrupt frame)
- If remaining stack after truncation `< sizeof(StackSizeType) + sizeof(U32)`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If remaining stack after header pop `< call_args_size`: `STACK_ACCESS_OUT_OF_BOUNDS`

| Arg Name        | Arg Type | Source     | Description |
|-----------------|----------|------------|-------------|
| return_val_size | StackSizeType | hardcoded  | Size of return value in bytes. Use 0 for void functions. |
| call_args_size  | StackSizeType | hardcoded  | Total size of function arguments in bytes. This must match the bytes pushed by the caller before CALL. |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The return value (only if `return_val_size > 0`). |

**Requirement:**  FPY-SEQ-009

## LOAD_ABS (74)
Loads a value from an absolute address in the stack (used for global variables), and pushes it to the stack.

**Preconditions:**
- `global_offset + size <= len(stack)`
- `len(stack) + size <= max_stack_size`

**Semantics:**
1. Read `size` bytes from `stack[global_offset .. global_offset + size)`.
2. Push these bytes to the top of the stack (preserving byte order).

**Error Conditions:**
- If `global_offset + size > len(stack)`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `len(stack) + size > max_stack_size`: `STACK_OVERFLOW`

| Arg Name      | Arg Type | Source     | Description |
|---------------|----------|------------|-------------|
| global_offset | StackSizeType | hardcoded  | Absolute byte offset from the start of the stack (index 0). |
| size          | StackSizeType | hardcoded  | Number of bytes to load. |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The `size` bytes read from the global variable location. |

**Requirement:**  FPY-SEQ-009

## STORE_ABS (75)
Stores a value to an absolute address in the stack (used for global variables), with the offset determined at runtime.

**Preconditions:**
- `len(stack) >= size + sizeof(StackSizeType)` (value bytes + offset)
- After popping offset: `global_offset + size <= len(stack)`

**Semantics:**
1. Pop a StackSizeType `global_offset` from the stack (big-endian).
2. Let `value` be the top `size` bytes of the remaining stack.
3. Remove these `size` bytes from the stack.
4. Write `value` to `stack[global_offset .. global_offset + size)`.

**Error Conditions:**
- If `len(stack) < size + sizeof(StackSizeType)`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `global_offset + size > len(stack)` (after popping offset, before popping value): `STACK_ACCESS_OUT_OF_BOUNDS`

| Arg Name      | Arg Type | Source     | Description |
|---------------|----------|------------|-------------|
| size          | StackSizeType | hardcoded  | Number of bytes to store. |
| global_offset | StackSizeType | stack      | Absolute byte offset from the start of the stack. |
| value         | bytes    | stack      | The value to store (below the offset on stack). |

**Requirement:**  FPY-SEQ-009

## STORE_ABS_CONST_OFFSET (76)
Stores a value to an absolute address in the stack (used for global variables), with a compile-time-known offset.

**Preconditions:**
- `len(stack) >= size`
- `global_offset + size <= len(stack)`

**Semantics:**
1. Let `value` be the top `size` bytes of the stack.
2. Remove these `size` bytes from the stack.
3. Write `value` to `stack[global_offset .. global_offset + size)`.

**Error Conditions:**
- If `len(stack) < size`: `STACK_ACCESS_OUT_OF_BOUNDS`
- If `global_offset + size > len(stack)` (after pop): `STACK_ACCESS_OUT_OF_BOUNDS`

| Arg Name      | Arg Type | Source     | Description |
|---------------|----------|------------|-------------|
| global_offset | StackSizeType | hardcoded  | Absolute byte offset from the start of the stack. |
| size          | StackSizeType | hardcoded  | Number of bytes to store. |
| value         | bytes    | stack      | The value to store (popped from stack top). |

**Requirement:**  FPY-SEQ-009
