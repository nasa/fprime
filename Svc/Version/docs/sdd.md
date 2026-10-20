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
|SVC-VERSION-016|`Svc::Version` shall provide a scheduled input port that periodically re-emits the framework, project and library version telemetry channels, and the populated custom version telemetry channels when verbosity is enabled, without generating events.| Allows ground systems that connect after startup, or that miss the one-shot telemetry write, to receive version telemetry periodically without flooding the event log|

## Emitting Versions on Start-Up

The version component can emit versions on startup by calling `version.start();` during start tasks.

## Periodically Re-Emitting Versions

The version component exposes an optional scheduled input port, `schedIn` (`Svc.Sched`), that can be connected to a rate group or other scheduler. On each invocation, `schedIn_handler` re-emits all version telemetry channels (framework, project, library and custom) **without** generating events, so operators can confirm version telemetry on a periodic basis without flooding the event log. Each invocation re-writes up to 22 channels (2 + 10 library + 10 custom) even though their values rarely change, which adds to downlink bandwidth and to `TlmChan`'s `TLMCHAN_MAX_ENTRIES_PER_RUN` budget; connect `schedIn` to a slow rate group (e.g. 1 Hz or slower) sized for the link.

This is achieved via a `sendEvent` function argument (default `true`) on the internal version-emit helpers, which decouples telemetry writes from event emission. The scheduled handler calls each helper with `sendEvent = false`:

- Framework, project and library telemetry are always re-emitted.
- Custom-version telemetry remains gated by the existing `ENABLE` verbosity configuration (see SVC-VERSION-013).

The port is optional and additive: deployments that do not connect `schedIn` are unaffected, and the startup (`start()`) and `VERSION` command paths continue to emit events as before (`sendEvent = true`). The `CdhCore` subtopology does not connect `version.schedIn`; deployments using `CdhCore` that want periodic re-emission connect `CdhCore.version.schedIn` to a rate group in their own topology.

## Change Log
| Date | Description |
|---|---|
|---| Initial Draft |
| 07/30/26 | Added `start()` information |
| 10/14/26 | Added `schedIn` scheduled port for periodic version re-emission without events |
