---
name: fprime-system-test-harness
description: >-
  Use when building the harness for an approved F Prime system test
  plan: tester components, integration tests, bench scripts, record
  markers, fault-injection knobs, and the builder's checker. Keeps the
  harness small, reusable, and reviewable, and keeps it out of the unit
  under test. Stimulus tests use the fth pytest plugin; knobs are
  `@knob` declarations. Keywords: F Prime, test harness, tester
  component, integration test, markers, fault injection, fth, record.
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
leave the system deployment's diff empty. When the project has no
deployment, check for guidance first; without a human call, build a
temporary uncommitted one outside the repositories (contract §2) —
never add one to a repository.

## 3. Record and markers

Every test is bracketed in the record by the plan's marker command
(`markers.command`, e.g. a tester `MARK(id: U32)` → `Marker(id)` event,
or `cmdDisp.NO_OP_STRING` where the deployment has it — 40-char
strings, so `<run>` and `<id>` are ≤ 8 characters) so the auditor can
slice it without timestamps (F Prime generation time may be
uptime-based). `fth run` sends the run-start markers (`0xFFFF0001`,
manifest sha8, `0xFFFF0002`, seal-id sha8, `0xFFFF0003`, `crc32(run
id)`; ids ≥ 0xFFFF0000 are reserved) and runs the stimulus tests
through `fth.pytest_plugin`:

```python
def test_L2_1(window, send, bench):          # test_<ID>, or @pytest.mark.plan_test("L2-1")
    with window(30, 31):                      # START marker … END marker in finally
        bench.wait_samples("framDriver.WritesCompleted", 2)
        send("framTester.WRITE_PATTERN", [0, 0, 64, 0x5A])
        send("framTester.VERIFY", [0, 0, 64, 0x5A])   # the drawn knob may rewrite this, for L2-1 only
```

Bodies are **stimulus only**: no assertions on results, only pacing
waits (`bench.wait_event/wait_channel/wait_samples/reset/wait`); the
record is judged on replay. END carries **no verdict** (contract §4).
The L0 feasibility gate sends one marker and checks the archive shows
it byte-exact (`NO_OP_STRING` truncates when a project shortens
`FW_LOG_STRING_MAX_SIZE`; a `MARK(U32)` round-trips or not) — nothing
is sliceable until the deployment decision is revisited. Reused
`test/int/` tests assert inline, so wrap them to discard the pytest
verdict. Harness stdout and pytest output go to `runner/`, never into
`record/`. The fixture API is `docs/plan-format.md` of the tooling.

Record requirements the harness must make possible:

- `bench.json` (relative paths only; hashed into the manifest) with
  the `flash`, `dump`, `reset`, `gds`, `recorder` and `send` command
  lines and timeouts; `fth run` starts one ground system and **one**
  recorder per run ID and the sender logs under `record/raw/cmd/`.
  Dedup of the recorder's double-written rows is the exporter's job
  (`fth export`: exact raw line, per file, per boot segment) — never
  the harness's or a checker's.
- Completeness per window from infrastructure facts only
  (`std_complete`): both markers; `commands_sent ==
  count(event cmdDisp.OpCodeDispatched)`; `OpCodeError`,
  `TooManyCommands` and the framer/event-logger drop counters unchanged
  unless the test expects a rejection; `<packet> samples >= n`. UUT
  completion events are pass-rule facts, not completeness facts.
- Pacing: wait for each command's completion before sending the next;
  never more commands in flight than the dispatcher queue depth — a
  flooded dispatcher asserts, and that is a harness defect, not a UUT
  one.
- Downlink budget: estimate event rate against link capacity
  beforehand; a harness that loses events cannot prove an absence.

## 4. Fault knobs

Implement every knob on the approved menu as harness **configuration**
(`rfr/<run-id>/sealed/faults.json`, read by the harness at start). The
draw plants knobs **per test** — the file is `{"manifest_sha256": …,
"faults": {"<test>": "<knob>"}}` — so the harness must switch a knob on
only for the test it was drawn for, never by knob name for every test
in `applies_to` (fram-r2: two drawn knobs made two undrawn tests fail
and `unseal` reported REAL FAILs that were harness scope errors). Fill
`ripples_to` with any undrawn test a knob necessarily fails. A
malformed or unknown fault file must fail loudly (stderr, non-zero),
never exit silently. A
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

The builder's checker is the tooling's engine: pass rules are plan
data, judged by `fth evaluate` (`fprime-test-oracle` §3). The builder
writes only what the grammar cannot say, as `@helper` functions in the
plan's helpers module, each returning `(holds, seen)` or `(holds,
seen, refs, unverified)`; a helper is reviewed like a predicate. The
reviewer's independent checker is written from the plan text alone —
do not discuss helper internals with the reviewer before it exists.

## 6. Size and review

Target: a human reads the harness in one sitting. Put pass rules and
knob handling in one small file each; isolate bench scripts (power
control, flashing, ground-system start) from test logic. Harness code
is reviewed by the system-test reviewer and, if the human requests,
the local multi-agent review (`fprime-iterative-development` §2, step 4).
Document the harness in a short README: how to run one test, the whole
plan, and the dry run.

## 7. Dry run

Stage 2 closes on the exit gate of `fprime-system-test-design` §6:
`fth run` dry runs on the real bench with neutral run ids (`dry-1`…;
the knob → run mapping lives only in the orchestration script) kept as
mutation vectors — one clean record (all PASS) and one per knob applied
alone (its target FAILs exactly its `fails` set, its `ripples_to` FAIL,
everything else PASSes), `fth vectors` green, engine and independent
checker byte-equal on every one — plus the **stage-3 rehearsal** with
a throwaway seed (`fth draw` → `fth run` → `fth audit --independent` →
`fth unseal`, no tooling error; fram-r1 was voided at run start because
the harness had only ever read a hand-written dry-run fault file). Then
`fth manifest` with the raw image binaries that were flashed and
`--toolchain-json`. Dry-run output is labelled and is not a record.

Bench entry (before the first dry run on hardware): prove the link in
order and say which layer is proven — **host** (your own heartbeat log
line `sent=N`; a TX LED or `flush()` corroborates), **wire** (loopback,
or one direction already proven by received bytes), **device** (a reply
that decodes per the datasheet). Unsolicited device text is an identity
clue: search the literal string and vendor word before hypothesising
wiring faults (`MVP Camera V2 RS422` was a different camera, not a
reversed pair). For an externally powered device, listen first: hold
the port open with TX silent across a power cycle, capture the boot
text, then transmit; bytes emitted while nothing holds the port are
lost and look like "no response".

Dry-run traps (SunCamMgr simulator runs sim1–sim9 and the bench):

- Run `fth evaluate` on the simulator dry run before any record; most
  first-run failures are predicate or harness defects, and each one is
  either a plan amendment (reviewer confirms) or a UUT defect — write
  down which.
- Start the simulator with the interpreter that has its image library,
  never through `head`/`tail` (SIGPIPE kills it mid-run → the UART driver
  reports `OTHER_ERROR`). Give it a per-chunk delay so captures take
  seconds, or mid-readout tests (BUSY, status during readout) cannot run.
- `pkill -f <pattern>` kills the calling shell when the pattern is in its
  own command line — anchor the pattern (`^python3 run_plan.py`) or
  bracket a character (`strea[m].py`); to free a serial port use
  `fuser -k /dev/<tty>`.
- A background stream is running when **its own output** says so, never
  because `pgrep` found something (it matched the agent's own ssh/bash
  wrappers while the stream had never started, and the human watched a
  dead TX LED). Every bench stream prints a heartbeat; the report to the
  human quotes it.
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
