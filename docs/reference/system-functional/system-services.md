
# System Services Functionality

## References

- [F Prime SystemResources SDD](https://github.com/nasa/fprime/blob/devel/Svc/SystemResources/docs/sdd.md)
- [F Prime Version SDD](https://github.com/nasa/fprime/blob/devel/Svc/Version/docs/sdd.md)
- [F Prime AssertFatalAdapter SDD](https://github.com/nasa/fprime/blob/devel/Svc/AssertFatalAdapter/docs/sdd.md)
- [F Prime FatalHandler SDD](https://github.com/nasa/fprime/blob/devel/Svc/FatalHandler/docs/sdd.md)
- [F Prime Fatal Port SDD](https://github.com/nasa/fprime/blob/devel/Svc/Fatal/docs/sdd.md)
- [F Prime PolyDb SDD](https://github.com/nasa/fprime/blob/devel/Svc/PolyDb/docs/sdd.md)

## Overview

System services provide utility functions that support overall system operation: resource monitoring, version reporting, assertion handling, and a general-purpose value database. These components are typically present in every F Prime deployment but do not interact with each other as a subsystem — each provides an independent supporting function.

### System Resource Monitoring

The System Resources component periodically downlinks information about the health of the running system, including:

- Free memory
- CPU utilization (per core)
- Other platform-specific resource metrics

This telemetry is used by operators to monitor the overall state of the flight processor and detect resource trends.

### Version Reporting

The Version component tracks and reports version information for the framework, project, libraries, and user-defined project-specific identifiers. Version information can be configured to report automatically at system startup. This allows ground operators to confirm which software version is running on the spacecraft.

### Assert-to-Fatal Adaptation

The Assert Fatal Adapter intercepts framework-level assertions (FW_ASSERT calls) and converts them into FATAL events within the event system. This bridges the C++ assertion mechanism with the F Prime event architecture, ensuring that assertion failures are captured, logged, and handled through the standard fatal event path.

### Fatal Event Handling

The Fatal Handler component receives fatal event announcements and performs the platform-specific response to unrecoverable errors. The default implementation may log a message and halt. Deployments typically override this with project-specific behavior such as:

- Writing a crash dump to the file system
- Commanding a processor reset
- Entering a safe mode

The fatal handling path is: assertion or fatal event → Event Manager fatal announcement → Fatal Handler.

### Polymorphic Database

The Polymorphic Database (PolyDb) provides a general-purpose in-memory store for named values of varying types. Components can write values to the database and other components can read them. Values are stored using a polymorphic type that can hold any primitive type. This is useful for sharing computed values, calibration data, or algorithm state between components without requiring dedicated ports for each value.

### Off Nominal

- If the platform does not support resource queries (CPU, memory), the System Resources component reports unavailable metrics.
- Fatal handler behavior is platform-specific; the default implementation is a last-resort action and may not preserve diagnostic information.
