<title>FileDownlink Component Dictionary</title>
# FileDownlink Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|FileDownlink_SendFile|0 (0x0)|Read a named file off the disk. Divide it into packets and send the packets for transmission to the ground.| | |
| | | |sourceFileName|Fw::CmdStringArg|The name of the on-board file to send|
| | | |destFileName|Fw::CmdStringArg|The name of the destination file on the ground|
|FileDownlink_Cancel|1 (0x1)|Cancel the downlink in progress, if any| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|FileDownlink_FilesSent|0 (0x0)|U32|The total number of files sent|
|FileDownlink_PacketsSent|1 (0x1)|U32|The total number of packets sent|
|FileDownlink_Warnings|2 (0x2)|U32|The total number of warnings|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FileDownlink_FileOpenError|0 (0x0)|An error occurred opening a file| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the file|
|FileDownlink_FileReadError|1 (0x1)|An error occurred reading a file| | | | |
| | | |fileName|Fw::LogStringArg&|60|The name of the file|
|FileDownlink_FileSent|2 (0x2)|The File Downlink component successfully sent a file| | | | |
| | | |sourceFileName|Fw::LogStringArg&|60|The source file name|
| | | |destFileName|Fw::LogStringArg&|60|The destination file name|
|FileDownlink_DownlinkCanceled|3 (0x3)|The File Downlink component canceled downlink of a file| | | | |
| | | |sourceFileName|Fw::LogStringArg&|60|The source file name|
| | | |destFileName|Fw::LogStringArg&|60|The destination file name|
