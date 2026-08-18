# Svc::ComLogger

## 1. Introduction

The ComLogger component logs `Fw::Com` buffers (e.g., framed telemetry, events, or command packets) to files on the on-board filesystem. Each received Com buffer is written to the currently open log file, optionally prefixed with a 2-byte length field. Log files are timestamped and automatically rotated when they reach a configured maximum size, and a companion hash file is written on close for ground-side validation.

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-COMLOGGER-001 | The ComLogger component shall log the contents of Com buffers received on its `comIn` port | unit test |
| SVC-COMLOGGER-002 | The ComLogger component shall optionally prefix each logged buffer with a 2-byte (U16) length field | unit test |
| SVC-COMLOGGER-003 | The ComLogger component shall close the current file and open a new one when the configured maximum file size would be exceeded | unit test |
| SVC-COMLOGGER-004 | The ComLogger component shall name log files using the configured prefix and the time of file opening | inspection |
| SVC-COMLOGGER-005 | The ComLogger component shall write a hash (validation) file when closing each log file | unit test |
| SVC-COMLOGGER-006 | The ComLogger component shall provide a command to close the currently open file | unit test |
| SVC-COMLOGGER-007 | The ComLogger component shall report file errors via events | unit test |

## 3. Design

The ComLogger is an active component. Com buffers arriving on the async `comIn` port are dispatched on the component's thread and written synchronously to the log file.

### 3.1 Port Description

| Port | Kind | Type | Description |
|---|---|---|---|
| `comIn` | async input | `Fw.Com` | Com buffers to log |
| `pingIn` / `pingOut` | async input / output | `Svc.Ping` | Health ping |

### 3.2 File Management

* Log files are named `<prefix>_<timeBase>_<seconds>_<microseconds>.com`, using the component's time at file-open.
* Files are opened lazily on the first received buffer and re-opened after rotation or a commanded close.
* When `storeBufferLength` is enabled (the default), each buffer is preceded by its size as a U16; this permits ground tools to re-segment the log into individual packets.
* When a write would exceed `maxFileSize`, the current file is closed and a new one opened.
* On close, a companion hash file (`<fileName><hash extension>`) is written via `Utils::Hash`; failures produce `FileValidationError`.
* Open and write errors are reported once per file via `FileOpenError` and `FileWriteError` to avoid event flooding.

### 3.3 Initialization

The component may be constructed either fully initialized — `ComLogger(compName, filePrefix, maxFileSize, storeBufferLength)` — or with the name-only constructor followed by a call to `init_log_file(filePrefix, maxFileSize, storeBufferLength)` before logging begins. Receiving data while uninitialized produces the throttled `FileNotInitialized` warning.

### 3.4 Commands

| Command | Description |
|---|---|
| `CloseFile` | Force a close of the currently opened file, if any |

### 3.5 Events

| Event | Severity | Description |
|---|---|---|
| `FileOpenError` | warning high | Error opening a log file |
| `FileWriteError` | warning high | Error writing to a log file |
| `FileValidationError` | warning low | Error creating a validation (hash) file |
| `FileClosed` | diagnostic | A file was closed successfully on command |
| `FileNotInitialized` | warning low | Data received before initialization (throttled after 5 occurrences) |

## 4. Usage

1. **Instantiate** the component in the topology and start its thread.
2. **Initialize** with a file prefix (which may include a directory path), maximum file size, and buffer-length storage option — either via the initializing constructor or `init_log_file`.
3. **Connect** the com stream to be logged (e.g., the downlink stream out of a framer or `Svc::ComQueue`) to `comIn`.
4. **Close files** on demand with the `CloseFile` command; files also close automatically on rotation and component destruction.

## 5. Change Log

| Date | Description |
|---|---|
| 2026-08-10 | Initial SDD |
