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
        code space since no file name is stored. The F´ build defines
        ASSERT\_FILE\_ID for every source it compiles; sources built
        outside of it (where ASSERT\_FILE\_ID is undefined) report file
        ID 0. This applies to both `FW_ASSERT` and `FW_CASSERT`.

      - FW\_RELATIVE\_PATH\_ASSERT Identifies which file the assert
        occurred in using a project-relative path (requires the build to
        define ASSERT\_RELATIVE\_PATH). It saves code space compared to
        the full \_\_FILE\_\_ path.

      - FW\_FILENAME\_ASSERT Identifies which file the assert occurred
        in. (The \_\_FILE\_\_ macro is used.)

  - FW\_ASSERT\_TEXT\_SIZE Identifies the size of the buffer used to
    store the text of the assert.

## Default Assert Handling

When no `AssertHook` is registered (see below), a failed assertion is handled by
three functions declared in `Fw/Types/assert_hook.hpp`:

| Function | Role |
|---|---|
| `Fw::formatAssertMessage` | formats the location and arguments of the assertion into a buffer |
| `Fw::printAssertMessage` | outputs the formatted message |
| `Fw::terminateOnAssert` | terminates (or not) after the message has been output |

These functions are provided by an implementation of the `Fw_AssertHook` CMake
implementation (see [CMake Implementations](../build-system/cmake-implementations.md)).
The platform or project configuration chooses one with `CHOOSES_IMPLEMENTATIONS`, and
the choice is resolved at link time, so a small target pays nothing for code it did not
choose. F´ ships two:

| Implementation | Behavior |
|---|---|
| `Fw_AssertHook_fputs` | formats with `Fw::stringFormat`, prints to `stderr` with `fputs`, calls `assert(false)` |
| `Fw_AssertHook_Stub` | produces an empty message, prints nothing, calls `assert(false)` |

`Fw_AssertHook_fputs` is the choice of the Unix (Linux/Darwin) platform. Every other platform
package must list an `Fw_AssertHook_*` implementation in its `CHOOSES_IMPLEMENTATIONS`
(or every deployment and unit test built for it must choose one itself); when neither
chooses, the build fails with "requires implementation of Fw_AssertHook". Targets
without a console, or that cannot afford `stdio`, choose `Fw_AssertHook_Stub` or
provide their own implementation of the three functions (for example writing to a UART
and resetting the processor) with `register_fprime_implementation`.

`terminateOnAssert` (or a registered hook's `doAssert`) is always called; when
`FW_ASSERTIONS_ALWAYS_ABORT` is `1` the framework additionally calls `abort()` afterwards,
so an implementation whose `terminateOnAssert` returns still terminates the program. With
`NDEBUG` defined, `assert(false)` compiles out and execution continues after the
assertion unless `FW_ASSERTIONS_ALWAYS_ABORT` is set. Both the C++ `FW_ASSERT` macros and
the C `FW_CASSERT` macros follow this sequence.

The link-time choice sets the default behavior of a platform. The runtime
`AssertHook` below sits on top of it and is how a flight system redirects assertions
into its FATAL handling (`Svc::AssertFatalAdapter`) and how unit tests intercept
them. The registry itself is independent of the implementation chosen, but the default
`AssertHook::printAssert` / `AssertHook::doAssert` and the message that
`Svc::AssertFatalAdapter` writes to `Fw::Logger` use the chosen functions: with
`Fw_AssertHook_Stub` that log line is empty (the `AF_ASSERT_n` FATAL events still carry
file, line and arguments).

## AssertHook

By default, when FW\_ASSERT is called the framework formats and prints a message
(location of and arguments to the macro) and then terminates, using the functions
of the chosen `Fw_AssertHook` implementation described above. The framework also provides a function that allows the
registration of a user-defined handler. The handler is
registerHook and can be found in Fw/Types/Assert.hpp. The assert hook is
called with a string representing the text of the assert. The user
implements a derived class that overrides the virtual methods below and
does whatever project-specific logic is required.

### reportAssert

Receives the location of and arguments to the assertion. The default formats
them with `Fw::formatAssertMessage` and passes the text to `printAssert`.

### printAssert

Outputs the formatted message. The default calls `Fw::printAssertMessage`.

### doAssert

Performs the assert action after the message has been reported. The default
calls `Fw::terminateOnAssert`.
