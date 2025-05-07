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
| CANCEL | Cancels a running or validated sequence. After running CANCEL, the sequence should return to IDLE |