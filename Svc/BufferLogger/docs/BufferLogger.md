<title>BufferLogger Component Dictionary</title>
# BufferLogger Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|BL_CloseFile|0 (0x0)|Close the currently open log file, if any| | |   
|BL_SetLogging|1 (0x1)|Sets the volatile logging state| | |   
| | | |state|LogState||                    
|BL_FlushFile|2 (0x2)|Flushes the current open log file to disk| | |   


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
