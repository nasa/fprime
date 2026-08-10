# Svc::FileManager Component

## Overview

`Svc::FileManager` provides a set of ground commands for common file and filesystem operations.
It is a wrapper around the OSAL file, filesystem and directory APIs. The component accepts
commands, calls the corresponding OSAL operation, and reports the result through events,
telemetry, and command responses.

## Functionality

`Svc::FileManager` supports common filesystem operations. They are currently:

- CreateDirectory
- RemoveDirectory
- ListDirectory
- MoveFile
- RemoveFile
- AppendFile
- FileSize
- CalculateCrc

For each command, the component returns success or failure and emits status
information for operators.


