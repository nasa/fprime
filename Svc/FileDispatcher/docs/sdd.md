# Svc::FileDispatcher

Component to dispatch delivered files to various services. The user passes in a table that maps file extensions to numbered output ports represented by a user's own version of the enumeration in the configuration [FileDispatcherCfg.fpp file](../../../default/config/FileDispatcherCfg.fpp). The user then connects the components that process the files to the output ports.

## Requirements

Add requirements in the chart below

| Name | Description | Rationale | Validation |
|---|---|---|---|
|FPRIME-FDISP-001|File dispatcher will provide a way to dispatch files to other components|Need to have a way to supply a file name for a newly transferred file|Unit Test/System Test|
|FPRIME-FDISP-002|File dispatcher will provide user configuration to map file types to output ports|Projects will want to customize the dispatching types|Unit Test/System Test|
|FPRIME-FDISP-003|File dispatcher will dispatch to specified output ports based on the supplied table|Projects will want to connect the dispatch types to components for processing new files|Unit Test/System Test|

## Usage Examples

### Typical Usage

`Svc::FileUplink` is typically connected to `FileDispatcher` to process files that have been uplinked to the system. The user should create a configuration table that maps file extensions to output ports and supply it to the `FileDispatcher` component during initialization.

#### Configuration Table Structure

The user should instantiate an instance of `FileDispatcherTable` and supply it to the `configure()` function during initialization. The fields should be filled in as follows:

|Field|Value|Notes|
|---|---|---|
|`numEntries`|Should be > 0 and <= `Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE`|Values outside this range will cause an assert|
|`entries`|An array of file extension to port mappings|Number populated should match `numEntries`|

The user should configure each entry in the `entries` array in the range covered by `numEntries`.

The fields should be filled as follows:

|Field|Value|Notes|
|---|---|---|
|`fileExt`|The file extension or suffix to detect in the file name and route. The same extension/suffix can be used multiple times and be routed to different ports|Must be non-zero in length, or it will cause an assert|
|`port`|The outgoing port number must be a member of the `FileDispatchPort` or it will cause an assert|
|`enabled`|A boolean that indicates if the routing is initially enabled or not|Can be changed by `ENABLE_DISPATCH` command|

## Diagrams

### Class Diagram

![`FileDispatcher` Diagram](img/FileDispatcher.drawio.png "FileUplink")

## Port Descriptions

| Name | Description |
|---|---|
|`fileAnnounceRecv`|Input port that receives file name of new file|
|`fileDispatch`|Array of output ports that dispatch a new files based on a user supplied table|
|`pingIn`|Input port for Health pinging
|`pingOut`|Output port for returning Health pings

## Component States
No state machines

## Parameters
No parameters

## Commands
| Name | Description |
|---|---|
|`ENABLE_DISPATCH`|Enables or disables a particular dispatch type|

## Events
| Name | Description |
|---|---|
|`FileDispatchState`|Sent when a dispatch type is enabled or disabled|
|`FileDispatched`|Sent when a dispatch type is matched and dispatched|

## Telemetry

No telemetry

## Unit Tests

| Name | Description | Output | Coverage |
|---|---|---|---|
|`dispatchTest`|Tests dispatches of files|---|---|
|`dispatchAllDisabledTest`|Tests dispatches of files with all file types initially disabled|---|---|
|`dispatchAllCmdDisabledTest`|Tests dispatches of files with all file types initially enabled, but then disabled by command|---|---|
|`dispatchAllCmdEnabledTest`|Tests dispatches of files with all file types initially disabled, but then enabled by command|---|---|
|`dispatchNotFullConfigTest`|Tests dispatches of files with a table with less entries than output ports|---|---|
|`dispatchPingTest`|Tests ping returns|---|---|


## Change Log
| Date | Description |
|---|---|
|12/10/2025| Initial Draft |
