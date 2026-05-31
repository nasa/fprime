
# Health Monitoring Functionality

## References

- [F Prime Health SDD](https://github.com/nasa/fprime/blob/devel/Svc/Health/docs/sdd.md)
- [F Prime Ping Port SDD](https://github.com/nasa/fprime/blob/devel/Svc/Ping/docs/sdd.md)
- [F Prime WatchDog Port SDD](https://github.com/nasa/fprime/blob/devel/Svc/WatchDog/docs/sdd.md)

## Overview

Health monitoring verifies that active components in the system are responsive and have not hung or stalled. The Health component periodically pings each monitored component and checks for a response within a configurable timeout. If a component fails to respond within a warning threshold, a WARNING event is issued. If the component remains unresponsive past a second (fatal) threshold, a FATAL event is issued, indicating a critical software health failure. An optional hardware watchdog stroke ensures the processor resets if the health monitoring task itself becomes unresponsive.

### Ping Mechanism

The Health component uses a ping/response pattern to verify liveness:

1. On each monitoring cycle, the Health component sends a ping with a unique key value to each monitored component via the Ping port.
2. Each monitored component echoes the key back through its ping response port.
3. The Health component tracks how many cycles have elapsed since each component last responded.
4. If a component exceeds its configured timeout (measured in monitoring cycles), a WARNING event is issued. If the component continues to be unresponsive past a second threshold, a FATAL event is triggered.

The ping table is configured at startup with entries specifying each monitored component and its warning and fatal timeout thresholds.

### Watchdog Stroking

The Health component optionally strokes a hardware watchdog timer on each monitoring cycle. If the Health component's own task hangs or the entire software system becomes unresponsive, the watchdog will expire and trigger a hardware reset. This provides a last-resort recovery mechanism independent of software health.

### Commands

Health monitoring supports the following ground commands:

- **Enable/Disable** — Globally enable or disable all health monitoring
- **Per-Component Enable/Disable** — Enable or disable monitoring for a specific component by name
- **Ping Timeout Update** — Update the warning and fatal timeout thresholds for a specific component

### Telemetry

The Health component reports a count of ping timeouts as telemetry, providing visibility into system health trends.

### Off Nominal

- If a monitored component is unresponsive beyond its warning threshold, a WARNING_HI event is logged.
- If the component remains unresponsive past its fatal threshold, a FATAL event is issued, typically triggering the system's fatal handling path.
- If health monitoring is disabled (via command), no pings are sent and no timeouts are tracked. This is intended for use during specific operational phases where normal monitoring would interfere.
