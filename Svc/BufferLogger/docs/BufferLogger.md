<title>BufferLogger Component Dictionary</title>
# BufferLogger Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|BL_OpenFile|0 (0x0)|Open a new log file with specified name; also resets unique file counter to 0| | |
| | | |file|Fw::CmdStringArg||
|BL_CloseFile|1 (0x1)|Close the currently open log file, if any| | |
|BL_SetLogging|2 (0x2)|Sets the volatile logging state| | |
| | | |state|LogState||
|BL_FlushFile|3 (0x3)|Flushes the current open log file to disk; a no-op with fprime's unbuffered file I/O, so always returns success| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|BufferLogger_NumLoggedBuffers|0 (0x0)|U32|The number of buffers logged|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|BL_LogFileClosed|0 (0x0)|The Buffer Logger closed a log file| | | | |
| | | |file|Fw::LogStringArg&|256|The file|
|BL_LogFileOpenError|1 (0x1)|The Buffer Logger encountered an error opening a log file| | | | |
| | | |errornum|U32||The error number returned from the open operation|
| | | |file|Fw::LogStringArg&|256|The file|
|BL_LogFileValidationError|2 (0x2)|The Buffer Logger encountered an error writing a validation file| | | | |
| | | |validationFile|Fw::LogStringArg&|256|The validation file|
| | | |status|U32||The Os::Validate::Status return|
|BL_LogFileWriteError|3 (0x3)|The Buffer Logger encountered an error writing to a log file| | | | |
| | | |errornum|U32||The error number returned from the write operation|
| | | |bytesWritten|U32||The number of bytes successfully written|
| | | |bytesToWrite|U32||The number of bytes attempted|
| | | |file|Fw::LogStringArg&|256|The file|
|BL_Activated|4 (0x4)|Buffer logger was activated| | | | |
|BL_Deativated|5 (0x5)|Buffer logger was deactivated| | | | |
|BL_NoLogFileOpenInitError|6 (0x6)|No log file open command was received by BufferLogger| | | | |
