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

## WAIT_ABS (2)
Sleeps until an absolute time.
| Arg Name      | Arg Type | Source | Description |
|---------------|----------|--------|-------------|
| useconds     | U32      | stack  | Microseconds |
| seconds      | U32      | stack  | Seconds |
| time_context | FwTimeContextStoreType       | stack  | Time context (user defined value, unused by Fpy) |
| time_base    | U16      | stack  | Time base |

## GOTO (4)
Sets the index of the next directive to execute.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| dir_idx  | U32      | hardcoded | The statement index to execute next |

## IF (5)
Pops a byte off the stack. If the byte is not 0, proceed to the next directive, otherwise goto a hardcoded directive index.
 
| Arg Name             | Arg Type | Source     | Description |
|---------------------|----------|------------|-------------|
| false_goto_dir_index| U32      | hardcoded | Directive index to jump to if false |
| condition          | bool     | stack     | Condition to evaluate |

## NO_OP (6)
Does nothing.

## PUSH_TLM_VAL (7)
Pushes a telemetry value buffer to the stack.
| Arg Name     | Arg Type | Source     | Description |
|--------------|----------|------------|-------------|
| chan_id      | U32      | hardcoded | the tlm channel id to get the time of |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the telemetry value buffer |
## PUSH_PRM (8)
Pushes a parameter buffer to the stack.
| Arg Name     | Arg Type | Source     | Description |
|--------------|----------|------------|-------------|
| prm_id       | U32      | hardcoded | the param id to get the value of |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the parameter buffer |

## CONST_CMD (9)
Runs a command with a constant opcode and a constant byte array of arguments.
| Arg Name   | Arg Type | Source     | Description |
|------------|----------|------------|-------------|
| cmd_opcode | U32      | hardcoded | Command opcode |
| args       | bytes    | hardcoded | Command arguments |

| Stack Result Type | Description |
| ------------------|-------------|
| Fw.CmdResponse | The CmdResponse that the command returned |


## OR (10)
Performs an `or` between two booleans, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | bool     | stack  | Right operand |
| lhs      | bool     | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

## AND (11)
Performs an `and` between two booleans, pushes result to stack.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | bool     | stack  | Right operand |
| lhs      | bool     | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## IEQ (12)
Compares two integers for equality, pushes result to stack. Doesn't differentiate between signed and unsigned.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## INE (13)
Compares two integers for inequality, pushes result to stack. Doesn't differentiate between signed and unsigned.

| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## ULT (14)
Performs an unsigned less than comparison on two unsigned integers, pushes result to stack
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## ULE (15)
Performs an unsigned less than or equal to comparison on two unsigned integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## UGT (16)
Performs an unsigned greater than comparison on two unsigned integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## UGE (17)
Performs an unsigned greater than or equal to comparison on two unsigned integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## SLT (18)
Performs a signed less than comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## SLE (19)
Performs a signed less than or equal to comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## SGT (20)
Performs a signed greater than comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## SGE (21)
Performs a signed greater than or equal to comparison on two signed integers, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FEQ (22)
Compares two floats for equality, pushes result to stack. If neither is NaN and they are otherwise equal, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FNE (23)
Compares two floats for inequality, pushes result to stack. If either is NaN or they are not equal, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FLT (24)
Performs a less than comparison on two floats, pushes result to stack. If neither is NaN and the second < first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FLE (25)
Performs a less than or equal to comparison on two floats, pushes result to stack. If neither is NaN and the second <= first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FGT (26)
Performs a greater than comparison on two floats, pushes result to stack. If neither is NaN and the second > first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FGE (27)
Performs a greater than or equal to comparison on two floats, pushes result to stack. If neither is NaN and the second >= first, pushes 1 to stack, otherwise 0. Infinity is handled consistent with C++.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## NOT (28)
Performs a boolean not operation on a boolean, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | bool     | stack  | Value to negate |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |
## FPTOSI (29)
Converts a float to a signed integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Float to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## FPTOUI (30)
Converts a float to an unsigned integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Float to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |
## SITOFP (31)
Converts a signed integer to a float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Integer to convert |
| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## UITOFP (32)
Converts an unsigned integer to a float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U64      | stack  | Integer to convert |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## IADD (33)
Performs integer addition, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## ISUB (34)
Performs integer subtraction, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## IMUL (35)
Performs integer multiplication, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## UDIV (36)
Performs unsigned integer division, pushes result to stack. A divisor of 0 will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |
## SDIV (37)
Performs signed integer division, pushes result to stack. A divisor of 0 will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## UMOD (38)
Performs unsigned integer modulo, pushes result to stack. A 0 divisor (rhs) will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | U64      | stack  | Right operand |
| lhs      | U64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |
## SMOD (39)
Performs signed integer modulo, pushes result to stack. A 0 divisor (rhs) will result in DOMAIN_ERROR.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | I64      | stack  | Right operand |
| lhs      | I64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## FADD (40)
Performs float addition, pushes result to stack. NaN, and infinity are handled consistently with C++ addition.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FSUB (41)
Performs float subtraction, pushes result to stack. NaN, and infinity are handled consistently with C++ subtraction.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FMUL (42)
Performs float multiplication, pushes result to stack. NaN, and infinity are handled consistently with C++ multiplication.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FDIV (43)
Performs float division, pushes result to stack. Zero divisors, NaN, and infinity are handled consistently with C++ division.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

## FLOAT_FLOOR_DIV (44)
Performs float floor division, pushes result to stack. Zero divisors, NaN, and infinity are handled consistently with C++ division and `std::floor`.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FPOW (45)
Performs float exponentiation, pushes result to stack. NaN and infinity values are handled consistently with C++ `std::pow`.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| exp      | F64      | stack  | Exponent value |
| base     | F64      | stack  | Base value |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FLOG (46)
Performs float logarithm, pushes result to stack. Negatives yield a DOMAIN_ERROR, NaN and infinity values are handled consistently with C++ `std::log`.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Value for logarithm |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FMOD (47)
Performs float modulo, pushes result to stack. A 0 divisor (rhs) will result in a DOMAIN_ERROR. A NaN will produce a NaN result or infinity as either argument yields NaN.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| rhs      | F64      | stack  | Right operand |
| lhs      | F64      | stack  | Left operand |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |
## FPTRUNC (49)
Truncates a 64-bit float to a 32-bit float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| F32 | The result |

## FPEXT (48)
Extends a 32-bit float to a 64-bit float, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | F32      | stack  | Float to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| F64 | The result |

## SIEXT_8_64 (50)
Sign-extends an 8-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I8       | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |

## SIEXT_16_64 (51)
Sign-extends a 16-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I16      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## SIEXT_32_64 (52)
Sign-extends a 32-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I32      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| I64 | The result |
## ZIEXT_8_64 (53)
Zero-extends an 8-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U8       | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |
## ZIEXT_16_64 (54)
Zero-extends a 16-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U16      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |
## ZIEXT_32_64 (55)
Zero-extends a 32-bit integer to a 64-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | U32      | stack  | Value to extend |

| Stack Result Type | Description |
| ------------------|-------------|
| U64 | The result |
## ITRUNC_64_8 (56)
Truncates a 64-bit integer to an 8-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| U8 | The result |
## ITRUNC_64_16 (57)
Truncates a 64-bit integer to a 16-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| I16 | The result |
## ITRUNC_64_32 (58)
Truncates a 64-bit integer to a 32-bit integer, pushes result to stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| value    | I64      | stack  | Value to truncate |

| Stack Result Type | Description |
| ------------------|-------------|
| I32 | The result |

## EXIT (59)
Pops a byte off the stack. If the byte == 0, end sequence as if it had finished nominally, otherwise exit the sequence and raise an event with an error code.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| success    | U8      | stack  | 0 if should exit without error |


## ALLOCATE (60)
Pushes a hard-coded count of 0x00-bytes to the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | U32      | hardcoded  | Bytes to allocate |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | A series of 0 bytes of length `size` |

## STORE_CONST_OFFSET (61)
Pops a hard-coded number of bytes off the stack, and writes them to the local variable array at a hard-coded offset.
| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| lvar_offset | U32      | hardcoded  | Local variable offset |
| size        | U32      | hardcoded  | Number of bytes |
| value       | bytes    | stack      | Value to store |

## LOAD (62)
Reads a hard-coded number of bytes from the local variable array at a specific offset, and pushes them to the stack.
| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| lvar_offset | U32      | hardcoded  | Local variable offset |
| size        | U32      | hardcoded  | Number of bytes |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The bytes from the lvar array |

## PUSH_VAL (63)
Pushes a constant array of bytes to the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| val      | bytes    | hardcoded  | the byte array to push |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The byte array from the arg |

## DISCARD (64)
Discards bytes from the top of the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | U32      | hardcoded  | Bytes to discard |


## MEMCMP (65)
Compares two memory regions on the stack.
| Arg Name | Arg Type | Source     | Description |
|----------|----------|------------|-------------|
| size     | U32      | hardcoded  | Bytes to compare |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The result |

## STACK_CMD (66)
Dispatches a command with arguments from the stack.
| Arg Name  | Arg Type | Source     | Description |
|-----------|----------|------------|-------------|
| args_size | U32      | hardcoded  | Size of command arguments |

| Stack Result Type | Description |
| ------------------|-------------|
| Fw.CmdResponse | The CmdResponse that the command returned |

## PUSH_TLM_VAL_AND_TIME (67)
Gets a telemetry channel and pushes its value, and then its time, onto the stack.
| Arg Name     | Arg Type | Source     | Description |
|--------------|----------|------------|-------------|
| chan_id      | U32      | hardcoded | the tlm channel id to get |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the telemetry value buffer |
| Fw.Time | The time tag of the telemetry value |

## PUSH_TIME (68)
Pushes the current time, from the `timeCaller` port, to the stack.
| Stack Result Type | Description |
| ------------------|-------------|
| Fw.Time | The current time |

## SET_FLAG (69)
Pops a bool off the stack, sets a flag with a specific index to that bool.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| flag_idx | U8 | hardcoded | Index of the flag to set |
| value | bool | stack | Value to set the flag to |

## GET_FLAG (70)
Gets a flag and pushes its value as a U8 to the stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| flag_idx | U8 | hardcoded | Index of the flag to get |

| Stack Result Type | Description |
| ------------------|-------------|
| bool | The value of the flag |

## GET_FIELD (71)
Pops an offset (StackSizeType) off the stack. Takes a hard-coded number of bytes from top of stack, and then inside of that a second array of hard-coded number of bytes. The second array is offset by the value previously popped off the stack, with offset 0 meaning the second array starts furthest down the stack. Leaves only the second array of bytes, deleting the surrounding bytes.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| parent_size | U32 | hardcoded | Size of the struct |
| member_size | U32 | hardcoded | Size of the field |
| offset | U32 | stack | Offset of the field in the struct |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The raw bytes of the field |

## PEEK (72)
Pops a StackSizeType `offset` off the stack, then a StackSizeType `byteCount`. Let `top` be the top of the stack. Takes the region starting at `top - offset - byteCount` and going to `top - offset`, and pushes this region to the top of the stack.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| offset | StackSizeType | stack | Offset from top at which to peek |
| byteCount | StackSizeType | stack | Number of bytes to peek at, starting at offset, and going downwards in the stack |

| Stack Result Type | Description |
| ------------------|-------------|
| bytes | The peeked bytes |

## ASSERT (73)
Pops one byte for a condition and one byte for an error code off the stack. If condition is false, raise the error code as an event.
| Arg Name | Arg Type | Source | Description |
|----------|----------|--------|-------------|
| error_code | U8 | stack | Error code to exit with if assertion fails |
| condition | bool | stack | Condition to assert |

## STORE (74)
Pops an offset (StackSizeType) off the stack. Pops a hardcoded number of bytes from the top of the stack, and moves them to the start of the lvar array plus the offset previously popped off the stack.
| Arg Name    | Arg Type | Source     | Description |
|-------------|----------|------------|-------------|
| size        | U32      | hardcoded  | Number of bytes |
| lvar_offset | U32      | stack      | Local variable offset |
| value       | bytes    | stack      | Value to store |
