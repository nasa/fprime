# Svc::FpySequencer

**The FpySequencer is currently in development. Use at own risk**

The FpySequencer loads, validates and runs up to one Fpy sequence at a time. The Fpy sequencing language currently supports executing commands and delays. In the future, it will support checks against telemetry and parameters, variables, functions and arguments.

The FpySequencer is primarily composed of a state machine and a runtime environment. The state machine controls the loading, validation, starting and stopping of the sequence, and the actual execution takes place in a sectioned-off runtime.

The FpySequencer accepts files in the Fpy Bytecode format, which is defined in `FpySequencerTypes.fpp`. You can create these files from human-readable bytecode with the `fpy-bytecode` tool in `fprime-gds`, or in the future from high-level Fpy with the `fpy-compile` tool, also from `fprime-gds`. Please refer to the tools' documentation for more info on how to use them.

## Commands
| Name | Description |
|-----|-----|
| RUN | Loads, validates and runs a sequence |
| VALIDATE | Loads and validates a sequence |
| RUN_VALIDATED | Must be called after VALIDATE. Runs the sequence that was validated. |
| CANCEL | Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE |

## Debugging Commands
The FpySequencer has a set of debugging commands which can be used to pause and step through sequences. They should not be necessary for nominal use cases.

| Name | Description |
|-----|-----|
| DEBUG_SET_BREAKPOINT | Sets the debugging breakpoint which will pause the execution of the sequencer when reached, until unpaused by the DEBUG_CONTINUE command.  |
| DEBUG_BREAK | Pauses the execution of the sequencer, just before it is about to dispatch the next statement, until unpaused by the DEBUG_CONTINUE command. |
| DEBUG_CONTINUE | Continues the execution of the sequence after it has been paused by a debug break. |
| DEBUG_CLEAR_BREAKPOINT | Clears the debugging breakpoint, but does not continue executing the sequence. |

## Directives
| Name | Description |
|------|-------------|
| NO_OP | Does nothing |
| WAIT_REL | Sleeps for a time duration relative to the current time |
| WAIT_ABS | Sleeps until an absolute time is reached |
| SET_LVAR | Sets a local variable to a byte array value |
| GOTO | Sets the index of the next statement to be executed |
| IF | Interprets an lvar as a boolean. If false, goto a specified statement index, otherwise proceed |

## Variables
A variable is a byte array accessible to sequences. 
### Local variables
Local variables are only accessible to the sequence currently executing in this sequencer, and are cleared after the sequences stops running.