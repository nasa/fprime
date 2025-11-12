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
| size     | U32      | hardcoded  | Bytes to allocate |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | A series of 0 bytes of length `size` |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## STORE_CONST_OFFSET (59)
Pops a hard-coded number of bytes off the stack, and writes them to the local variable array at a hard-coded offset.
| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| lvar_offset | U32      | hardcoded  | Local variable offset |
| size        | U32      | hardcoded  | Number of bytes |
| value       | bytes    | stack      | Value to store |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## LOAD (60)
Reads a hard-coded number of bytes from the local variable array at a specific offset, and pushes them to the stack.
| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| lvar_offset | U32      | hardcoded  | Local variable offset |
| size        | U32      | hardcoded  | Number of bytes |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The bytes from the lvar array |

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
| size     | U32      | hardcoded  | Bytes to discard |

**Requirement:**  FPY-SEQ-009, FPY-SEQ-010

## MEMCMP (63)

Pops 2x `size` bytes off the stack.  Compares the first `size` bytes to the second `size` bytes with a byte-for-byte comparison pushing a boolean true when equal and false when unequal.

| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | U32      | hardcoded  | Bytes to compare |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

**Requirement:** FPY-SEQ-019

## STACK_CMD (64)
Dispatches a command with arguments from the stack.
| Arg Name  | Arg Type | Source     | Description |
|-----------|----------|------------|-------------|
| args_size | U32      | hardcoded  | Size of command arguments |

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
| parent_size | U32 | hardcoded | Size of the struct |
| member_size | U32 | hardcoded | Size of the field |
| offset | U32 | stack | Offset of the field in the struct |

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

## STORE (71)
Pops an offset (StackSizeType) off the stack. Pops a hardcoded number of bytes from the top of the stack, and moves them to the start of the lvar array plus the offset previously popped off the stack.
| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| size        | U32      | hardcoded  | Number of bytes |
| lvar_offset | U32      | stack      | Local variable offset |
| value       | bytes    | stack      | Value to store |

| Stack Result Type | Description |
| ------------------|-------------|
| N/A | |

**Requirement:**  FPY-SEQ-009
