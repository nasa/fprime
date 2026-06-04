---
name: jpl-design-principles
description: >
  Software-relevant distillation of the JPL Design Principles (DP10,
  External Release V4). Use as a developer checklist when writing or
  modifying F Prime flight software, and as a reviewer reference for
  flagging JPL Design Principle violations in PRs.
  Keywords: JPL, design principles, flight software, margins, fault
  protection, testability, keep-it-simple, KISS, robustness.
---

# Skill: JPL Design Principles — Software Distillation

Source: **JPL Design Principles** — "Design, Verification/Validation and
Operations Principles for Flight Systems" (DP10, External Release V4).
Numbering below mirrors the original document sections so every rule is
traceable.

---

## Section 2 — Principles (software-relevant subset)

### P-2.1.1 Enable Anomaly Investigations

Design and operate the mission so that future anomaly investigations
have the data needed to prevent recurrence — even if the mission is lost.

### P-2.2.1 Mission Robustness

Protect threshold mission in the presence of flight system faults,
mission system faults, and operator errors. Stored critical data shall
be protected from loss due to credible fault scenarios.

### P-2.2.3 Fault Protection Expectations

Fault protection preserves flight system health, safety, and
consumables throughout all mission phases, except when completion of
time-critical events takes priority.

### P-2.2.5 Escapes

Take actions during I&T and operations to detect errors and reduce the
chance of escapes.

### P-2.2.6 Test As You Fly

Test systems in conditions that match or exceed flight conditions; fly
systems within the bounds already tested.

### P-2.3.1 Design for Ops

Develop the project and flight system keeping operational impacts of
design choices in mind. Encourage approaches that reduce operational
complexity and interdependencies (fewer calibrations, more on-board
autonomy, robust margins). Critical flight software parameter updates
shall have the same rigor as a full FSW update.

### P-2.3.2 Unambiguous

Design flight and ground systems to set and know the state of the
flight system unambiguously.

### P-2.3.3 Fault Avoidance

Take design action to avoid building-in known vulnerabilities.

### P-2.3.6 Graceful Degradation

Build design robustness through graceful degradation following a
failure — incremental loss of capability such that a reduced set of
mission requirements is still met.

### P-2.3.7 Fault Recovery

Full mission capability is retained or recovered following faults or
operator errors.

### P-2.4.1 Technical Resource Margins for Operations

Preserve a portion of technical resource margins (power, memory,
throughput, bus bandwidth, delta-V) at launch to solve problems after
launch.

### P-2.4.2 Reliability through Design Margins

Margins are established consistent with design maturity and mission
environments, accommodate "unknown unknowns," and enable changes with
minimal ripple effects.

### P-2.5.1 Margins for Development

Manage development risk through margin establishment. Margins may
follow a glide slope (e.g., mass/power decrease over lifecycle) or be
set once (e.g., pointing/stability).

### P-2.5.2 Avoid Local Optimization

Constrain the number and type of interfaces to avoid duplication of
design and testing effort at the system level.

### P-2.6.1 Follow Identified Standards

Follow identified standards or practices (possibly with JPL
exceptions).

### P-2.6.2 Follow Software Best Practices

Follow software best practices as mapped to Design Rules in Section
4.11.

---

## Section 4 — Flight System Design Rules (software-relevant subset)

### DR-4.1.3.1 Single Failure Tolerance

No single failure of any element shall result in failure to meet
threshold mission requirements. All single-point failures must be
identified.

### DR-4.1.3.2 Protection Against Operator Errors

The design shall protect against ground operator errors that could
result in loss of mission or significant impact.

### DR-4.4.4.4 Assured Commanding

Commanding functions shall be robust to power outages and degraded
communications (e.g., hardware command decoding, non-volatile memory).

### DR-4.4.4.6 Commanding Reliability

Provide protection against all incorrect commands and against correct
but untimely commands that risk health/safety, initiate irreversible
state changes, threaten consumables, or jeopardize commandability.

### DR-4.4.4.8 Onboard Command Processing Approach

Avoid direct memory location manipulation for command processing. Use
software functions to translate commands into actions.

### DR-4.4.4.9 Two Independent Commands for Hazardous Events

Initiation of hazardous or mission-critical in-flight events shall
require at least two independent commands or actions.

### DR-4.4.6.1 Visibility of S/C Status

Provide telemetry data to assess spacecraft status under normal,
stressed, and faulted operations — including health, anomaly
determination, and state visibility.

### DR-4.4.6.4 Visibility of Spacecraft State

Permit ground operators to rapidly and unambiguously determine
spacecraft state, particularly fault protection responses, during the
first part of a tracking pass.

### DR-4.4.6.8 Visibility to Reconstruct Faults

Provide visibility to support post-mortem analysis of activities with
potential for mission-catastrophic outcome.

### DR-4.4.7.1 Clock and Counter Rollover

Spacecraft shall continue to operate in the event of rollover of
spacecraft clocks and/or incremental counters (hardware or software).

### DR-4.9.1.2 Protection for Credible Single Faults

Fault protection shall handle all credible single faults within all
expected environmental conditions.

### DR-4.9.1.5 Tolerance to False Alarms

The spacecraft shall tolerate execution of fault protection in response
to false alarms at all times throughout the mission.

### DR-4.9.1.6 Variation in FP Behavior

Variations in fault protection behavior shall be based directly on
system mode or activity, not on individual enable/threshold
manipulation.

### DR-4.9.1.7 Speed of Fault Detection and Response

Fault protection shall respond in a timely manner to identified threats
to health, safety, and/or mission success.

### DR-4.9.2.2 Flight System Safing

Following fault conditions, fault protection shall autonomously
configure the spacecraft to a safe, sustainable, ground-commandable
mode preserving vital resources and providing at least an RF carrier
downlink.

### DR-4.9.2.3 Autonomous Completion

For events that must execute without ground intervention, fault
protection shall endeavor to ensure autonomous, timely completion.

### DR-4.9.3.1 In-Flight Commandability

Fault protection design shall permit ground operators to easily modify
control settings (enables, thresholds, persistence) in flight.

---

## Section 4.11 — Flight Software System Design (complete)

### DR-4.11.1.1.1 Documentation of Architecture Design

Document architectural elements, external interfaces, interfaces
between elements, element responsibilities, and interaction
constraints.

### DR-4.11.1.1.2 Architectural Analysis of Quality Attributes

Architecture documentation shall include multiple views and support
evaluation of key quality attributes (performance, availability,
maintainability, modifiability, security, testability, usability).

### DR-4.11.1.3 Semantics of Data Interfaces

Semantics of data across public interfaces shall be clearly specified
and, if possible, verified automatically at build time (range,
precision, units, coordinate frames).

### DR-4.11.1.4 Compatibility with COTS Tools

Flight software shall operate on commercial platforms with little
change, so tests on those platforms are substantially relevant to V&V.

### DR-4.11.1.5 Demonstrable Correctness Properties

Software should have demonstrable correctness properties supported by
appropriate static analysis techniques.

### DR-4.11.2.1 Nominal Initialization

Flight software shall initialize software and hardware to a known,
safe, and deliberate state.

### DR-4.11.2.2 Multiple Restart Initialization

Software shall detect off-nominal restarts and successively
reinitialize with less dependency on preserved state until a fully
known configuration and stable operation are restored.

### DR-4.11.2.3 Minimalist Boot

Boot implementation shall include a minimalist configuration requiring
minimum on-board resources for vehicle safety and ground intervention.

### DR-4.11.2.4 System Initialization Trace Telemetry

Flight software shall record and transmit its progress through each
initialization attempt.

### DR-4.11.3.1 Use of Standards

Flight software shall employ applicable JPL standard products,
formats, interfaces, and processes.

### DR-4.11.3.3 Parameter and Argument Specification

Parameters and arguments shall be specified in terms of their
attributes. Physical units and reference frames shall be specified and
checked for consistency (automatically where possible).

### DR-4.11.4.2 Response to Incorrect Commands/Data/Memory

Flight software shall detect and respond safely to: (a) corrupted
commands, data, loads, and memory faults; (b) incorrectly formatted
data including invalid or out-of-range parameters; (c) commands/data
invalid in the current context.

### DR-4.11.4.3 Protection from Unintended Software Modification

Flight software modifiable during flight shall be protected from
unintended modifications (operations errors, SEEs, hardware problems).

### DR-4.11.4.5 Response to Resource Over-Subscription

Software shall contain a robust response when computer resources are
oversubscribed (buffer overflow, rate-group overrun, excessive
interrupts).

### DR-4.11.4.6 Response to I/O Anomalies

Software shall tolerate and continue functioning when periodic inputs
are missing or I/O fails/completes unsuccessfully/is invalid.

### DR-4.11.4.7 Use of Time-Outs

Software shall detect and respond appropriately to failures to
complete required activities on time (e.g., watchdog timers).

### DR-4.11.4.11 Protection Against Incorrect Memory Use

Protect against execution in data areas, using code as data, and
unintended overwriting of code areas.

### DR-4.11.4.12 Data Set Consistency

Ensure data sets and parameter lists are consistent when passed among
threads — no danger of using a mixture of old and new data.

### DR-4.11.4.13 Thread-Safe Operations

Software shall be demonstrated free of deadlocks, failures to make
progress, race conditions, and other threats to multi-threaded
operations.

### DR-4.11.4.14 Software Managed State Transitions

Avoid uncertain, unsafe, or hazardous consequential states during
software-managed state transitions. Account for all transition steps
and their timing; detect incomplete/interrupted transitions and
reinforce a known configuration.

### DR-4.11.5.2 Design for Incremental Verification

Enable easy software testing at unit, module, subsystem-testbed, and
system-testbed levels including regression testing.

### DR-4.11.5.4 Stress Testing

Software shall degrade with understandable behavior when stressed
beyond performance limitations (singularities, capability exceedance,
identified uncertainties).

### DR-4.11.6.1 Self-Test Capability

Include capabilities to test operation and permit timely fault
diagnostics. If not removed for flight, test capabilities shall not
damage hardware or interfere with proper operation.

### DR-4.11.6.2 Fault Diagnostics

Incorporate test/diagnostic code early (by PDR design, by CDR code),
accessible through flight interfaces for rapid problem resolution.

### DR-4.11.6.3 Measurement of Constrained Resources

Provide easy and timely visibility into computing resource usage
(throughput, memory, bus utilization, stack headroom, cycle slip
statistics, fragmentation, memory leaks, allocation latency).

---

## Section 6.3.5 — Flight Software Margins

### DR-6.3.5.1 Margin Definitions

Margin = Capability − CBE. % Margin = 100% × (Margin / Capability).

### DR-6.3.5.3 Margins at Key Milestones

| Milestone | Computer Selection | PDR | CDR | Launch |
|---|---|---|---|---|
| % Margin | 75% | 60% | (no spec) | 20% |

Applies to constrained resources: computing capacity, memory,
throughput, bus bandwidth, etc.

---

## Section 8 — V&V (software-relevant subset)

### DR-8.2.1 Subsystem Pre-Delivery Verification

All flight hardware and software shall be functionally verified to
conform to all allocated requirements prior to delivery to ATLO.

### DR-8.3.2.2 Software Regression Testing

New flight software versions delivered to ATLO shall undergo
regression testing on the flight vehicle prior to use in system-level
verification.

### DR-8.3.3.5 Phasing Test

Phasing tests in final flight configuration shall verify proper
operation of polarity-sensitive functions, including software.

---

## Section 9 — Flight Operations (software-relevant subset)

### DR-9.2.2 Post-Launch FSW Update Rigor

Post-launch FSW updates shall have the same rigor (review, testing,
safeguards, procedures) as pre-launch development, including
functional testing on a testbed.

### DR-9.2.3 In-Flight Parameter Update

Critical on-board parameter updates shall have the same rigor as
pre-launch parameter selection, including testbed functional testing.

### DR-9.5.1 Operating Margins for Real-Time Operations

Plan, maintain, and monitor operating margins for system resources
(power, thermal, comms links, computer memory, timing, throughput,
bus bandwidth, FP thresholds/persistences) in all flight activities.
