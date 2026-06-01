---
name: jpl-design-principles
description: >
  Software-relevant distillation of JPL D-17868 "Design, Verification/Validation
  and Operations Principles for Flight Systems" (Rev 1, Feb 2001). Use as a
  developer checklist when writing or modifying F Prime flight software, and as
  a reviewer reference for flagging JPL Design Principle violations in PRs.
  Keywords: JPL, design principles, D-17868, flight software, margins, fault
  protection, testability, keep-it-simple, peer review, KISS.
---

# Skill: JPL Design Principles — Software Distillation

Source: **JPL D-17868 Rev 1** — "Design, Verification/Validation and Operations
Principles for Flight Systems." Numbering below mirrors the original document
sections so every rule is traceable.

---

## Section 1 — General Principles (software-relevant subset)

### DP-1.1 Priorities
1. Safety of people shall be paramount.
2. Ordered priorities: **safety, reliability, cost, schedule, performance** — sacrifice performance if necessary.

### DP-1.5 Modeling / Simulation
1. Use models/simulations early and often; make them realistic.
3. Models shall be test-validated.

### DP-1.6 Make Early Design Decisions
1. Maintain a "top-ten" list of required design decisions.
   *"Better is the enemy of good."*

### DP-1.7 Design to Requirements / Capability
2. Consider existing capability and cost-effective reuse of inherited designs (HW, SW, SE).
3. Use COTS where feasible and cost/risk-reducing.
5. Consider new technology only when needed to meet priorities or preserve margins.

### DP-1.8 Standards
1. Use industry / JPL standards (HW & SW) — e.g. CCSDS. Document deviations on the risk list.

### DP-1.9 Risk-Based Trade-Offs and Margin
2. HW/SW trades shall be performed early using risk as a metric.
3. Establish margin requirements (mass, power, budget, schedule, **memory, throughput**) early.

### DP-1.10 Single Failure Tolerance
1. No credible single failure shall cause loss of mission.
7. Single-string design allowed only if risk is demonstrably acceptable.

### DP-1.12 Design Fallback Options
1. Identify descope / fallback options early.
2. Define trigger-events/dates in advance.

### DP-1.13 Safety & Mission Assurance
2. Assurance engineering shall be integrated and concurrent with design throughout the lifecycle.

### DP-1.14 Design Margins
1. Margins shall accommodate uncertainties and "don't-know-don't-knows."
2. Robust enough to enable changes with minimal ripple effects.

### DP-1.17 Lessons Learned
1. Review the design against JPL/NASA Lessons Learned and Alerts early and at lifecycle checkpoints.

### DP-1.19 Closed-Loop Failure Reporting
1. Use the electronic PFR system; forward Red Flag PFRs to the flight ops team.

### DP-1.20 Peer Reviews
1. Use independent peer reviews prior to design reviews.
12. Apply peer review to requirements, designs, code, test plans, test results, and documentation.

### DP-1.21 Testability
2. Enable software testing at unit, module, subsystem-testbed, and system-testbed levels.
3. Include self-test and built-in-test routines for diagnostics.
4. Self-test routines shall be removable (or harmless) for flight.
5. Enable "early and often" testing.

### DP-1.23 Test Beds
1. Identify SW-only and HW/SW test beds early in the development plan.
2. Maintain test-bed fidelity; document differences from flight.

### DP-1.26 Design Verification
2. "Test as you fly, fly as you test."
4. Perform stress testing beyond normal verification to determine capability boundaries.
8. Testing shall be the primary verification method; use analysis/simulation only when testing is impractical.

### DP-1.33 Keep-It-Simple
1. Employ "keep-it-simple" (straightforward designs) to reduce risk/cost.
2. Avoid complex implementations; justify added complexity as essential.

### DP-1.34 HW/SW System Design & Verification
2. System requirements traceable to project-level; HW & SW requirements traceable to system requirements.
4. Minimize the number of interface types in flight software.
5. Incorporate test/diagnostic code early for rapid problem resolution.
6. Address fault cases early; fault-protection SW shall handle all credible single-fault scenarios.
7. Processing resources shall exceed estimated requirements by **at least 4x** at computer selection.

---

## Section 2 — Detailed Principles (software-relevant subset)

### DP-2.2 Flight Software Margins
1. Resource margins at milestones:
   - Computer selection: **total capability >= 400% of CBE**
   - Phase C/D start: **>= 60% margin**
   - Launch: **>= 20% margin**
2. Design SW to support measurement of throughput and memory.
3. Track CBEs continuously; review at PDR, CDR, ATLO start, launch.
5. SW shall handle nominal inputs *and* transient off-nominal inputs.

### DP-2.4 Fault Protection / Commandability
1. Fault-protection system shall be in-flight-commandable (enable/disable, thresholds, persistence).

### DP-2.5 Fault Recovery State
1. After a cruise-phase fault: autonomously configure to safe, quiescent, ground-commandable state.
2. During critical events: autonomously re-establish needed functionality.

### DP-2.18 Graceful Degradation
1. Design for graceful degradation under partial failure.

---

## Section 3 — Flight Operations Principles (software-relevant subset)

### DP-3.1 Operability
2. Reduce operational complexity (fewer calibrations, more on-board autonomy, robust margins).

### DP-3.2 Flight Sequences
1. Operate within ground-tested environments/regimes.
2. All sequences tested on a high-fidelity test bed; anomalies dispositioned before uplink.
9. Verify SW loads/updates via memory readout or checksum.

### DP-3.4 Test Bed Fidelity
1. Post-launch, keep the ground test bed configuration close to spacecraft state (especially flight SW).

### DP-3.6 Operating Margins
1. Maintain adequate margins (memory, timing, power) for all sequence-controlled and real-time activities.

---

## Appendix A — Software Principles (complete)

### DP-A.3.1 System Definition / System Engineering
1. All requirements organized, documented, traceable to higher-level requirements.
2. Requirements shall be prioritized.
3. All interfaces defined and documented.
4. Validate functional requirements against a concept of operations early.
7. Document the mission software architecture before PIP approval.
8. Software design shall be **simple and modular**: strong cohesion, minimal coupling, encapsulation, ease of testing.
9. Architecture shall accommodate probable requirements change.
11. Formal review of software requirements prior to implementation.

### DP-A.3.2 Planning & Monitoring
3. Use an incremental or iterative approach.
7. Concurrent HW/SW development shall be jointly planned; integrated peer reviews.
12. Peer reviews on requirements, designs, code, test plans, test results, documentation.
13. Track progress and quality with metrics (effort, progress, quality, growth, processor capability).
14. Document, disposition, and track all anomalies, change requests, and liens.

### DP-A.3.4 Software Risk Management
1. Prepare a software risk management plan with risk list, trigger events, descope options.
3. Validate interfaces, high-risk algorithms, and COTS early; develop in risk order.

### DP-A.3.5 Organization & Staffing
4. Establish a software system architect role with authority for structure, function, and implementation philosophy.
5. Document the architecture before staffing up implementation.

### DP-A.3.6 Design & Implementation
3. Two-way trace: system requirements down, software requirements up.
4. Logic design based on control-flow/state-transition diagrams, failure analysis, off-nominal HW behavior.
5. Explore performance issues in design reviews (queue depth, array sizes, task starvation).
6. Parameter values: specify nominal + allowable range; document derivation.
7. Validate every parameter against its allowable range; provide fault correction/recovery on violation.
9. Establish margins early for CPU, memory, cycle rates, bandwidth; track continuously.
11. Implement core architectural elements early.
12. Use institutionally supported development tools.
14. Establish and monitor documented design rules and coding standards.

### DP-A.3.7 Integration & Test
1. Enable testing at unit, module, subsystem-testbed, system-testbed levels.
2. Focus testing on critical functional areas; monitor coverage.
4. Comply with I&T requirements; maintain regression suite; use automated testing.
5. Use independent testers for mission-critical software.
7. Guide test planning by FMEA/FTA considering SW failure modes and off-nominal HW.
9. Design tests assuming the software contains serious errors.
10. Regression test systematically after every change.
14. Unit testing required for build inclusion; test full operational parameter ranges.

### DP-A.3.8 Configuration Management
1. Apply CM to code, build scripts, tools, test products, and documentation.
3. Audit delivery builds for correct module versions and proper isolation of test SW.

### DP-A.4 Flight Software
5. Accommodate nominal and transient off-nominal inputs.
6. Use standards (e.g. CCSDS) for ground and subsystem interfaces.
7. All flight software shall be readily modifiable during flight.
9. Incorporate fault/failure/anomaly recovery early; handle all credible single faults.
10. Avoid single-point failures that incapacitate all redundant strings.
11. Accommodate processor resets during mission-critical events.
14. Design for testability and operability; include self-test/diagnostic code early.
18. "Fly as you test, test as you fly" — use the flight version for all critical testing.
20. When using models in lieu of test, validate model fidelity and bound uncertainty.
