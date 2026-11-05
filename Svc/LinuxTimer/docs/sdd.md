# Svc::LinuxTimer

## 1. Introduction

The LinuxTimer component provides a periodic tick source for Linux and Darwin systems. It implements the [`Drv.Tick`](../../../Drv/Interfaces/Tick.fpp) interface, invoking its `CycleOut` output port at a fixed interval. It is typically connected to a [`Svc::RateGroupDriver`](../../RateGroupDriver/docs/sdd.md) to drive the rate groups of a deployment, serving the same role as a hardware timer interrupt does on embedded platforms.

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-LINUXTIMER-001 | The LinuxTimer component shall implement the Drv.Tick interface | inspection |
| SVC-LINUXTIMER-002 | The LinuxTimer component shall invoke its `CycleOut` port at a caller-specified fixed interval | unit test |
| SVC-LINUXTIMER-003 | The LinuxTimer component shall provide a timestamp of the tick with each `CycleOut` invocation | inspection |
| SVC-LINUXTIMER-004 | The LinuxTimer component shall stop ticking when `quit()` is called | unit test |

## 3. Design

The LinuxTimer is a passive component with no thread of its own. The caller's thread — typically the deployment's main thread after topology startup — is donated to the timer by calling `startTimer()`, which blocks in the timer loop until `quit()` is called from another thread.

### 3.1 Port Description

| Port | Kind | Type | Description |
|---|---|---|---|
| `CycleOut` | output | `Svc.Cycle` | Periodic tick output; meant to be connected to a rate group driver |

The component has no commands, events, telemetry, or parameters.

### 3.2 Implementations

Two implementations are provided; the build selects one based on the target platform (see `CMakeLists.txt`):

* **timerfd (Linux)** — `LinuxTimerFd.cpp` creates a `timerfd` on `CLOCK_MONOTONIC` set to the requested interval and blocks on `read()` for each expiration. This provides drift-free periodic ticks, since the kernel maintains the expiration schedule.
* **Task delay (Darwin)** — `LinuxTimerTaskDelay.cpp` sleeps for the interval between ticks using a task delay. This is subject to accumulated drift and is intended for development hosts without `timerfd` support.

On each tick, the implementation captures a raw timestamp (`Os::RawTime`) and invokes `CycleOut`. The loop checks the mutex-protected quit flag each iteration and returns when it is set.

## 4. Usage

1. **Instantiate** the component in the topology and connect `CycleOut` to the rate group driver's cycle input.
2. **Start the timer** from deployment startup code, donating the calling thread: `linuxTimer.startTimer(Fw::TimeInterval(0, 100000))` for a 100 ms (10 Hz) tick. This call blocks.
3. **Shut down** by calling `linuxTimer.quit()` from an exit handler; `startTimer` then returns and the deployment can tear down.

## 5. Change Log

| Date | Description |
|---|---|
| 2026-08-10 | Initial SDD |
