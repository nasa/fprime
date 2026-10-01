# Svc::PassiveConsoleTextLogger Component

## 1. Introduction

The `Svc::PassiveConsoleTextLogger` is a passive component that prints the text version of events to stdout. It supports per-severity filtering and event ID filtering.

## 2. Requirements

Requirement | Description | Verification Method
----------- | ----------- | -------------------
ISF-PTL-001 | The `Svc::PassiveConsoleTextLogger` component shall print received log texts to standard output. | Inspection
ISF-PTL-002 | The `Svc::PassiveConsoleTextLogger` component shall support per-severity event filtering via a public `setSeverityFilter()` method. | Unit Test
ISF-PTL-003 | The `Svc::PassiveConsoleTextLogger` component shall never filter FATAL events regardless of filter configuration. | Unit Test

## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Svc::PassiveConsoleTextLogger` component has one synchronous input port for receiving text events. All processing occurs in the caller's thread.

#### 3.1.2 Ports

Port Data Type | Name | Direction | Kind | Usage
-------------- | ---- | --------- | ---- | -----
[`Fw::LogText`](../../../Fw/Log/docs/sdd.md) | TextLogger | Input | Synchronous | Logging port

### 3.2 Functional Description

The `Svc::PassiveConsoleTextLogger` prints formatted event text to stdout via `Fw::Logger::log`.

#### 3.2.1 Severity Filtering

The component supports per-severity event filtering via the `setSeverityFilter(Fw::LogSeverity, bool)` method. Each of the six filterable severity levels (WARNING_HI, WARNING_LO, COMMAND, ACTIVITY_HI, ACTIVITY_LO, DIAGNOSTIC) can be independently enabled or disabled. When a severity is disabled, events at that level are silently dropped. FATAL events are never filtered regardless of configuration.

Default filter states are configured via constants in `PassiveTextLoggerCfg.hpp`. By default, all severities pass through except DIAGNOSTIC, which is filtered out.

The filtering logic is provided by the shared `Svc::EventSeverityFilter` utility class.

## 4. Dictionaries

Not applicable

## 5. Module Checklists

## 6. Unit Testing

To see unit test coverage run fprime-util check --coverage

## 7. Change Log

Date | Description
---- | -----------
4/20/2017 | Initial Version
2026 | Added severity filtering documentation, requirements, and unit tests



