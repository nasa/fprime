<title>CmdSequencer Component Dictionary</title>
# CmdSequencer Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|CS_RUN|0 (0x0)|Run a command sequence file| | |
| | | |fileName|Fw::CmdStringArg|The name of the sequence file|
|CS_VALIDATE|1 (0x1)|Validate a command sequence file| | |
| | | |fileName|Fw::CmdStringArg|The name of the sequence file|
|CS_CANCEL|2 (0x2)|Cancel a command sequence| | |
|CS_START|3 (0x3)|Start running a command sequence| | |
|CS_STEP|4 (0x4)|Perform one step in a command sequence. Valid only if CmdSequencer is in MANUAL run mode.| | |
|CS_AUTO|5 (0x5)|Set the run mode to AUTO.| | |
|CS_MANUAL|6 (0x6)|Set the run mode to MANUAL.| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|CS_LoadCommands|0 (0x0)|U32|The number of Load commands executed|
|CS_CancelCommands|1 (0x1)|U32|The number of Cancel commands executed|
|CS_Errors|2 (0x2)|U32|The number of errors that have occurred|
|CS_CommandsExecuted|3 (0x3)|U32|The number of commands executed across all sequences.|
|CS_SequencesCompleted|4 (0x4)|U32|The number of sequences completed.|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|CS_SequenceLoaded|0 (0x0)|Sequence file was successfully loaded.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
|CS_SequenceCanceled|1 (0x1)|A command sequence was successfully canceled.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
|CS_FileReadError|2 (0x2)|The Sequence File Loader could not read the sequence file.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
|CS_FileInvalid|3 (0x3)|The sequence file format was invalid.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |stage|FileReadStage||The read stage|
| | | |error|I32||The error code|
|CS_RecordInvalid|4 (0x4)|The format of a command record was invalid.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |recordNumber|U32||The record number|
| | | |error|I32||The error code|
|CS_FileSizeError|5 (0x5)|The sequence file was too large.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |size|U32||Inavalid size|
|CS_FileNotFound|6 (0x6)|The sequence file was not found| | | | |
| | | |fileName|Fw::LogStringArg&|60|The sequence file|
|CS_FileCrcFailure|7 (0x7)|The sequence file validation failed| | | | |
| | | |fileName|Fw::LogStringArg&|60|The sequence file|
| | | |storedCRC|U32||The CRC stored in the file|
| | | |computedCRC|U32||The CRC computed over the file|
|CS_CommandComplete|8 (0x8)|The Command Sequencer issued a command and received a success status in return.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |recordNumber|U32||The record number of the command|
| | | |opCode|U32||The command opcode|
|CS_SequenceComplete|9 (0x9)|A command sequence successfully completed.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
|CS_CommandError|10 (0xa)|The Command Sequencer issued a command and received an error status in return.| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |recordNumber|U32||The record number|
| | | |opCode|U32||The opcode|
| | | |errorStatus|U32||The error status|
|CS_InvalidMode|11 (0xb)|The Command Sequencer received a command that was invalid for its current mode.| | | | |
|CS_RecordMismatch|12 (0xc)|Number of records in header doesn't match number in file| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |header_records|U32||The number of records in the header|
| | | |extra_bytes|U32||The number of bytes beyond last record|
|CS_TimeBaseMismatch|13 (0xd)|The running time base doesn't match the time base in the sequence files| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |time_base|U16||The current time|
| | | |seq_time_base|U16||The sequence time base|
|CS_TimeContextMismatch|14 (0xe)|The running time base doesn't match the time base in the sequence files| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the sequence file|
| | | |currTimeBase|U8||The current time base|
| | | |seqTimeBase|U8||The sequence time base|
|CS_PortSequenceStarted|15 (0xf)|A local port request to run a sequence was started| | | | |
| | | |filename|Fw::LogStringArg&|60|The sequence file|
|CS_UnexpectedCompletion|16 (0x10)|A command status came back when no sequence was running| | | | |
| | | |opcode|U32||The reported opcode|
|CS_ModeSwitched|17 (0x11)|Switched step mode| | | | |
| | | |mode|SeqMode||The new mode|
|CS_NoSequenceActive|18 (0x12)|A sequence related command came with no active sequence| | | | |
|CS_SequenceValid|19 (0x13)|A sequence passed validation| | | | |
| | | |filename|Fw::LogStringArg&|60|The sequence file|
|CS_SequenceTimeout|20 (0x14)|A sequence passed validation| | | | |
| | | |filename|Fw::LogStringArg&|60|The sequence file|
| | | |command|U32||The command that timed out|
|CS_CmdStepped|21 (0x15)|A commmand in a sequence was stepped through| | | | |
| | | |filename|Fw::LogStringArg&|60|The sequence file|
| | | |command|U32||The command that was stepped|
|CS_CmdStarted|22 (0x16)|A manual sequence was started| | | | |
| | | |filename|Fw::LogStringArg&|60|The sequence file|
