
# CDH Core Subtopology

## References

- [CdhCore Subtopology SDD](https://github.com/nasa/fprime/blob/devel/Svc/Subtopologies/CdhCore/docs/sdd.md)
- [F Prime CommandDispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/CmdDispatcher/docs/sdd.md)
- [F Prime EventManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/EventManager/docs/sdd.md)
- [F Prime Health SDD](https://github.com/nasa/fprime/blob/devel/Svc/Health/docs/sdd.md)
- [F Prime Version SDD](https://github.com/nasa/fprime/blob/devel/Svc/Version/docs/sdd.md)
- [F Prime PassiveTextLogger SDD](https://github.com/nasa/fprime/blob/devel/Svc/PassiveConsoleTextLogger/docs/sdd.md)
- [F Prime AssertFatalAdapter SDD](https://github.com/nasa/fprime/blob/devel/Svc/AssertFatalAdapter/docs/sdd.md)

## Overview

The CdhCore subtopology bundles the core Command and Data Handling (CDH) services that nearly every F Prime deployment requires into a single reusable unit. It provides command dispatching, event handling, health monitoring, version reporting, text logging, and assertion-to-fatal adaptation. By packaging these services as a subtopology, deployments avoid repetitive wiring and get a consistent baseline for their CDH infrastructure.

This subtopology is available as a reusable building block that can be imported into any F Prime topology.

### Included Services

The CdhCore subtopology contains the following component instances:

- **Command Dispatcher** (active) — Routes incoming commands to the appropriate component and reports execution status.
- **Event Manager** (active) — Collects events from all components, packages them for downlink, and announces fatal events.
- **Health** (queued) — Pings active components to verify liveness; configured with a ping table.
- **Version** (passive) — Reports framework, project, and library version information. Configured to report at startup.
- **Text Logger** (passive) — Prints human-readable event text to the console.
- **Assert Fatal Adapter** (passive) — Converts FW_ASSERT failures into FATAL events.

Two additional instances are configurable — the telemetry sender and the fatal event handler are provided by the deployment rather than built into the subtopology, allowing integration engineers to connect project-specific implementations.

### Internal Wiring

Within the subtopology, the Event Manager's fatal announcement port is wired to the configurable fatal handler. All other service connections (command registration, event reporting, health pinging, telemetry) are established by the parent topology when the subtopology is imported.

### Configuration

The subtopology supports configuration of:

- Base IDs for all component instances
- Queue sizes for active and queued components
- Stack sizes and priorities for active component threads
- Health ping table entries (warning and fatal timeout thresholds per component)

The telemetry sender and fatal handler instances are specified by the deployment, allowing these to be project-specific while reusing the standard CDH core.

### Required Inputs

- Rate group connections to drive the Health component's monitoring cycle
- A telemetry sender implementation
- A fatal handler implementation
- Command, event, and health connections from all components in the topology
