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

<!-- fpp-dictionary-begin -->
## Component Dictionary

The following tables are derived from the component's FPP model.

### Port Descriptions

| Name | Kind | Port Type | Description |
|---|---|---|---|
| `pingIn` | `async input` | `Svc.Ping` | Ping input port |
| `schedIn` | `sync input` | `Sched` | Scheduler input port for rate group operations |
| `pingOut` | `output` | `Svc.Ping` | Ping output port |

### Events

| Name | Severity | Description |
|---|---|---|
| `DirectoryCreateError` | `warning high` | An error occurred while attempting to create a directory |
| `DirectoryRemoveError` | `warning high` | An error occurred while attempting to remove a directory |
| `FileMoveError` | `warning high` | An error occurred while attempting to move a file |
| `FileRemoveError` | `warning high` | An error occurred while attempting to remove a file |
| `AppendFileFailed` | `warning high` | The File System component returned status non-zero when trying to append 2 files together |
| `AppendFileSucceeded` | `activity high` | The File System component appended 2 files without error |
| `CreateDirectorySucceeded` | `activity high` | The File System component created a new directory without error |
| `RemoveDirectorySucceeded` | `activity high` | The File System component deleted and existing directory without error |
| `MoveFileSucceeded` | `activity high` | The File System component moved a file to a new location without error |
| `RemoveFileSucceeded` | `activity high` | The File System component deleted an existing file without error |
| `AppendFileStarted` | `activity high` | The File System component appended 2 files without error |
| `CreateDirectoryStarted` | `activity high` | The File System component began creating a new directory |
| `RemoveDirectoryStarted` | `activity high` | The File System component began deleting a directory |
| `MoveFileStarted` | `activity high` | The File System component began moving a file to a new location |
| `RemoveFileStarted` | `activity high` | The File System component began deleting an existing file |
| `FileSizeSucceeded` | `activity high` | File size response |
| `FileSizeError` | `warning high` | Failed to get file size |
| `FileSizeStarted` | `activity high` | Checking file size |
| `ListDirectoryStarted` | `activity high` | Starting directory listing |
| `ListDirectorySucceeded` | `activity high` | Directory listing completed successfully |
| `ListDirectoryError` | `warning high` | Failed to list directory |
| `DirectoryListing` | `activity high` | Directory listing file entry |
| `DirectoryListingSubdir` | `activity high` | Directory listing subdirectory entry |
| `CalculateCrcStarted` | `activity high` | CRC started |
| `CalculateCrcFailed` | `warning high` | CRC failed |
| `FileNameFormatError` | `warning high` | Error formatting a file path |
| `CalculateCrcSucceeded` | `activity high` | CRC succeeded |

### Telemetry

| Name | Type | Description |
|---|---|---|
| `CommandsExecuted` | `U32` | The total number of commands successfully executed |
| `Errors` | `U32` | The total number of errors |

<!-- fpp-dictionary-end -->
