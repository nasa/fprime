
# ComLoggerTee Subtopology

## References

- [F Prime ComLogger](https://github.com/nasa/fprime/blob/devel/Svc/ComLogger/README.md)
- [F Prime ComSplitter](https://github.com/nasa/fprime/blob/devel/Svc/ComSplitter/ComSplitter.fpp)

## Overview

The ComLoggerTee subtopology provides a simple tee (splitter) that duplicates outgoing communication data, sending it both to the normal transmission path and to a communication logger for on-board recording. This is useful for capturing a record of all transmitted data for later analysis or debugging without modifying the primary communication path.

### Included Components

- **Communication Splitter** (passive) — Takes a single input communication buffer and replicates it to multiple outputs.
- **Communication Logger** (active) — Writes communication buffers to files on the file system, with configurable file rotation.

### Data Flow

When inserted into the communication path, the splitter receives outgoing data, sends one copy onward to the normal transmission path, and sends another copy to the communication logger. The logger writes each buffer to a log file. Files are rotated based on a configurable size limit.

### Configuration

- Base IDs, queue sizes, stack sizes, and priorities are configurable via ComLoggerTeeConfig.
- The communication logger's file path prefix and maximum file size are configured at setup time.

### Required Inputs

- Incoming communication data from the framing path.
- Output connection onward to the communication adapter or driver.
