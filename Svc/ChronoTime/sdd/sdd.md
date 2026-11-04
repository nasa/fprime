# Svc::ChronoTime

As an F Prime project matures, a custom time component will likely be developed soliciting time in the project directed
way. For reference projects and projects in an early phase of development, this custom time component does not exist.
This miss-match was historically filled by Svc::PosixTime (formerly Svc::LinuxTime), however; this requires a project to
support posix.

The std::chrono library provided by C++11 is a language feature and as such can implement time without the need for
posix.

This is a C++11 implementation of the TimeInterface used to supply time to the various parts of the system.

## Requirements

The following requirements describe the behavior of Svc::ChronoTime.

| Name                | Description                                                              | Validation |
|---------------------|--------------------------------------------------------------------------|------------|
| SVC_CHRONO_TIME_001 | Svc::ChronoTime shall be implemented using only C++11 language features. | Inspection |
| SVC_CHRONO_TIME_002 | Svc::ChronoTime shall implement the Svc.TimeInterface.                   | Unit-Test  |
| SVC_CHRONO_TIME_003 | Svc::ChronoTime shall provide time with resolution to microseconds.      | Unit-Test  |


## Usage Examples

In order to use this component, projects must supply `TimeBase::TB_WORKSTATION_TIME` in their `TimeBase` enumeration.

To use Svc::ChronoTime in your project, make sure to set the time service with the following lines in your topology:

**Add the instance to the instance list:**
```
    instance chronoTime: Svc.PosixTime base id 0x4500
```

**Use the instance to supply time:**
```
    time connections instance chronoTime
```

> Since an implementation of the time interface comes with he standard topology template, projects should replace it.

## Change Log
| Date       | Description   |
|------------|---------------|
| 2024-11-25 | Initial Draft |