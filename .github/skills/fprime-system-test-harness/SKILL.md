---
name: fprime-system-test-harness
description: >-
  Use when building the harness for an approved F Prime system test
  plan: tester components, integration tests, bench scripts, record
  markers, fault-injection knobs, and the builder's checker. Keeps the
  harness small, reusable, and reviewable, and keeps it out of the unit
  under test. Keywords: F Prime, test harness, tester component,
  integration test, NO_OP_STRING, fault injection, fprime-yamcs, record.
---

# Skill: F Prime system test harness

Turn an approved plan into runnable tests whose output lands in an
auditable record. Governing rules are in
`.github/agents/_shared/test-process-contract.md`; code is written via
`fprime-development.agent.md` and must satisfy `fprime-cpp-design`.

---

## 1. Reuse before writing

In this order:

1. Existing commands, events, and telemetry of the UUT and the
   deployment — most tests need no new flight code.
2. The UUT's reusable integration tests (`test/int/`,
   `fprime-component-integration-test`) as building blocks.
3. Standard components (`Svc/*`, `Drv/*`) for stimulus and
   observation.
4. A tester component in the **test** tree only when stimulus or
   observation cannot be reached from the ground (e.g. driving a port
   that has no command path). It follows the component development
   skills and ships with its own unit tests.

A tester component is harness, not UUT. It is never wired into the
mission topology unless the human has chosen a dedicated test
deployment.

## 2. Deployment

Default is the project's full system deployment on the complete
testbed. When the human has directed a reduced or sibling deployment
(`fprime-topology-development`), keep it a copy of the system
deployment minus what the human excluded, in its own directory, and
leave the system deployment's diff empty.

## 3. Record and markers

Every test is bracketed in the record by command markers so the
auditor can slice it without timestamps (F Prime generation time may
be uptime-based):

```
cmdDisp.NO_OP_STRING "QUAL <run> <id> START"
… test procedure …
cmdDisp.NO_OP_STRING "QUAL <run> <id> END"
```

END carries **no verdict** (contract §4). At run start the harness
emits `NO_OP_STRING "RFR <run> MANIFEST <sha8> SEALED <sha8>"` so the
record proves which manifest and draw ran.

Record requirements the harness must make possible:

- Ground system with a persistent archive (fprime-yamcs preferred; GDS
  logs acceptable), one instance and data directory per run ID.
- Completeness check per window: commands sent by the harness equal
  the dispatcher's completed count delta, and every expected
  completion event is present. The checker marks a window whose check
  fails `UNVERIFIED`, not `FAIL`.
- Downlink budget: estimate event rate against link capacity
  beforehand; a harness that loses events cannot prove an absence.

## 4. Fault knobs

Implement every knob on the approved menu as harness **configuration**
(`sealed/faults.json`, read by the harness at start). A knob may change
a seed, an expected constant, a length, an opcode in a tester
component — never the test ID, the log text, or the marker. Verify in
the dry run that each knob, applied alone, makes exactly its target
test fail and nothing else.

## 5. Checker

Write the builder's checker per `fprime-test-oracle`. It reads the
exported record and emits `verdicts.json`. Do not share it with the
reviewer, who writes an independent one.

## 6. Size and review

Target: a human reads the harness in one sitting. Put pass rules and
knob handling in one small file each; isolate bench scripts (power
control, flashing, ground-system start) from test logic. Harness code
is reviewed by the system-test reviewer and, if the human requests,
the local multi-agent review (`fprime-iterative-development` §2.4).
Document the harness in a short README: how to run one test, the whole
plan, and the dry run.

## 7. Dry run

Stage 2 closes with an unsealed run of the full plan on the real
bench, every knob exercised once, both checkers agreeing on every
verdict. Dry-run output is labelled and is not a record.

## Anti-patterns

- Fixing the UUT from the harness branch.
- Tests that pass by construction (assert on command return only, count
  without payload, no completion event awaited).
- Verdicts printed by the harness instead of computed by a checker
  from the record.
- Knobs that are visible in the record.
