
# Sequencing Functionality

## References

- [F Prime Command Sequencing SDD](https://github.com/nasa/fprime/blob/devel/Svc/CmdSequencer/docs/sdd.md)
- [F Prime Sequence Dispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/SeqDispatcher/docs/sdd.md)
- [F Prime Sequence Format](https://github.com/nasa/fprime/blob/devel/Svc/SeqDispatcher/docs/seq_format.md)
- [F Prime Command Dispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/CmdDispatcher/docs/sdd.md)
- [Example F Prime Sequence](https://github.com/nasa/fprime-gds/blob/devel/examples/simple_sequence.seq)]

## Overview

Spacecraft Sequences, are an ordered list of commands that are executed together. Sequences store a series of commands in a specific order and with specific timing to be dispatched later in that order and with that timing. Any software command can be executed in a sequence.

In Fprime this capability is handled by the Sequence Dispatcher and Command Sequencer components. The Sequence Dispatcher component is responsible for coordinating the execution of multiple sequences while the Command Sequence component is instanced for each sequence to be executed. 

### Sequence Management

Prior to execution, sequences are simply files contained within the file system. They are managed via file system functionality and stored within the defined partitions of the file system. 

Maximum Sequence Size is defined at compile time and is [configurable](https://github.com/nasa/fprime/blob/14b48734d0bb010526b2d31ca427c100c6615af6/Ref/Top/RefTopology.cpp#L55).

### Sequence Loading and Validation

Sequence validation is composed of the following checks:

1. Confirm that the file data CRC matches the CRC in sequence file.
2. Confirm that all opcodes in the sequence are valid based on the current fsw build
3. Deserialized Argument Validation (confirming that the arguments are the correct size)

If any of these validation checks fail, the sequence will not load and an error will be reported.

### Sequence Execution

Fprime can be configured with multiple Sequence Dispatcher components and multiple Command Sequencer components. Each Command Sequencer component is assigned to a specific Sequence Dispatcher component creating a pool of sequence "engines" to execute.

The Sequence Dispatcher component routes the sequence file to an available Command Sequencer component for execution. If no Command Sequencer components are available, the sequence will be rejected.

Each Command Sequencer component executes one sequence at a time. 

Sequences can be executed in two ways:

1. Manual Mode - The sequence component will execute one command at a time based on immediate commands from the ground. 
2. Auto Mode (Default) - The sequence component will execute the entire sequence without waiting for user input.

These modes are selected via command  prior to the execution of the sequence. 

If the sequence is in progress, the mode switch will be rejected.

### Sequence Timing

Fprime sequences can support either a relative time or an absolute time. This is defined per command in the sequence.

Absolute times are defined in UTC and are converted to spacecraft time based on the spacecraft clock. If an absolute timed command is in the past, it will be executed immediately.

Relative times are defined in seconds and are executed relative to different conditions based on several factors:

1. Manual mode - The relative time command is executed relative to the receipt of the step command
2. Auto mode - Blocking Enabled - The relative time command is executed relative to the completion of the last command in the sequence (or the start of the sequence if this is the first command)
3. Auto mode - Blocking Disabled - The relative time command is executed relative to the dispatch of the step command

### Off Nominal

If a command in a sequence fails, the sequence will immediately abort.

