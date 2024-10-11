# Svc::Version

Tracks versions for framework,project, libraries and user defined project specific versions.

## Requirements

| Name | Description | Validation |
|---|---|---|
|SVC-VERSION-001|`Svc::Version` upon startup shall generate an event and a telemetry channel with version for framework.| This is to provide transparency on framework version being used|
|SVC-VERSION-002|`Svc::Version` upon startup shall generate an event and a telemetry channel with version for project | This is to provide transparency on project version being used|
|SVC-VERSION-003|`Svc::Version` upon startup shall generate events and telemetry channels (upto 10) with versions for library.| Transparency on different library versions|
|SVC-VERSION-004|`Svc::Version` upon startup shall make verbosity on custom versions configurable.| Verbosity will enable/disable the channel generation but will not effect the event generation|
|SVC-VERSION-005|`Svc::Version` shall provide a ground command to request events and telemetry on framework version| Accessibility on demand|
|SVC-VERSION-006|`Svc::Version` shall provide a ground command to request events and telemetry on project version| Accessibility on demand|
|SVC-VERSION-007|`Svc::Version` shall provide a ground command to request events and telemetry channels (upto 10) on library versions| Accessibility on demand|
|SVC-VERSION-008|`Svc::Version` shall provide a ground command to request events and telemetry channels (upto 10) on custom versions| Accessibility on demand. Verbosity configuration will determine the channel generation|
|SVC-VERSION-009|`Svc::Version` shall provide a ground command to enable/disable verbosity on custom versions| Accessibility on demand|
|SVC-VERSION-010|`Svc::Version` shall provide a telemetry channel on framework version| Accessibility to versions being used|
|SVC-VERSION-011|`Svc::Version` shall provide a telemetry channel on project version| Accessibility to versions being used|
|SVC-VERSION-012|`Svc::Version` shall provide upto 10 telemetry channels on library versions| Accessibility to versions being used|
|SVC-VERSION-013|`Svc::Version` shall provide upto 10 telemetry channels on custom versions| Accessibility to versions being used. Only accessible if verbosity is enabled|
|SVC-VERSION-014|`Svc::Version` shall provide an interface for other components to set custom versions.| Enables projects to set hardware and FPGA versions, say, as needed. Also generates Events/TLM based on verbosity configuration|
|SVC-VERSION-015|`Svc::Version` shall provide an interface for other components to get custom versions.| Also generates Events/TLM based on verbosity configuration|

## Emitting Versions on Start-Up

The version component can emit versions on startup by calling `version.config(true);` during component configuration.

## Change Log
| Date | Description |
|---|---|
|---| Initial Draft |
