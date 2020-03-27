<title>ActiveLogger Component Dictionary</title>
# ActiveLogger Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|ALOG_SET_EVENT_REPORT_FILTER|0 (0x0)|Set filter for reporting events. Events are not stored in component.| | |
| | | |FilterLevel|InputFilterLevel|Filter level|
| | | |FilterEnable|InputFilterEnabled|Filter state|
|ALOG_SET_EVENT_SEND_FILTER|1 (0x1)|Set filter for sending events. Event will not be sent as a buffer.| | |
| | | |FilterLevel|SendFilterLevel|Filter level|
| | | |FilterEnable|SendFilterEnabled|Severity filter state|
|ALOG_DUMP_EVENT_LOG|2 (0x2)|Dump circular buffers of events to a file| | |
| | | |filename|Fw::CmdStringArg||
|ALOG_SET_ID_FILTER|3 (0x3)|Filter a particular ID| | |
| | | |ID|U32||
| | | |IdFilterEnable|IdFilterEnabled|ID filter state|
|ALOG_DUMP_FILTER_STATE|4 (0x4)|Dump the filter states via events| | |


## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ALOG_FILE_WRITE_ERR|0 (0x0)|Failed to write circular buffer| | | | |
| | | |stage|LogWriteError||The write stage|
| | | |error|I32||The error code|
|ALOG_FILE_WRITE_COMPLETE|1 (0x1)|Evemt log file write complete| | | | |
| | | |records|U32||number of records written|
|ALOG_SEVERITY_FILTER_STATE|2 (0x2)|Dump severity filter state| | | | |
| | | |severity|EventFilterState||The severity level|
| | | |recvEnabled|bool|||
| | | |sendEnabled|bool|||
|ALOG_ID_FILTER_ENABLED|3 (0x3)|Indicate ID is filtered| | | | |
| | | |ID|U32||The ID filtered|
|ALOG_ID_FILTER_LIST_FULL|4 (0x4)|Attempted to add ID to full ID filter ID| | | | |
| | | |ID|U32||The ID filtered|
|ALOG_ID_FILTER_REMOVED|5 (0x5)|Removed an ID from the filter| | | | |
| | | |ID|U32||The ID removed|
|ALOG_ID_FILTER_NOT_FOUND|6 (0x6)|ID not in filter| | | | |
| | | |ID|U32||The ID removed|
