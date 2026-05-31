
# Rate Group Scheduling Functionality

## References

- [F Prime RateGroupDriver SDD](https://github.com/nasa/fprime/blob/devel/Svc/RateGroupDriver/docs/sdd.md)
- [F Prime ActiveRateGroup SDD](https://github.com/nasa/fprime/blob/devel/Svc/ActiveRateGroup/docs/sdd.md)
- [F Prime PassiveRateGroup SDD](https://github.com/nasa/fprime/blob/devel/Svc/PassiveRateGroup/docs/sdd.md)
- [F Prime ActivePhaser SDD](https://github.com/nasa/fprime/blob/devel/Svc/ActivePhaser/docs/sdd.md)
- [F Prime Rate Group Design Pattern](https://github.com/nasa/fprime/blob/devel/docs/user-manual/design-patterns/rate-group.md)

## Overview

Rate group scheduling provides periodic execution of components at defined rates. A system clock source generates a primary tick, which is divided down to drive multiple rate groups at different frequencies. Each rate group invokes a set of components in a defined order. This is the fundamental execution pattern for cyclic processing in F Prime — telemetry collection, sensor reading, control loops, and other periodic tasks are all driven through rate groups.

The capability is provided by a chain of collaborating components: a cycle source generates the primary tick, a rate group driver divides it into sub-rates, and rate group components dispatch calls to their member components.

### Clock Division

The Rate Group Driver receives a primary system tick and divides it to produce outputs at different rates. For example, a 1 Hz tick can be divided to produce 1 Hz, 0.5 Hz, and 0.25 Hz outputs. The division ratios are specified at construction time. The Rate Group Driver is a passive component that runs in the caller's thread context.

Each output of the Rate Group Driver is typically connected to the input of a rate group component.

### Rate Group Execution

Rate groups invoke their member components in sequence, passing a context value that indicates the call order. Three variants are available:

- **Active Rate Group** — Has its own thread. The input cycle port is asynchronous, placing a message on the queue that wakes the component's thread. The thread then invokes all member output ports in order. This decouples the rate group execution from the clock source.
- **Passive Rate Group** — Executes in the caller's thread. The input cycle port is synchronous, so member components are invoked directly in the context of whoever called the rate group. This is suitable for lightweight processing where the overhead of a separate thread is unnecessary.
- **Active Phaser** — Provides phased scheduling where each cycle is divided into time windows, and individual port calls are assigned to specific windows. See the Phased Scheduling section below.

Both the Active and Passive Rate Groups track the time required to complete each cycle and report it as telemetry.

### Phased Scheduling

The Active Phaser provides a more sophisticated scheduling model where each cycle is divided into a configurable number of ticks, and individual port calls are mapped to specific windows of ticks within the cycle. This allows for precise control over when within a cycle each component is invoked, enabling phased scheduling where different components run at different points in time to spread processing load. The phaser checks for overruns if a port call exceeds its specified time window.

### Overrun Detection

The Active Rate Group detects overruns (also called slips) — when a new cycle begins before the previous cycle has completed. When an overrun is detected, a warning event is issued. This indicates that the system is not meeting its timing requirements and may need attention (either the rate is too fast or the member components are taking too long).

### Configuration

- The number of rate group member output ports is configurable at compile time.
- The division ratios for the Rate Group Driver are specified at construction.
- Context values passed to member components are specified via a configuration table.
- The Active Phaser's tick count and per-port timing windows are configured at setup time.

### Telemetry

Rate groups report the maximum execution time observed for their cycle, allowing operators to monitor timing margins.

The cycle source that drives rate group scheduling is described in the [Time Services](time-services.md) document.
