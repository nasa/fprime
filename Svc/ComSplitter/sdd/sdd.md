# Svc::ComSplitter

## 1. Introduction

The ComSplitter component duplicates a stream of `Fw::Com` buffers to multiple consumers. Each Com buffer received on its input port is copied and forwarded to every connected output port. A typical use is fanning a single downlink stream out to several sinks, for example a radio path and an on-board logger such as [`Svc::ComLogger`](../../ComLogger/docs/sdd.md).

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-COMSPLITTER-001 | The ComSplitter component shall forward each Com buffer received on `comIn` to every connected `comOut` port | unit test |
| SVC-COMSPLITTER-002 | The ComSplitter component shall pass a copy of the buffer to each output so that consumers cannot affect one another | unit test |

## 3. Design

The ComSplitter is a passive component. The `comIn` handler executes synchronously on the caller's thread: it iterates over the `comOut` output port array (5 ports) and, for each connected port, invokes it with a copy of the incoming `Fw::ComBuffer`. The context value forwarded to consumers is always 0. At least one output port must be connected; the handler asserts otherwise.

### 3.1 Port Description

| Port | Kind | Type | Description |
|---|---|---|---|
| `comIn` | sync input | `Fw.Com` | Com buffer stream to split |
| `comOut` | output (array of 5) | `Fw.Com` | Duplicated Com buffer stream |

The component has no commands, events, telemetry, or parameters.

## 4. Usage

1. **Instantiate** the component in the topology.
2. **Connect** the source Com stream to `comIn`.
3. **Connect** up to 5 consumers to elements of `comOut`; unconnected elements are skipped.

## 5. Change Log

| Date | Description |
|---|---|
| 2026-08-10 | Initial SDD |
