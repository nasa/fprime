---
name: fprime-component-checklist-review
description: >-
  Fill in the F Prime Component Development Checklist (v1.2.0) for a
  component from automated evidence plus a short LLM review of the
  items no tool can prove. Maps every checklist item to its CodeQL
  query, component-checks script, review rule, or human step; defines
  the review rules CL-1..CL-8 for the residual items. Use when asked to
  run, fill, or audit the component checklist, or to review a
  component for flight readiness. Keywords: F Prime, checklist,
  component, flight software, CodeQL, component-checks, close-out,
  ISR, units, boundary tests.
---

# F Prime component checklist review

The checklist has 72 items in five sections. All but one are decided by
tools; this skill tells you where each verdict comes from and defines the
few judgements that remain for the reviewer.

## Evidence sources

| Source | Where | Produces |
| --- | --- | --- |
| CodeQL (`fprime/jpl-c-queries`) | `.github/codeql/fprime-queries/*.ql`, run by `codeql-jpl-standard.yml` | SARIF alerts per rule id `cpp/fprime/...` |
| component-checks | `nasa/fprime-actions/component-checks`, run by `component-checks.yml` | `<module>/checks/summary.json` with ids R1..R9, D1..D10, I1..I4, U1..U5, C1..C3 |
| This skill | rules CL-1..CL-8 below | reviewer findings |

A checklist item is **YES** when every mapped rule/check passes for the
module, **NO** when any fails (cite the alert or check line in Notes),
**N/A** only when the mapped check reports `SKIP` with a reason or the
component has none of the construct (e.g. no ISR, no floats).

## Item map

Legend: Q = CodeQL rule id (`cpp/fprime/` prefix omitted), S = component-checks id,
L = review rule in this skill, H = human.

### 1 Requirements
| Item | Mechanism |
| --- | --- |
| 1.1 behaviours | S R9 (`Kind` column covers Behavior) + R1 (commands) + L CL-7 |
| 1.2 inputs | S R9 (Input) + R6 (ports) + R4 (parameters) |
| 1.3 outputs | S R9 (Output) + R2 (telemetry) + R3 (events) + R5 (data products) |
| 1.4 memory | S R9 (Memory, or waived in SDD) |
| 1.5 hardware I/O | S R9 (Hardware I/O, or waived) |
| 1.6 performance | S R9 (Performance, or waived) |
| 1.7 verification method | S R8 |
| 1.8 external ICD/FDD | S D9 (external-interfaces section or explicit `None`) |

### 2 Design
| Item | Mechanism |
| --- | --- |
| 2.1 high-level design | S `check_sdd.py` grade >= silver + L CL-8 |
| 2.2 block diagram | S D9 |
| 2.3 port table vs model | S D7 |
| 2.4 input-port behaviour | S D10 + L CL-8 |
| 2.5 port handlers | S D1 |
| 2.6 command handlers | S D2 |
| 2.7 sequence diagrams | S D9 |
| 2.8 state machines / use cases | S D9 |
| 2.9 state-machine actions | S D3 |
| 2.10 cmd/tlm/evr/prm tables | S D8 |
| 2.11 data-product tables | S D8 |
| 2.12 DP default priority | S D4 |
| 2.13 every port has a use case | S R6 + D10 |
| 2.14 port directions/types | S D7 |
| 2.15 component kind | S D9 |
| 2.16 requirements mapped to design | S D5 |
| 2.17 FPP compiles | S D6 |

### 3 Implementation
| Item | Mechanism |
| --- | --- |
| 3.1 CMake dependencies | S I1 |
| 3.2 no warnings | S I2 |
| 3.3 loops bounded | Q `jpl-c/loop-bounds` |
| 3.4 cyclic messages gated | Q `unthrottled-cyclic-event` |
| 3.5 switch default/break | Q `switch-default`, `switch-case-fallthrough` |
| 3.6 fallthrough annotated | Q `switch-case-fallthrough` |
| 3.7 variables local | Q `global-mutable-variable` |
| 3.8 access specifiers | Q `public-data-member` |
| 3.9 float equality | Q `float-equality` |
| 3.10 float precision / margins / loops | Q `float-loop-control`, `float-narrowing` + L CL-6 |
| 3.11 initialised before use | Q `uninitialized-member` + stock `cpp/uninitialized-local` |
| 3.12 DP priority configurable | S I3 |
| 3.13 command acknowledged | Q `command-response` |
| 3.14 validation failure returns | Q `validation-failure-return` |
| 3.15 internal port invoke() | Q `internal-port-invoke` |
| 3.16 pointer args asserted | Q `checking-parameter-values` |
| 3.17 state asserted | Q `jpl-c/use-of-assertions-density` |
| 3.18 no assert on HW/user input | Q `assert-on-untrusted-input` |
| 3.19 ISR no float | Q `no-float-in-isr` |
| 3.20 ISR minimal | Q `isr-complexity` |
| 3.21 ISR state before enable | L CL-1 |
| 3.22 ISR non-blocking | Q `no-blocking-in-isr` |
| 3.23 interrupt enables cleared at init | L CL-2 |
| 3.24 rate-group handlers non-blocking | Q `no-blocking-in-sched-handler` |
| 3.25 no file I/O in critical code | Q `no-file-access-in-critical` |
| 3.26 compound statements | Q `compound-statements` |
| 3.27 division guarded | Q `division-guard`, `float-division-guard` |
| 3.28 array bounds | Q `unchecked-array-index`, `checking-parameter-values` |
| 3.29 no nested function decl | Q `no-function-decl-in-function` |
| 3.30 for-header side effects | Q `for-header-side-effects` |
| 3.31 signed overflow | Q `signed-overflow` |
| 3.32 return values checked | Q `jpl-c/checking-return-values` |
| 3.33 units in names | L CL-3 |
| 3.34 named constants | Q `magic-numbers`, `define-constant` |
| 3.35 no printf | Q `no-printf` |
| 3.36 fixed-width types | Q `jpl-c/basic-int-types` |
| 3.37 static analysis | S I4 |
| 3.38 implementation matches reqs/design | L CL-4 |

### 4 Unit testing
| Item | Mechanism |
| --- | --- |
| 4.1 tests published | S U1 |
| 4.2 UT CMake dependencies | S U3 |
| 4.3 boundary tests | L CL-5 |
| 4.4 no leaks | S U4 |
| 4.5 coverage | S U5 |

### 5 Close-out
| Item | Mechanism |
| --- | --- |
| 5.1 no TBD/TODO/printf | Q `incomplete-code`, `no-printf` |
| 5.2 topology builds | S C1 |
| 5.3 runs on hardware | **H** performs the run; S C2 verifies `docs/hardware-run.md` |
| 5.4 SDD final | S D7, D8, D9, `check_sdd.py` |
| 5.5 CI passes | S C3 |

## Review rules (residual LLM items)

Apply each rule only to the construct it names; report `N/A` when the
component has none. Cite file:line for every finding.

- **CL-1 ISR state before enable (3.21).** For every function matched by
  the ISR naming convention (`*_isr`, `*interrupt_handler*`, `*_irq*`) find
  the code that enables its interrupt (register write, `request_irq`,
  `enableInterrupt`, vector install). Every variable the ISR reads or writes
  must be assigned on a path that completes before that enable call.
  Finding: `ISR state uninitialised` (severity error).
- **CL-2 Interrupt enables cleared at init (3.23).** In the component's
  constructor/`init`/`configure`/`preamble`, interrupt-enable bits must be
  explicitly cleared (or interrupts masked) before peripheral configuration,
  not assumed clear from reset. Finding: `Interrupt enable assumed clear`
  (warning).
- **CL-3 Units in names (3.33).** Every variable, member, parameter, FPP
  telemetry channel, and command argument that carries a physical quantity
  (time, distance, angle, rate, temperature, voltage, current, size in bytes
  vs words) must encode the unit in its name (`timeoutMs`, `angleRad`,
  `sizeBytes`) or in an FPP `@` annotation. Finding: `Missing unit in name`
  (recommendation).
- **CL-4 Implementation matches requirements and design (3.38).** For each
  requirement row, locate the handler(s) that satisfy it and confirm the
  behaviour, including each error path the SDD describes. Report any
  requirement with no implementing code, and any handler behaviour the SDD
  does not describe. Finding: `Implementation diverges from SDD` (error).
- **CL-5 Boundary tests (4.3).** For each bounded quantity in the model
  (array sizes, `FW_ASSERT` limits, configuration constants, command-argument
  ranges, queue depths) there must be a unit test exercising the limit value
  and one past it. Finding: `Boundary untested` (warning).
- **CL-6 Float precision justified (3.10).** Each `F32` quantity must have
  sufficient precision for its range (e.g. seconds since epoch in `F32` loses
  seconds after ~190 days). Flag accumulators and time values held in `F32`.
  Finding: `Insufficient float precision` (warning).
- **CL-7 Requirement quality (1.1).** Apply `fprime-requirements-review`
  to the requirements table when it has changed in the PR.
- **CL-8 Design prose adequacy (2.1, 2.4).** The SDD must describe, for each
  input port and command, what the component does, including the rejection
  path. Table rows alone do not satisfy this. Finding: `Behaviour not
  described` (warning).

## Procedure

1. Run `component-checks` for the module (or read its published
   `checks/summary.json`) and the CodeQL SARIF for the branch.
2. Fill sections 1, 2, 4, 5 and the Q/S rows of section 3 from that evidence.
3. Apply CL-1..CL-8; fill the remaining rows.
4. Item 5.3: confirm `docs/hardware-run.md` (checked by C2) names a human
   tester; the run itself is not delegable.
5. Emit the checklist with status (`YES`/`NO`/`N/A`) and Notes per the
   checklist's "Status Legend"; non-`YES` rows must carry the alert or check
   line that justifies them.
