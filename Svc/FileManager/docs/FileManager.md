<title>FileManager Component Dictionary</title>
# FileManager Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|CreateDirectory|0 (0x0)|Create a directory| | |
| | | |dirName|Fw::CmdStringArg|The directory to create|
|MoveFile|1 (0x1)|Move a file| | |
| | | |sourceFileName|Fw::CmdStringArg|The source file name|
| | | |destFileName|Fw::CmdStringArg|The destination file name|
|RemoveDirectory|2 (0x2)|Remove a directory, which must be empty| | |
| | | |dirName|Fw::CmdStringArg|The directory to remove|
|RemoveFile|3 (0x3)|Remove a file| | |
| | | |fileName|Fw::CmdStringArg|The file to remove|
|ShellCommand|4 (0x4)|Perform a Linux shell command and write the output to a log file.| | |
| | | |command|Fw::CmdStringArg|The shell command string|
| | | |logFileName|Fw::CmdStringArg|The name of the log file|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|CommandsExecuted|0 (0x0)|U32|The total number of commands successfully executed|
|Errors|1 (0x1)|U32|The total number of errors|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|DirectoryCreateError|0 (0x0)|An error occurred while attempting to create a directory| | | | |
| | | |dirName|Fw::LogStringArg&|256|The name of the directory|
| | | |status|U32||The error status|
|DirectoryRemoveError|1 (0x1)|An error occurred while attempting to remove a directory| | | | |
| | | |dirName|Fw::LogStringArg&|256|The name of the directory|
| | | |status|U32||The error status|
|FileMoveError|2 (0x2)|An error occurred while attempting to move a file| | | | |
| | | |sourceFileName|Fw::LogStringArg&|256|The name of the source file|
| | | |destFileName|Fw::LogStringArg&|256|The name of the destination file|
| | | |status|U32||The error status|
|FileRemoveError|3 (0x3)|An error occurred while attempting to remove a file| | | | |
| | | |fileName|Fw::LogStringArg&|256|The name of the file|
| | | |status|U32||The error status|
|ShellCommandFailed|4 (0x4)|The File System component executed a shell command that returned status non-zero| | | | |
| | | |command|Fw::LogStringArg&|256|The command string|
| | | |status|U32||The status code|
|ShellCommandSucceeded|5 (0x5)|The File System component executed a shell command that returned status zero| | | | |
| | | |command|Fw::LogStringArg&|256|The command string|
