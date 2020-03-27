<title>ActiveRateGroup Component Dictionary</title>
# ActiveRateGroup Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|RgMaxTime|0 (0x0)|U32|Max execution time rate group|
|RgCycleSlips|1 (0x1)|U32|Cycle slips for rate group|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|RateGroupStarted|0 (0x0)|Informational event that rate group has started| | | | |
|RateGroupCycleSlip|1 (0x1)|Warning event that rate group has had a cycle slip| | | | |
| | | |cycle|U32||The cycle where the cycle occurred|
