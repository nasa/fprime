
# Event Management Functionality

## References

- [F Prime EventManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/EventManager/docs/sdd.md)
- [F Prime ActiveTextLogger SDD](https://github.com/nasa/fprime/blob/devel/Svc/ActiveTextLogger/docs/sdd.md)
- [F Prime PassiveConsoleTextLogger SDD](https://github.com/nasa/fprime/blob/devel/Svc/PassiveConsoleTextLogger/docs/sdd.md)
- [FPP User Guide — Events](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Events)

## Overview

Event management collects time-tagged log entries from components across the system, packages them for downlink, and optionally logs human-readable text to the console. Events are the primary mechanism for recording system activities, anomalies, and state changes in F Prime. Every component can emit events at various severity levels, and the event management subsystem ensures they are captured and delivered.

### Event Collection and Distribution

The Event Manager is an active component that receives events from all components in the system. Events arrive as serialized data containing the event ID, severity, timestamp, and arguments. The Event Manager packages these events into communication buffers and sends them to the downlink path for transmission to the ground system.

### Event Severity Levels

Events are categorized by severity to support filtering and prioritization:

- **FATAL** — A condition rendering the software unable to continue. Issuing a FATAL event triggers the fatal handler (see Fatal Event Handling below).
- **WARNING_HI** — A serious failure, but the software can continue
- **WARNING_LO** — A failure with minimal overall impact
- **COMMAND** — Activity related to command processing
- **ACTIVITY_HI** — An important nominal event
- **ACTIVITY_LO** — A minor nominal event, typically used for background activities
- **DIAGNOSTIC** — Detailed debugging information, normally suppressed

### Event Throttling

Events can be configured with throttle limits to prevent log flooding when a condition triggers the same event repeatedly. Throttling is configured on the individual component that emits the event. Two throttling modes are available:

- **Count-based** — Limits the number of times an event can be reported before it is suppressed. A specific clear action resets the throttle counter.
- **Time-based** — Limits the rate at which an event can be reported based on elapsed time.

### Text Logging

In addition to binary event distribution for downlink, F Prime supports human-readable text logging to the console. Two text logging components are available:

- **Active Text Logger** — Has its own thread and can optionally write log text to a file in addition to standard output.
- **Passive Console Text Logger** — Prints text events directly to stdout in the caller's thread.

Text logging is primarily used for development and ground testing. It is connected via a separate text event port that carries pre-formatted strings.

### Fatal Event Handling

Fatal events receive special handling through a dedicated path. When a fatal event is issued, the Event Manager announces it via a fatal announcement port. This is connected to a fatal handler component that performs platform-specific responses to unrecoverable errors (such as rebooting the system or entering a safe mode).

The Assert Fatal Adapter component converts framework-level assertions (FW_ASSERT failures) into FATAL events, bridging the C++ assertion mechanism with the event system.
