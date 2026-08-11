# Asserts in F

The F′ framework uses asserts to perform run-time checks for software
errors. They are a method for verifying conditions that should be true
unless there is a software or processor error. Assert.hpp defines macros
to declare an assertion in C++ code. CAssert.hpp defines macros to
declare an assertion in C code.

The definition for the framework assert is found in Fw/Types/Assert.hpp.
The user calls the `FW_ASSERT(cond, arg1, ...)` macro with up to six arguments. The
arguments can consist of any basic types shown below.

  - I8 8-bit signed integer

  - U8 8-bit unsigned integer

  - I16 16-bit signed integer

  - U16 16-bit unsigned integer

  - I32 32-bit signed integer

  - U32 32-bit unsigned integer

  - F32 32-bit IEEE floating point number (float)

  - F64 64-bit floating point number (double)

  - I64 64-bit signed integer

  - U64 64-bit unsigned integer

  - bool C++ Boolean type

These types are used in FPP specifications. Note that not all types are
available on all processor architectures. The types that are available
is a configurable feature of the architecture and is typically set by
compiler arguments.

## C Assertion Macros

For C code, the framework provides `FW_CASSERT` macros in Fw/Types/CAssert.h:

- `FW_CASSERT(cond)` - Basic assertion with condition only
- `FW_CASSERT_1(cond, arg1)` - Assertion with condition and one argument for reporting
- `FW_CASSERT_2(cond, arg1, arg2)` - Assertion with condition and two arguments for reporting
- `FW_CASSERT_3(cond, arg1, arg2, arg3)` - Assertion with condition and three arguments for reporting
- `FW_CASSERT_4(cond, arg1, arg2, arg3, arg4)` - Assertion with condition and four arguments for reporting
- `FW_CASSERT_5(cond, arg1, arg2, arg3, arg4, arg5)` - Assertion with condition and five arguments for reporting
- `FW_CASSERT_6(cond, arg1, arg2, arg3, arg4, arg5, arg6)` - Assertion with condition and six arguments for reporting

The C assertion macros support the same argument types as the C++ version and integrate with the same assertion hook system.

### Example C Assertion Usage

```c
#include <Fw/Types/CAssert.h>

void example_function(int value) {
    // Basic assertion
    FW_CASSERT(value > 0);

    // Assertion with argument reporting
    FW_CASSERT_1(value <= 1000, value);
}
```

The assert can be configured in the following ways:

  - FW\_ASSERT\_LEVEL Sets the level or reporting for the asserts.

      - FW\_NO\_ASSERT Turns the asserts off. No assertion failure is
        reported; however, the condition expression (the first argument
        to `FW_ASSERT`) is still evaluated and its result discarded.
        Some developers prefer this once the code has been tested to
        regain some processing performance.

        > [!WARNING]
        > `FW_NO_ASSERT` discards the results of these checks, many of
        > which guard against serious errors (out-of-bounds accesses,
        > invalid states). Failures that would have been trapped go
        > undetected, so use this setting with caution.

      - FW\_FILEID\_ASSERT Identifies an integer value for the file
        where the assert occurs (as opposed to \_\_FILE\_\_). It saves
        code space since no file name is stored.

      - FW\_RELATIVE\_PATH\_ASSERT Identifies which file the assert
        occurred in using a project-relative path (requires the build to
        define ASSERT\_RELATIVE\_PATH). It saves code space compared to
        the full \_\_FILE\_\_ path.

      - FW\_FILENAME\_ASSERT Identifies which file the assert occurred
        in. (The \_\_FILE\_\_ macro is used.)

  - FW\_ASSERT\_TEXT\_SIZE Identifies the size of the buffer used to
    store the text of the assert.

## AssertHook

By default, when FW\_ASSERT is called the framework prints a message
(location of and arguments to the macro), and then calls the C assert()
function. The framework also provides a function that allows the
registration of a user-defined handler. The handler is
registerHook and can be found in Fw/Types/Assert.hpp. The assert hook is
called with a string representing the text of the assert. The user
implements a derived class that implements the reportAssert() pure
virtual method, and does whatever project-specific logic is required.

### reportAssert

When formatting an assert message use the default message line hash or
name, and argument approach.

### printAssert

When outputting the assert message use the default Fw::Logger. Force the
Fw::Logger default to printf for systems with print f.

### doAssert

Actually asserts (action of assert).
