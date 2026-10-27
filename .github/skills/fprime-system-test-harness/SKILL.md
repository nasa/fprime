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
be uptime-based). `NO_OP_STRING` and `NoOpStringReceived` are 40-char
strings (`Svc/CmdDispatcher/CmdDispatcher.fpp`), so `<run>` and `<id>`
are each ≤ 8 characters:

```
cmdDisp.NO_OP_STRING "RFR <run> M <manifest sha8>"     # once, at run start
cmdDisp.NO_OP_STRING "RFR <run> S <seal-id sha8>"      # once, at run start
cmdDisp.NO_OP_STRING "QUAL <run> <id> START"
… test procedure …
cmdDisp.NO_OP_STRING "QUAL <run> <id> END"             # always sent, in a finally block
```

END carries **no verdict** (contract §4). The L0 feasibility gate
sends one marker and checks the archive shows the text byte-exact —
a project that shortens `FW_LOG_STRING_MAX_SIZE` truncates it, and
nothing is sliceable until the deployment decision is revisited.

The procedure **drives and records; it never decides**. Reused
`test/int/` tests assert inline, so run them under a wrapper that
catches `AssertionError`, continues to the next step, discards the
pytest verdict, and sends END in `finally`. Harness stdout and pytest
output go to `runner/`, never into `record/`.

Record requirements the harness must make possible:

- Ground system with a persistent archive (fprime-yamcs preferred; GDS
  logs acceptable), one instance and data directory per run ID, and an
  exporter that pages the whole archive and writes
  `export-counts.json` (`fprime-test-oracle` §2).
- Completeness per window from infrastructure facts only: both
  markers; `commands_sent` (markers included) equals
  `delta(cmdDisp.CommandsDispatched)`; `CommandErrors`,
  `CommandsDropped` and `eventLogger.EventsDropped` unchanged unless
  the test expects a rejection. UUT completion events are pass-rule
  facts, not completeness facts.
- Pacing: wait for each command's completion before sending the next;
  never more commands in flight than the dispatcher queue depth — a
  flooded dispatcher asserts, and that is a harness defect, not a UUT
  one.
- Downlink budget: estimate event rate against link capacity
  beforehand; a harness that loses events cannot prove an absence.

## 4. Fault knobs

Implement every knob on the approved menu as harness **configuration**
(`rfr/<run-id>/sealed/faults.json`, read by the harness at start). A
knob may change a seed, an expected constant, a length, a stimulus, or
disturb device state through the harness — never the test ID, the log
text, or the marker. The harness **never logs, prints, asserts on, or
writes which knobs are active**, to any file or stream, including its
own stdout in `runner/`; the only trace of `faults.json` outside
`sealed/` is the `S` marker. Verify in the dry run that each knob,
applied alone, makes its target test fail with exactly the predicates
in its `fails` list, and that every test in its `ripples_to` fails
(FAIL, not UNVERIFIED) while all others pass.

## 5. Checker

Write the builder's checker per `fprime-test-oracle`. It reads the
exported record and emits `verdicts.json`. Do not share it with the
reviewer, who writes an independent one.

## 6. Size and review

Target: a human reads the harness in one sitting. Put pass rules and
knob handling in one small file each; isolate bench scripts (power
control, flashing, ground-system start) from test logic. Harness code
is reviewed by the system-test reviewer and, if the human requests,
the local multi-agent review (`fprime-iterative-development` §2, step 4).
Document the harness in a short README: how to run one test, the whole
plan, and the dry run.

## 7. Dry run

Stage 2 closes with the dry run on the real bench: one clean
full-plan run (all PASS expected) plus one full-plan run per knob
applied alone, outputs under `dry/clean/` and `dry/<knob>/` outside
both repositories, both checkers agreeing on every verdict of every
run. Then `seal.py manifest` with the raw image binaries that were
flashed (each must contain the UUT sha7). Dry-run output is labelled
and is not a record.

Dry-run traps (SunCamMgr simulator runs sim1–sim9):

- Run the **checker on the simulator dry run** before any record; most
  first-run failures are predicate or harness defects, and each one is
  either a plan amendment (reviewer confirms) or a UUT defect — write
  down which.
- Start the simulator with the interpreter that has its image library,
  never through `head`/`tail` (SIGPIPE kills it mid-run → the UART driver
  reports `OTHER_ERROR`). Give it a per-chunk delay so captures take
  seconds, or mid-readout tests (BUSY, status during readout) cannot run.
- `pkill -f <pattern>` kills the calling shell when the pattern is in its
  own command line — anchor the pattern (`^python3 run_plan.py`).
- Write the manifest in `finally` so a failed run still has one; archive
  `git diff` of an uncommitted tree next to the run.
- Check harness values against the device manual before blaming the UUT
  (an ISP backlight of 0x80 against a 0–10 range was the harness).
- Parse data-product containers from the record layout
  (`FwDpIdType` + `FwSizeStoreType` header before the image), not by
  guessing bytes before the JPEG SOI.

## Anti-patterns

- Fixing the UUT from the harness branch.
- Tests that pass by construction (assert on command return only, count
  without payload, no completion event awaited).
- Verdicts printed by the harness instead of computed by a checker
  from the record.
- Knobs that are visible in the record.
- pytest or harness verdicts anywhere under `record/`.
