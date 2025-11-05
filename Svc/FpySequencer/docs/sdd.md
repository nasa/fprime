# Svc::FpySequencer

**The FpySequencer is currently in development. Use at own risk**

The FpySequencer loads, validates and runs up to one Fpy sequence at a time.

The FpySequencer is primarily composed of a state machine and a runtime environment. The state machine controls the loading, validation, starting and stopping of the sequence, and the actual execution takes place in a sectioned-off runtime.

The FpySequencer runs files compiled by `fprime-fpyc` (in the `fprime-gds` package). See the compiler documentation for the details of the Fpy language.

> [!CAUTION]
> The FpySequencer depends on `float` and `double` conforming to IEEE-754 standard on the target system. Users should ensure `SKIP_FLOAT_IEEE_754_COMPLIANCE` is defined as `0` to guarantee compliance.

> [!CAUTION]
> The FpySequencer depends on a 2's complement representation of integers.

## Requirements

| **ID**      | **Requirement**                                                                                                                                   | **Verification**                |
| ----------- | ------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------- |
| FPY-SEQ-001 | The sequencer shall support branching on conditionals.                                                                                            | Unit Test                       |
| FPY-SEQ-002 | The sequencer shall support arithmetic and logical operations on 64bit signed, unsigned, and floating point numbers.                              | Unit Test                       |
| FPY-SEQ-003 | The sequencer shall support accessing telemetry.                                                                                                  | Unit Test                       |
| FPY-SEQ-004 | The sequencer shall support accessing the parameter database.                                                                                     | Unit Test                       |
| FPY-SEQ-005 | The sequencer shall support calling subroutines with arguments and a single return value.                                                         | Unit Test                       |
| FPY-SEQ-006 | The sequencer shall support scoped variables.                                                                                                     | Unit Test                       |
| FPY-SEQ-007 | The sequencer shall support executing directives at relative times.                                                                               | Unit Test                       | 
| FPY-SEQ-008 | The sequencer shall support executing directives at absolute times.                                                                               | Unit Test                       | 
| FPY-SEQ-009 | The sequencer shall support sequence-local variables.                                                                                             | Unit Test                       |
| FPY-SEQ-010 | The sequencer shall support dispatching commands with dynamic and constant arguments.                                                             | Unit Test                       |
| FPY-SEQ-011 | The sequencer shall support running sequences with arguments.                                                                                     | Unit Test                       |
| FPY-SEQ-012 | The sequencer shall read a binary-encoded sequence file of the format described in [TBD]()                                                        | Unit Test                       |
| FPY-SEQ-013 | The sequencer shall support sending commands for dispatch.                                                                                        | Unit Test                       |
| FPY-SEQ-014 | The sequencer shall support parameterized sequences.                                                                                              | Unit Test                       |
| FPY-SEQ-015 | The sequencer shall support conversions between F Prime signed, unsigned, and floating-point primitive types and their equivalent 64-bit types.   | Unit Test                       |
| FPY-SEQ-016 | The sequencer shall support exiting the sequence execution with a specified exit code.                                                            | Unit Test                       |
| FPY-SEQ-017 | The sequencer shall support looping constructs.                                                                                                   | Unit Test                       |
| FPY-SEQ-018 | The sequencer shall support NO OP functionality.                                                                                                  | Unit Test                       |
| FPY-SEQ-019 | The sequencer shall support working with complex modeled data structures (Arrays, Serializables).                                                 | Unit Test                       |
| FPY-SEQ-020 | The sequencer shall support setting flags as described in the [Flags](#flags) section via command and sequence directive.                  | Unit Test                       |
| FPY-SEQ-021 | The sequencer shall support the directives described in the [Directives](#directives) section.                                                    | Unit Test                       |

## States

The following diagram represents the states of the `FpySequencer`.


```mermaid
stateDiagram-v2
  direction LR
  state "IDLE
entry / clearBreakpoint, clearSequenceFile
    ------------------------------------------------------
    cmd_SET_BREAKPOINT / setBreakpoint
    cmd_CLEAR_BREAKPOINT / clearBreakpoint 
  " as IDLE

  state "VALIDATING
    enter / report_seqStarted, validate
    ------------------------------------------------------
    cmd_SET_BREAKPOINT / setBreakpoint
    cmd_CLEAR_BREAKPOINT / clearBreakpoint
  " as VALIDATING

  state VALID <<choice>>

  state "AWAITING_CMD_RUN_VALIDATED
    enter / resp_OK
    ------------------------------------------------------
    cmd_SET_BREAKPOINT / setBreakpoint
    cmd_CLEAR_BREAKPOINT / clearBreakpoint
  " as AWAITING_CMD_RUN_VALIDATED

  state "RUNNING entry / resetRuntime
  " as RUNNING {
    state BREAK_CHECK <<choice>>

    state "DISPATCH_STATEMENT
      enter / dispatch 
    " as DISPATCH_STATEMENT

    state "PAUSED
      entry / clearBreakBeforeNextLine, if breakOnce: clearBreakpoint
      -------------------------------------------------------------
    " as PAUSED

    [*] --> BREAK_CHECK
    BREAK_CHECK --> PAUSED: if break
    BREAK_CHECK --> DISPATCH_STATEMENT: if not break

    PAUSED --> DISPATCH_STATEMENT: cmd_CONTINUE
    PAUSED --> DISPATCH_STATEMENT: cmdSTEP/setBreakBeforeNextLine

    DISPATCH_STATEMENT --> [*]: noMoreStatements/resp_OK
    DISPATCH_STATEMENT --> [*]: failure/resp_EXECUTION_ERROR 
    DISPATCH_STATEMENT --> AWAITING_STATEMENT_RESPONSE: success


    AWAITING_STATEMENT_RESPONSE --> [*]: failure/resp_EXECUTION_ERROR 
    AWAITING_STATEMENT_RESPONSE --> [*]: timeout
    AWAITING_STATEMENT_RESPONSE --> SLEEPING: beginSleep
    AWAITING_STATEMENT_RESPONSE --> BREAK_CHECK: success

    SLEEPING --> [*]: timeout/resp_EXECUTION_ERROR
    SLEEPING --> [*]: error/resp_EXECUTION_ERROR

    SLEEPING --> BREAK_CHECK: shouldWake

  }

  IDLE --> VALIDATING: cmd_VALIDATE/setSequenceFilePath
  IDLE --> VALIDATING: cmd_RUN/setSequenceFilePath

  VALID --> RUNNING: if cmd_RUN
  VALID --> AWAITING_CMD_RUN_VALIDATED: if cmd_VALIDATE

  VALIDATING --> IDLE:  failure/seqFailed,resp_EXECUTION_ERROR
  VALIDATING --> VALID: success
  VALIDATING --> IDLE: cmd_CANCEL/seqCancelled,resp_EXECUTION_ERROR

  AWAITING_CMD_RUN_VALIDATED --> IDLE: cmd_CANCEL/seqCancelled
  AWAITING_CMD_RUN_VALIDATED --> RUNNING: cmd_RUN_VALIDATED

  RUNNING --> IDLE: failure
  RUNNING --> IDLE: noMoreStatements

```


## Flags
The FpySequencer supports certain boolean flags which control the behavior of the sequencer while running a sequence. The flags can be accessed and modified by the sequence itself, or by command while a sequence is running. When a sequence starts running, the flags are initialized to a value configured by the FLAG_DEFAULT_XYZ parameters.

| Name | Description | Default value (configurable) |
|---|------------|---|
|EXIT_ON_CMD_FAIL|if true, the sequence will exit with an error if a command fails|false|

## Commands
| Name | Description |
|-----|-----|
| RUN | Loads, validates and runs a sequence |
| VALIDATE | Loads and validates a sequence. Mutually exclusive with RUN |
| RUN_VALIDATED | Must be called after VALIDATE. Runs the sequence that was validated. |
| CANCEL | Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE |
| SET_FLAG | Sets the value of a flag |

## Debugging Commands
The FpySequencer has a set of debugging commands which can be used to pause and step through sequences. They should not be necessary for nominal use cases.

| Name | Description |
|-----|-----|
| SET_BREAKPOINT | Sets a breakpoint at the specified statement index. When reached, execution will pause before dispatching that statement. |
| BREAK | Immediately pauses execution before dispatching the next statement. Will break once, then continue normal execution. |
| CONTINUE | Continues automatic execution of the sequence after it has been paused. If a breakpoint is still set, execution may pause again. |
| CLEAR_BREAKPOINT | Clears any set breakpoint, but does not continue executing the sequence. |
| STEP | When paused, executes the next statement then returns to paused state. Not valid during automatic execution. |
| DUMP_STACK_TO_FILE | Writes the contents of the stack to a file. Not valid during automatic execution. |

## Directives
See `directives.md` for documentation on all directives.
