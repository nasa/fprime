# Svc::Version

Tracks versions for framework,project, libraries and user defined project specific versions.

## Usage Examples
Add usage examples here

### Diagrams
Add diagrams here

### Typical Usage
And the typical usage of the component here

## Class Diagram
Add a class diagram here

## Port Descriptions
| Name | Description |
|---|---|
|---|---|

## Component States
Add component states in the chart below
| Name | Description |
|---|---|
|---|---|

## Sequence Diagrams
Add sequence diagrams here

## Parameters
| Name | Description |
|---|---|
|---|---|

## Commands
| Name | Description |
|---|---|
|---|---|

## Events
| Name | Description |
|---|---|
|---|---|

## Telemetry
| Name | Description |
|---|---|
|---|---|

## Unit Tests
Add unit test descriptions in the chart below
| Name | Description | Output | Coverage |
|---|---|---|---|
|---|---|---|---|

## Requirements

| Name | Description | Validation |
|---|---|---|
|SVC-VERSION-001|`Svc::Version` upon startup shall generate an event and a telemetry channel with version for framework.| This is to provide transparency on framework version being used|
|SVC-VERSION-002|`Svc::Version` upon startup shall generate an event and a telemetry channel with version for project | This is to provide transparency on project version being used|
|SVC-VERSION-003|`Svc::Version` upon startup shall generate events and telemetry channels (upto 10) with versions for libary.| Transparency on different library versions|
|SVC-VERSION-004|`Svc::Version` upon startup shall make verbosity on custom versions configurable.| Transparency on different library versions|
|SVC-VERSION-005|`Svc::Version` shall provide a ground command to request events and telemetry on framework version| Accessiblity on demand|
|SVC-VERSION-006|`Svc::Version` shall provide a ground command to request events and telemetry on project version| Accessiblity on demand|
|SVC-VERSION-007|`Svc::Version` shall provide a ground command to request events and telemetry channels (upto 10) on library versions| Accessiblity on demand|
|SVC-VERSION-008|`Svc::Version` shall provide a ground command to request events and telemetry channels (upto 10) on custom versions| Accessiblity on demand|
|SVC-VERSION-009|`Svc::Version` shall provide a ground command to enable/disable verbosity on custom versions| Accessiblity on demand|
|SVC-VERSION-010|`Svc::Version` shall provide a telemetry channel on framework version| Accessiblity to versions being used|
|SVC-VERSION-011|`Svc::Version` shall provide a telemetry channel on project version| Accessiblity to versions being used|
|SVC-VERSION-012|`Svc::Version` shall provide upto 10 telemetry channels on library versions| Accessiblity to versions being used|
|SVC-VERSION-013|`Svc::Version` shall provide upto 10 telemetry channels on custom versions| Accessiblity to versions being used|
|SVC-VERSION-014|`Svc::Version` shall provide an interface for other components to set custom versions.| Enables projects to set hardware and FPGA versions, say, as needed. Also generates EVRs/EHAs|
|SVC-VERSION-015|`Svc::Version` shall provide an interface for other components to get custom versions.| Also generates EVRs/EHAs|


## Change Log
| Date | Description |
|---|---|
|---| Initial Draft |