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

The version component can emit versions on startup by calling `version.start();` during start tasks.

## Change Log
| Date | Description |
|---|---|
|---| Initial Draft |
| 07/30/26 | Added `start()` information |

<!-- fpp-dictionary-begin -->
## Component Dictionary

The following tables are derived from the component's FPP model.

### Port Descriptions

| Name | Kind | Port Type | Description |
|---|---|---|---|
| `getVersion` | `guarded input` | `Svc.Version` | Mutexed Port to get values |
| `setVersion` | `guarded input` | `Svc.Version` | Mutexed Port to set values |

### Commands

| Name | Kind | Description |
|---|---|---|
| `ENABLE` | `guarded` | A command to enable or disable Event verbosity and Telemetry |
| `VERSION` | `guarded` | Report version as Event |

### Events

| Name | Severity | Description |
|---|---|---|
| `FrameworkVersion` | `activity low` | Version of the git repository. |
| `ProjectVersion` | `activity low` | Version of the git repository. |
| `LibraryVersions` | `activity low` | Version of the git repository. |
| `CustomVersions` | `activity low` | Version of the git repository. |

### Telemetry

| Name | Type | Description |
|---|---|---|
| `FrameworkVersion` | `string size 40` | Software framework version |
| `ProjectVersion` | `string size 40` | Software project version |
| `CustomVersion01` | `CustomVersionDb` | Custom Versions |
| `CustomVersion02` | `CustomVersionDb` |  |
| `CustomVersion03` | `CustomVersionDb` |  |
| `CustomVersion04` | `CustomVersionDb` |  |
| `CustomVersion05` | `CustomVersionDb` |  |
| `CustomVersion06` | `CustomVersionDb` |  |
| `CustomVersion07` | `CustomVersionDb` |  |
| `CustomVersion08` | `CustomVersionDb` |  |
| `CustomVersion09` | `CustomVersionDb` |  |
| `CustomVersion10` | `CustomVersionDb` |  |
| `LibraryVersion01` | `string size 40` | Library Versions |
| `LibraryVersion02` | `string size 40` |  |
| `LibraryVersion03` | `string size 40` |  |
| `LibraryVersion04` | `string size 40` |  |
| `LibraryVersion05` | `string size 40` |  |
| `LibraryVersion06` | `string size 40` |  |
| `LibraryVersion07` | `string size 40` |  |
| `LibraryVersion08` | `string size 40` |  |
| `LibraryVersion09` | `string size 40` |  |
| `LibraryVersion10` | `string size 40` |  |

<!-- fpp-dictionary-end -->
