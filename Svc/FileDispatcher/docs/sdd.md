# Svc::FileDispatcher

Component to dispatch delivered files to various services. The user passes in a table that maps file extensions to numbered output ports represented by their own version of the enumeration in the configuration [file](../../../default/config/FileDispatcherCfg.fpp). The user then connects the components that process the files to the output ports.

## Requirements

Add requirements in the chart below

| Name | Description | Rationale | Validation |
|---|---|---|---|
|FPRIME-FDISP-001|File dipatcher will provide a way to dispatch files to other components|Need to have a way to supply a new file name|Unit Test/System Test|
|FPRIME-FDISP-002|File dipatcher will provide user configuration to map file types to output ports|Projects will want to customize the dipatching types|Unit Test/System Test|
|FPRIME-FDISP-003|File dipatcher will dispatch to specified output ports based on the supplied table|Projects will want to connect the dispatch types to components for utilizing files|Unit Test/System Test|

## Usage Examples

### Typical Usage

The `Ref` example connects `Svc::FIleUplink` to `FileDispatcher` to process uplinked files. The `Ref` example configuration table has compiled sequences with a certain extension that `FileDispatcher` dispatches to `Svc::CmdSequencer` for automatic running when those files are uplinked.

## Diagrams

### Class Diagram

![`FileDispatcher` Diagram](img/FileDispatcher.drawio.png "FileUplink")

## Port Descriptions

| Name | Description |
|---|---|
|`fileAnnounceRecv`|Input port that receives file name of new file|
|`fileDispatch`|Output port that dipatches a new file based on a user supplied table|

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
|`dispatchAllCmdEnabledTest`|Tests dispatches of files with all file types initially disabled, but then enaabled by command|---|---|
|`dispatchNotFullConfigTest`|Tests dispatches of files with a table with less entries than output ports|---|---|


## Change Log
| Date | Description |
|---|---|
|12/7/2025| Initial Draft |