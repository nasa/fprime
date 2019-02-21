<title>PrmDb Component Dictionary</title>
# PrmDb Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|PRM_SAVE_FILE|0 (0x0)|Command to save parameter image to file. Uses file name passed to constructor| | |


## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|PrmIdNotFound|0 (0x0)|Parameter ID not found in database.| | | | |
| | | |Id|U32||The parameter ID|
|PrmIdUpdated|1 (0x1)|Parameter ID updated in database| | | | |
| | | |Id|U32||The parameter ID|
|PrmDbFull|2 (0x2)|Parameter database is full| | | | |
| | | |Id|U32||The parameter ID|
|PrmIdAdded|3 (0x3)|Parameter ID added to database| | | | |
| | | |Id|U32||The parameter ID|
|PrmFileWriteError|4 (0x4)|Failed to write parameter file| | | | |
| | | |stage|PrmWriteError||The write stage|
| | | |record|I32||The record that had the failure|
| | | |error|I32||The error code|
|PrmFileSaveComplete|5 (0x5)|Save of parameter file completed| | | | |
| | | |records|U32||The number of records saved|
|PrmFileReadError|6 (0x6)|Failed to read parameter file| | | | |
| | | |stage|PrmReadError||The write stage|
| | | |record|I32||The record that had the failure|
| | | |error|I32||The error code|
|PrmFileLoadComplete|7 (0x7)|Load of parameter file completed| | | | |
| | | |records|U32||The number of records loaded|
