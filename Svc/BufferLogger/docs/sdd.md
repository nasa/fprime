# Svc::BufferLogger

## 1. Introduction

The BufferLogger component logs binary data to files on the on-board filesystem. It accepts data either as `Fw::Buffer` objects (typically raw payloads such as sensor data) or as `Fw::Com` buffers (typically packetized telemetry or events), and writes each received item to the currently open log file, prefixed with a length field. Buffers received on the `bufferSendIn` port are forwarded downstream after logging so the component can be inserted transparently into a buffer pipeline.

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-BUFFERLOGGER-001 | The BufferLogger component shall log the contents of buffers received on its `bufferSendIn` port | unit test |
| SVC-BUFFERLOGGER-002 | The BufferLogger component shall log the contents of Com buffers received on its `comIn` port | unit test |
| SVC-BUFFERLOGGER-003 | The BufferLogger component shall forward buffers received on `bufferSendIn` to `bufferSendOut` regardless of logging state | unit test |
| SVC-BUFFERLOGGER-004 | The BufferLogger component shall prefix each logged buffer with a configurable-width size field | unit test |
| SVC-BUFFERLOGGER-005 | The BufferLogger component shall open a new log file when the configured maximum file size would be exceeded | unit test |
| SVC-BUFFERLOGGER-006 | The BufferLogger component shall provide commands to open a log file, close the log file, flush the log file, and enable or disable logging | unit test |
| SVC-BUFFERLOGGER-007 | The BufferLogger component shall write a hash (validation) file when closing each log file | unit test |
| SVC-BUFFERLOGGER-008 | The BufferLogger component shall report file errors via events | unit test |

## 3. Design

The BufferLogger is an active component. Data arriving on the async `bufferSendIn` and `comIn` ports is dispatched on the component's thread and written synchronously to the log file.

### 3.1 Port Description

| Port | Kind | Type | Description |
|---|---|---|---|
| `bufferSendIn` | async input | `Fw.BufferSend` | Buffers to log; forwarded to `bufferSendOut` after logging |
| `bufferSendOut` | output | `Fw.BufferSend` | Forwarded buffers (e.g., for return to a buffer manager) |
| `comIn` | async input | `Fw.Com` | Com buffers to log (not forwarded) |
| `pingIn` / `pingOut` | async input / output | `Svc.Ping` | Health ping |
| `schedIn` | async input | `Svc.Sched` | Run-time scheduling (currently unused) |

### 3.2 Logging State

Logging is controlled by a volatile state (`LogState`): `LOGGING_ON` (default at construction) or `LOGGING_OFF`, set by the `BL_SetLogging` command. When logging is off, incoming data is not written, but `bufferSendIn` data is still forwarded to `bufferSendOut`. Setting the state to `LOGGING_OFF` closes the current file.

### 3.3 File Management

* File names have the form `<prefix><baseName><suffix>` for the first file after `BL_OpenFile`, and `<prefix><baseName><counter><suffix>` for subsequent files, where the counter increments each time a file fills up.
* Each logged buffer is written as a size field (of `sizeOfSize` bytes, configured via `initLog`) followed by the buffer data.
* When a write would exceed the configured maximum file size, the current file is closed and a new one is opened.
* On close, a companion hash file is written for ground-side validation (see `Utils::Hash`); failures produce `BL_LogFileValidationError`.
* Logging before any `BL_OpenFile` command has set a base name produces `BL_NoLogFileOpenInitError`.

### 3.4 Commands

| Command | Description |
|---|---|
| `BL_OpenFile` | Set the log file base name and reset the unique file counter to 0 |
| `BL_CloseFile` | Close the currently open log file, if any |
| `BL_SetLogging` | Set the volatile logging state (`LOGGING_ON` / `LOGGING_OFF`) |
| `BL_FlushFile` | Flush the open log file to disk; a no-op with F Prime's unbuffered file I/O, so always returns success |

### 3.5 Events

| Event | Severity | Description |
|---|---|---|
| `BL_LogFileClosed` | diagnostic | A log file was closed |
| `BL_LogFileOpenError` | warning high | Error opening a log file |
| `BL_LogFileValidationError` | warning high | Error writing a validation (hash) file |
| `BL_LogFileWriteError` | warning high | Error writing to a log file |
| `BL_Activated` / `BL_Deactivated` | activity low | Logging was enabled / disabled |
| `BL_NoLogFileOpenInitError` | warning high | Data received before a log file base name was set |
| `BL_LogFileNameError` | warning high | Error formatting a log file name |

### 3.6 Telemetry

| Channel | Type | Description |
|---|---|---|
| `BufferLogger_NumLoggedBuffers` | U32 | The number of buffers logged |

## 4. Usage

1. **Instantiate** the component in the topology and start its thread.
2. **Initialize the log parameters** by calling `initLog(prefix, suffix, maxFileSize, sizeOfSize)` from topology setup code.
3. **Open a log file** by sending the `BL_OpenFile` command with the desired base name.
4. **Stream data** into `bufferSendIn` and/or `comIn`.
5. **Close or rotate** files with `BL_CloseFile`/`BL_OpenFile` as needed.

Note that `bufferSendIn` forwards ownership of each buffer to `bufferSendOut`; the topology must route the forwarded buffers to their owner (e.g., a `Svc::BufferManager`).

## 5. Change Log

| Date | Description |
|---|---|
| 2026-08-10 | Initial SDD |
