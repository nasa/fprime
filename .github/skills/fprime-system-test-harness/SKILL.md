---
name: fprime-system-test-harness
description: >-
  Use when building the harness for an approved F Prime system test
  plan: tester components, integration tests, bench scripts, record
  markers, fault-injection planted faults, and the tests' named checks. Keeps the
  harness small, reusable, and reviewable, and keeps it out of the unit
  under test. Stimulus tests use the fth pytest plugin; planted faults are
  `@planted_fault` declarations. Keywords: F Prime, test harness, tester
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

### Layout rule (where the test code lives)

- **Deployment repo/branch that hosts the test-helper component**
  (e.g. `fprime-samd-reference` `fram-harness`,
  `CuriosityReference/FramTester/`): the tester component AND, beside
  it under its `test/int/`, the plan (`plan.json`, rendered `plan.md`),
  the planted-fault module (`planted_faults.py`, named by `plan.json`
  `helpers_module`; the tests carry their own checks), the
  generated fault menu, `bench.json`, the pytest stimulus and the
  adversary's independent checker (legacy plans with plan-owned pass rules only). They require the deployment the
  helper lives in, so they belong there.
- **UUT repo** (e.g. `fprime-devices` `Fram.FramDriver`): the component,
  its unit tests (`test/ut/`) and its SDD (whose Tests table is derived
  from the plan by `fth render-sdd-tests`). Nothing from the system
  test moves in here.
- **Tooling repo** (`fprime-testing-harness`): nothing UUT-specific —
  generic engine/runner/auditor/renderer plus vendored *records* as
  regression vectors; each vector names a vendored, clearly labelled
  COPY of its plan (`vectors/plans/<name>/SOURCE.json` = repo, branch,
  sha, path, per-file sha256). `fth run --plan/--tests` and
  `fth audit --plan/--tests` (legacy: `--independent`) point at the deployment checkout.

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
@pytest.mark.req("REQ-FRAM-001", "REQ-FRAM-002")        # requirement ↔ test (VnV matrix)
def test_L2_1(window, send, bench, check, require):      # test_<ID>, or @pytest.mark.plan_test("L2-1")
    with window(30, 31):                      # START marker … END marker in finally
        require("STD")                        # markers, dispatch, drops, samples → else Incomplete → UNVERIFIED
        bench.wait_samples("framDriver.WritesCompleted", 2)
        send("framTester.WRITE_PATTERN", [0, 0, 64, 0x5A])
        send("framTester.VERIFY", [0, 0, 64, 0x5A])   # the drawn planted fault may rewrite this, for L2-1 only
        check("ABS")
        check("event framTester.VerifyDone(client=0, offset=0, length=64, status=OK, mismatches=0) count == 1")
        check("delta(framDriver.WritesCompleted) == 1")
```

Bodies send the stimulus and **declare the test's named checks** inside
the window (`check("<predicate>")` in the plan grammar, or
`check("<name>", fn)` with `fn(h)` over the closed window's history);
at window close every check is judged over the record and all failures
are raised together (`CheckFailures`, no fail-fast) — the test's verdict
is pytest's, `fth run` only reads the junit. Waits are pacing only
(`bench.wait_event/wait_channel/wait_samples/reset/wait`). An expected
UUT event that is absent is a **FAIL**; `require(...)` is for
infrastructure absences only (Captain's rule). Check names are the
identity the planted-fault `fails` lists, the unseal and the VnV matrix
use — keep them verbatim-stable and **constant per test** (built from
literals, never from live data: the gate harvests them from one dry
run); `fth verify-plan plan.json --tests test/int` dry-runs the tests and
refuses a `fails` entry no test declares, and `fth run` repeats that
pre-flight before flashing. A window must declare at least one check
(a check-less window can never PASS); a judged FAIL stands whatever else
happens on the test (teardown error, body exception, a later
`Incomplete`); all failed checks are raised together at the test's last
window — the stimulus of every window runs so the record stays whole for
replay. END carries **no verdict** (contract §4).
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

## 3a. Coverage: reports are RfR inputs, the image stays pure (on-target needs GCC ≥ 11 freestanding gcov)

The RfR runs the pure, uninstrumented image; an instrumented image is never the qualified
image. The coverage REPORTS, though, are **RfR inputs** (Captain's ruling): the builder hands
`dev-evidence/ut`, `dev-evidence/system` and `evidence.json`+`identities.json` to the runner, who
pins them in row 2 (`fth manifest --ut-coverage --system-coverage --coverage-evidence`) so row 5
renders `coverage.md` + `inputs/coverage/` in the RfR package. An instrumented
image is not the qualified image. The builder produces coverage with
`fth dev-campaign --out dev-evidence/ …`, which runs (a) UT coverage on the host
(`fprime-util check --coverage` in a **default-config** project — a deployment's
trimmed config, e.g. `FW_FILEID_ASSERT`, fails UTs for configuration reasons),
(b) the plan on the gcov-instrumented image followed by the `DUMP_COVERAGE` dump
plan, reassembled and measured by `fth coverage-system`, and (c) the pure run on
the RfR image judged by the runner's junit and the auditor's replay of the same tests; it writes one
`dev-evidence/` bundle (banner `DEVELOPMENT EVIDENCE — NOT RfR`, `ut/`, `system/`,
`pure/`, `identities.json`, `findings.md`, and `ql/` — the dev QL package of the
pure run rendered with `--dev`: index with both image identities + tri-state VnV
matrix, test pages, `commands.md`/`helpers/`/`sdd.md` when `--dictionary`/`--sdd`
are given, `coverage/ut` + `coverage/system` copied from the bundle) and exits non-zero with a findings
list when a pure-run verdict is not PASS, runner and replay disagree (DIVERGENCE), or coverage is
under the plan's `coverage: {lines_min, branches_min, not_covered_ack}` (design
skill §2). "Iterate until clean" = repeat until it exits 0 and the uncovered
lines are all in the acknowledgement. **Caveat — provenance, not equivalence:**
until a toolchain pin is approved, the coverage image may be a different
compiler and binary than the RfR image (FRAM: GCC 13 instrumented vs GCC 9
pure); the bundle records both identities (image sha256 + read-back, toolchain,
repo shas) and claims nothing about one from the other. The toolchain override
(`SAMD_TOOLCHAIN_ROOT`, fprime-samd branch `gcc-override`) stays on its branch
until the human decides.

On-target coverage needs a compiler with
freestanding gcov (`-fprofile-info-section`, GCC ≥ 11; GCC 9 rejects it)
and costs RAM: 8 bytes per arc counter plus a per-function record — on a
16 KiB part instrument only the UUT's and the tester's hand-written
sources, never a whole target with its autocoded `*Ac.cpp`, and use
`-fprofile-filter-files=<UUT>\.cpp$|<Tester>\.cpp$` so header inlines
instantiated in those TUs are not counted. Recipe (FRAM trial,
`fprime-samd-reference` branch `coverage-trial`):

1. **Build option, never default**: `-DFRAM_COVERAGE_TRIAL=ON`-style
   option adds `-fprofile-arcs -ftest-coverage -fprofile-info-section
   -fprofile-update=single` to the two sources (`set_source_files_properties`),
   links `-lgcov` (only `__gcov_merge_add` / `__gcov_info_to_gcda` are
   pulled; no hosted libc) and, under `-flto`, adds `-fdata-sections
   -ffunction-sections` at link so LTRANS keeps the `__gcov0.*` sections.
2. **Linker script**: `.gcov_info` (KEEP, `__gcov_info_start/end`) in
   flash; `.gcov_counters (NOLOAD)` in RAM collecting `.bss.__gcov0*`
   outside `.bss`, so counters **survive the plan's MCU resets**; the
   tester zeroes them on a cold start / image change (magic + identity
   word beside them) and on `DUMP_COVERAGE(count=0)`.
3. **Tester command** `DUMP_COVERAGE(start: U16, count: U8)`: walks
   `__gcov_info_start..__gcov_info_end` with `__gcov_info_to_gcda`, emits
   global 32-byte chunks `start..start+count-1` as `CoverageChunk(object,
   offset, length, data0..data7)`, `CoverageObject(object, nameCrc, bytes)`
   with an object's last chunk (`nameCrc` = CRC-32 of the compiler's
   `.gcda` path — how fth finds the matching `.gcno`), and
   `CoverageDumpDone(objects, totalBytes, crc)` once the window reaches
   the end. Everything that runs **between** two windows — the dump path
   itself and any periodic handler of the tester (its `schedIn`) — carries
   `__attribute__((no_profile_instrument_function))`, otherwise the
   counters move between commands and the windows describe different
   streams (the first FRAM dump failed exactly so: the Done CRCs disagreed).
   `count` is capped so two events fit one downlink frame; the
   **ground paces** the walk (one command per window, `bench.wait`),
   because a baremetal framer with two buffers drops packets under a
   burst — `delta(framer.DroppedPackets) == 0` is in the dump plan's checks.
4. **Dump plan**: a separate one-test plan + stimulus beside the main
   plan (`test/int/gcov-dump/`), run with `fth run --unsealed` right after
   the main plan on the same image (fth's flash of the *same* image keeps
   the counters). Then `fth coverage-system --record <run>/record
   --gcno-root <build cache> --project-root <checkout> --filter <UUT dir>
   --filter <tester dir> --gcov arm-none-eabi-gcov --out <dir>` and
   `render-ql … --system-coverage <dir>`. Gaps, repeated-but-different
   chunks, byte-count or CRC mismatches are loud errors; never report
   partial coverage.
5. **Report** the size table (flash, static RAM, counter region, stack
   headroom) baseline vs instrumented and the dump cost (bytes, events,
   commands, duration); an instrumented image is a **development trial,
   not an RfR image** — the RfR verdicts come from the uninstrumented
   pinned build.

Toolchain: a compiler override (e.g. `SAMD_TOOLCHAIN_ROOT` in
`fprime-samd`'s toolchain file) lives on its own branch until the human
decides to pin it; the trial never repins the deployment's toolchain.

## 4. Planted faults

Implement every planted fault as a `@planted_fault` declaration in the plan's helpers
module (`fprime-test-oracle` §5): a command rewrite or insertion that
the plugin's private sender applies to the **drawn test only** (no fixture exposes the draw; a test naming `faults`/`sealed` is refused by `fth verify-plan --tests`), from
`rfr/<run-id>/sealed/faults.json` (`{"manifest_sha256": …, "faults":
{"<test>": "<planted fault>"}}`); a malformed, unknown or stale file stops the
run before any command, naming no planted fault. Fill `ripples_to` with any
undrawn test a planted fault necessarily fails. A planted fault may change a seed, an
expected constant, a length, a stimulus, or disturb device state
through an inserted harness command — never the test ID, the log text,
or the marker. The harness **never logs, prints, asserts on, or writes
which planted faults are active**, to any file or stream, including its own
stdout in `runner/`; the only trace of `faults.json` outside `sealed/`
is the seal-id run-start marker. The menu is `fth planted-faults-menu` output,
never hand-written. The mutation vectors (§7) prove each planted fault, applied
alone, fails its target with exactly its `fails` entry and its
`ripples_to` with FAIL, not UNVERIFIED, while all others pass.

## 5. Checks

The checks live in the integration tests: `check("<stable name>")` /
`require(...)` inside `with window(...)`, judged over the closed window's
history; the runner's verdict is pytest's junit (fail → FAIL, `Incomplete`
→ UNVERIFIED) and the auditor replays the same test functions over the
sealed record (`fth audit --tests`). Check functions beyond the grammar
live in the test file and are reviewed like any check. A reviewer's
independent checker exists only for legacy plans with plan-owned pass
rules (`fth evaluate`) — do not discuss check internals with the reviewer
before the review.

## 6. Size and review

Target: a human reads the harness in one sitting. Put the named checks and
planted fault handling in one small file each; isolate bench scripts (power
control, flashing, ground-system start) from test logic. Harness code
is reviewed by the system-test reviewer and, if the human requests,
the local multi-agent review (`fprime-iterative-development` §2, step 4).
The review scope of the harness stage explicitly includes, besides the
tooling: the tester component (FPP, C++, SDD), the stimulus tests, the
planted fault bodies and the helper predicates — lens passes (code, design,
architecture, test quality) AND an adversarial pass (planted fault acting outside
its drawn test via shared state; stimulus tautology — the harness emitting
what the oracle checks; helper predicates without known-answer tests; the
tester hiding UUT errors; steps that depend on another test's device
state). Review findings are written locally, never posted to GitHub.

Before hand-off the builder answers the reviewer's five questions
(`system-test-review.agent.md` Mode B step 3b) as a self-check, with
file:line evidence: (1) tautology — does any plan check pass because the
harness emits what the oracle checks, without the UUT doing the work?
(2) swallowed status — does the tester swallow, default or remap any
UUT/driver status or return instead of surfacing it in an event/channel
the plan checks? (3) planted fault reach — can a planted fault body act outside its
`applies_to`/drawn id via module, fixture, device state or command
ordering? (4) cross-test state — does any step rely on device state left
by a prior test, so that reordering or a single-test rerun changes the
verdict? (5) helper KATs — does every `@helper` have a known-answer test
(pass and fail cases)? The stage-2 exit gate (design skill §6) requires
the reviewer's step-3b round on file with no open must-fix; mechanics
checks alone do not close stage 2.
Document the harness in a short README: how to run one test, the whole
plan, and the dry run.

**Stage-2 hand-off** (build agent step 5): plan, harness, mutation
vectors, manifest **and** the `dev-evidence/` bundle from §3a incl. its
dev QL package (`dev-evidence/ql/`; UT coverage, on-target coverage, pure
run), clean (`fth dev-campaign` exit 0) — one bundle, nothing else to
assemble by hand. The human gate lists dev-evidence as a *review* input beside the RfR
inputs; it is never an RfR input and nothing from it enters the RfR
package or the stage-3 checklist.

## 7. Dry run

Stage 2 closes on the exit gate of `fprime-system-test-design` §6:
`fth run` dry runs on the real bench with neutral run ids (`dry-1`…;
the planted fault → run mapping lives only in the orchestration script) kept as
mutation vectors — one clean record (all PASS) and one per planted fault applied
alone (its target FAILs exactly its `fails` set, its `ripples_to` FAIL,
everything else PASSes), `fth vectors` green, hand `pytest` ==
`fth run --replay-record` per test and `fth audit --tests` without
DIVERGENCE on every one — plus the **stage-3 rehearsal** with
a throwaway seed (`fth draw` → `fth run` → `fth audit --tests` →
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

- Run `fth verify-plan --tests <test/int>` (dry-runs the tests: declared checks, `fails:`, blind guard, `req` == `covers`) before any record; most
  first-run failures are predicate or harness defects, and each one is
  either a plan amendment (reviewer confirms) or a UUT defect — write
  down which.
- Start the simulator with the interpreter that has its image library,
  never through `head`/`tail` (SIGPIPE kills it mid-run → the UART driver
  reports `OTHER_ERROR`). Give it a per-chunk delay so captures take
  seconds, or mid-readout tests (BUSY, status during readout) cannot run.
- Kill only by a pid you recorded when you launched the process (`kill $PID`), never by
  pattern: on a shared bench `pgrep -f`/`pkill -f` matched a `bash -c` wrapper instead of the
  runner it wrapped, the runner lived on, and two flashers met on one CMSIS-DAP probe within the
  same second (`CMD_CONNECT failed`, empty read-back). One actor per bench, one recorded pid per
  launch, and a `MATCH/MISMATCH` read-back line after every flash.
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
- Planted faults that are visible in the record.
- pytest or harness verdicts anywhere under `record/`.
