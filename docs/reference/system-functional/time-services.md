
# Time Services Functionality

## References

- [F Prime ChronoTime SDD](https://github.com/nasa/fprime/blob/devel/Svc/ChronoTime/docs/sdd.md)
- [F Prime OsTime SDD](https://github.com/nasa/fprime/blob/devel/Svc/OsTime/docs/sdd.md)
- [F Prime PosixTime SDD](https://github.com/nasa/fprime/blob/devel/Svc/PosixTime/docs/sdd.md)
- [F Prime LinuxTimer](https://github.com/nasa/fprime/blob/devel/Svc/LinuxTimer/LinuxTimer.fpp)
- [Fw::Time SDD](https://github.com/nasa/fprime/blob/devel/Fw/Time/docs/sdd.md)

## Overview

Time services provide the system time used to tag events, telemetry, data products, and other time-stamped data throughout the flight software. A time source component implements the framework's time interface and is connected to all components that need timestamps. Separately, a timer component can serve as the primary cycle source that drives rate group scheduling.

### Time Representation

Time in F Prime is represented as a structure containing:

- A time base — a project-specific identifier for the time system (e.g., spacecraft clock, ground time)
- A time context — additional project-specific context information
- Seconds and microseconds — the time value

Time intervals use a simpler representation with just seconds and microseconds, without time base or context.

### Time Source Implementations

Multiple time source components are available:

- **ChronoTime** — Uses the C++ standard library chrono facilities to provide time. Portable across any platform with a C++ standard library.
- **PosixTime** — Uses POSIX clock_gettime to provide time. Available on POSIX-compliant systems (Linux, macOS).
- **OsTime** — Uses the OSAL RawTime interface to provide time. Available on any system that implements the OSAL, providing the broadest platform support.

A deployment selects one time source component and connects it to all components that require timestamps.

### Cycle Source

The Linux Timer provides a periodic tick source on Linux systems. It generates cycle port calls at a configured rate, driving the [rate group scheduling](rate-group-scheduling.md) chain. On other platforms, the cycle source may be provided by a hardware timer driver or other platform-specific component.

### Off Nominal

- If the system clock is not available or returns an error, the time source may return a zero or invalid time value. Components using timestamps should be resilient to this case.
