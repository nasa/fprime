---
name: fprime-run-for-record
description: >-
  Use when executing or auditing a run-for-record (stage 3 of the
  system test process): freezing the manifest, drawing the human-seeded
  planted faults, running the approved plan once, hashing the record,
  auditing it blind into quick-looks, and producing the unseal table
  the human reviews. The mechanics are the `fth` CLI of
  fprime-testing-harness (manifest, verify, draw, run, audit,
  hash-record, verify-record, unseal). Keywords: F Prime, run for
  record, RfR, seal, manifest, fault injection, quick-look, audit,
  unseal, fth.
---

# Skill: F Prime run-for-record

Stage 3 of `.github/agents/_shared/test-process-contract.md`. Nothing
here designs or fixes; it executes an approved plan against a frozen
UUT and harness and makes the result inspectable. Every artifact
lives in the run directory `rfr/<run-id>/` outside both repositories;
`<run-id>` ≤ 8 characters.

## 0. Tooling

The mechanics live in the `fth` CLI of
[fprime-testing-harness](https://github.com/LeStarch/fprime-testing-harness)
(`fth manifest | verify | draw | seal-id | run | audit | hash-record |
verify-record | unseal | vectors | selftest`), pinned by version in
`.github/agents/_shared/test-process-contract.md` §8 — no script ships
beside this skill. Install the pinned commit on every host that runs a
stage-3 step (bench host included):

```bash
pip install "git+https://github.com/LeStarch/fprime-testing-harness@<pinned commit>#egg=fprime-testing-harness"
fth selftest          # exercises every seal subcommand in a temp dir
```

Requirements the tooling must keep meeting (its own test suite carries
them; a tool that drops one is not pinned): a planted fault's `fails` is a
per-test dict (`{"<test>": [predicate…]}`; a legacy list expands to
every `applies_to` test) and `unseal` matches failed predicates against
that test's entry only; `draw` reads the menu from the manifest;
`unseal` needs the menu file at its manifest-relative path.

---

## 1. Manifest (closes stage 2)

```bash
fth manifest --uut <path> --harness <path> \
    --repo project=<path> --repo fprime=<path> [--repo <lib>=<path> …] \
    --plan <plan.json> [--plan-revision <rev>] --checker reviewer=checks/<name>.py \
    --image <Deployment>=<flashed>.bin:"<build command>" \
    --menu <faults-menu.json> --bench <bench.json> \
    --toolchain-json <toolchain.json>              # -> rfr/manifest-<uut sha8>.json
```

Records, for the UUT, the harness (both listed in `repos`
automatically — tester components and test deployments compile into
the image) and every other repository the image is built from, the
SHA and `git describe --tags --always --dirty --broken` string (dirty
or broken trees are refused); file hashes of the plan, the generated
fault menu, `bench.json` and (legacy plans) the reviewer's independent checker; the
plan's test list (`tests[].id`, unique, ≤ 8 characters); the
**toolchain** block from `--toolchain-json` (compiler package and
version as the builder installed them — without it a faithful rebuild
can only be reported `REBUILD DIFFERS (toolchain unpinned)`); and the
exact raw binaries that will be flashed — each must contain every
repository's describe string, which is what `Svc.Version` reports, or
`--no-version-check <reason>` is recorded on the manifest and printed
at unseal. ELF, hex and UF2 files are refused (flash the `.bin` and
read back the range with `picotool save -r` or OpenOCD `dump_image`).
The builder's checker is the tooling's engine (`fth evaluate`), so only
the reviewer's checker is a manifest file. The manifest lives beside
the runs, outside both repositories; `fth verify <manifest>` must pass
before every later step. Before announcing the draw, stage on the bench
host (where the human draws and unseals) the pinned `fth`, the
manifest, and every manifest-listed non-repo file at its recorded
relative path (plan, fault menu, bench, checker, the helpers module)
— `unseal` reads the menu by that path and fails with "missing fault
menu" otherwise; `bench.json` uses relative paths only for the same
reason. The plan's independent final-state tool (§4.5) must have been
exercised once in the stage-2 rehearsal; an improvised read on the day
yields `UNVERIFIED`.

## 2. Draw (human action)

The human runs this in a shell of their own, on any machine holding the
pinned `fth` and the manifest (the fault menu is embedded in the
manifest; no repository or other file is needed — `draw` checks only
the manifest file, the full `verify` is the runner's and auditor's
step). The seed comes from an un-echoed prompt (preferred) or a
`chmod 600` file that `draw` deletes after reading — never a chat
message or a command-line argument — and is kept by the human.

```bash
fth draw --manifest rfr/manifest-<sha8>.json --run-dir rfr/<run-id> [--seed-file <file>]
```

Writes `rfr/<run-id>/sealed/{faults.json, expectations.json,
sealed.sha256}` and `rfr/<run-id>/seal-id.txt`. The number of planted
tests is drawn in `[1, max_count]` from the approved menu (no
command-line override); expectations are `FAIL` for drawn tests,
`FAIL-RIPPLE` for their planted faults' `ripples_to`, `PASS` otherwise. The
draw **refuses to uncover a requirement**: if a candidate draw leaves
any requirement without an unaltered covering test it re-draws from a
derived sub-seed, deterministically, up to `--max-tries` (16), then
fails loudly — a plan that trips this has a redundancy defect
(`fprime-system-test-design` §3) and goes back to stage 2. Only
`seal-id.txt` (and the sha8 the command prints) is readable by runner
and auditor.

## 3. Runner

One command, run once per run ID, from the campaign root on the bench
host:

```bash
fth run --run-dir rfr/<run-id> --manifest rfr/manifest-<sha8>.json \
    --plan <plan.json> --bench <bench.json> --tests <deployment>/test/int
```

Before it, on a **shared bench**, the runner checks for foreign processes
on the serial port and the debugger (`pgrep -fa "fprime-gds|fprime-cli|
openocd|comm_bridge|pytest"`, plus who holds the target's `/dev/tty*`)
and writes the result — timestamp, the processes found, which port each
holds — into the run log (`rfr/<run-id>-run.log` / `runner/notes.md`)
before launching. A process on *another* port is noted, not a blocker;
a process on the target's port stops the launch. If a foreign process
kills the ground system mid-run, the run is void as an **external
outage** (recorded as such in `runner/notes.md`), not a verdict, and is
rerun once under a new run ID; a second outage is reported to the human.
Never power-cycle a shared USB hub to clear a port.

It performs, in order, and stops at the first failure:

1. `fth verify` of the manifest (repos at their SHAs, clean trees, file
   hashes, pinned `fth`).
2. Flash exactly the manifest image with the recipe's `flash` line,
   read back the same flash range (`dump`), and write
   `record/raw/flash.log` with the debugger USB serial, the image
   sha256 and the read-back sha256; a missing serial or a differing
   read-back stops the run.
3. Start the ground system and **exactly one** recorder from
   `bench.json` (`gds`, `recorder`), fresh archive under `record/raw/`
   for this run ID; the command sender logs under `record/raw/cmd/`
   (the only source of `commands.jsonl`).
4. Send the run-start markers `MARK(0xFFFF0001)`, `MARK(manifest
   sha8)`, `MARK(0xFFFF0002)`, `MARK(seal-id sha8)`, `MARK(0xFFFF0003)`,
   `MARK(crc32(<run-id>))` (ids ≥ 0xFFFF0000 are reserved; a deployment
   with `NO_OP_STRING` may instead send `"RFR <run> M <sha8>"` /
   `"RFR <run> S <sha8>"` — the plan's `markers` block decides).
5. Run the plan's stimulus tests once, in plan order, through
   `fth.pytest_plugin` (`window`, `send`, `bench`, `faults` fixtures);
   the plugin validates `sealed/faults.json` and applies the drawn planted fault
   to the drawn test only; stdout and pytest output go to `runner/`.
6. Stop the ground system; `fth export` the raw logs into
   `record/{events,channels,commands}.jsonl` + `export-counts.json`
   (dedup of the recorder's double-written rows happens here, per file
   per boot segment, exact raw line only); `fth hash-record`.
7. Derive `runner/verdicts.json` from pytest's junit (`fth.junit`; the junit is copied into `record/` before hashing),
   `runner/report.md`.

The runner agent adds nothing to the record: every anomaly, operator
action and timing it noticed goes to `runner/notes.md`. It does not
read `sealed/`, fix, tune or retry; a mid-run break is recorded and
reported, and a rerun is the human's call with a new run ID. Hand over
the run directory by path; the auditor reads `record/`,
`record.sha256`, `seal-id.txt` and nothing under `runner/` or
`sealed/`.

## 4. Auditor

```bash
fth audit --run-dir rfr/<run-id> --manifest rfr/manifest-<sha8>.json \
    --plan <plan.json> --tests <test/int> --bench <bench.json> \
    [--rebuild --toolchain-json <toolchain.json>]      # legacy plans: --independent checks/<name>.py
```

Each step is a line in `auditor/report.md`; a failed hash or marker step
ends the audit with `RECORD INVALID`:

1. `fth verify` of the manifest and `fth verify-record` (every export
   file present, row counts equal `export-counts.json`); the run-start
   markers in `record/events.jsonl` match this manifest, this run ID and
   `seal-id.txt`; `record/raw/flash.log` carries the debugger serial and
   a read-back sha equal to a manifest image sha (`RECORD INVALID
   (image)` otherwise).
2. With `--rebuild`: rebuild from the manifest SHAs with the recorded
   `build_cmd` and toolchain and report `REBUILD MATCH` or `REBUILD
   DIFFERS (<exactly which pinned item differs>)` — cross-compiled images
   often embed paths; a differing rebuild is reported, not `RECORD
   INVALID`.
3. Re-export from a **copy** of `record/raw/` and compare with
   `record/*.jsonl` (the independent count); replay never touches the
   archive in place.
4. Record integrity findings (one START..END per window, no command
   outside a window, monotonic time, export counts, junit hashed, no
   duplicate test), then **replay the same test functions over the
   record** (`--tests <test/int>`) → `auditor/junit.xml` →
   `auditor/verdicts.json` and diff verdict **and** `failed` list per
   test against `runner/verdicts.json` → `auditor/diff-runner-replay.md`;
   any `DIVERGENCE` or integrity finding is a human trigger. Legacy
   plans with plan-owned pass rules: engine [+ `--independent`
   checker] → `auditor/diff.md` as before.
5. Render the quick-look pages, `index.md` (metadata, VnV matrix) and
   `ql.zip` into `auditor/ql/` — **by program only** (`fth render-ql`
   inside `audit`). No agent writes, edits or summarises a QL page,
   ever; the auditor's words go to `auditor/report.md`.

Then, outside the tool: the plan's independent final-state check after
the runner has released the bench and before any reflash — UUT powered
down or halted, a non-F´ tool — written to `auditor/final-state.md`
(narrative) and `auditor/final-state.json`: `{"test": "L7-1",
"verdict": "PASS|FAIL", "tool", "version", "cmdline",
"expected_sha256", "observed_sha256"}`; `unseal` adds it as its own row
and a missing file is a human trigger. `auditor/report.md` closes with
every `UNVERIFIED` window, every divergence, the final-state result,
the rebuild result and the blindness statement (contract §4). Do not
open `runner/` or `sealed/`; do not guess which failures were planted.

`fth audit --skip-repo-check` exists for hosts without the repos
(tooling CI, vectors) and is **development-only**: it prints
`repo_check: skipped` in the QL index metadata and a ⚠ line under every
page header, so a campaign audit that used it is visible to the human
on every page.

## 5. Quick-look pages

`auditor/ql/<id>.md`, rendered from the hashed record and the plan,
byte-identical across renders: title and `purpose`; engine verdict;
`covers`; the pass rule with each predicate's result and the record
lines (`L`/`C`/`K` = events/channels/commands jsonl line numbers) that
decided it; completeness (commands sent vs dispatched, drops);
`rationale` lines; hex beside decimal where the rule uses hex; a "vs
plan" column that is an observation, never attributed to a planted fault. The
pass rule is on the page so the human checks the evidence against the
rule, not against anyone's word. `index.md` carries the run metadata
(manifest, seal id, `repo_check`) and the requirement × test VnV
matrix.

Simulator evidence is labelled `SIMULATED` in the manifest and on the
pages and is never a record; the record requires the device to pass the
bench entry gate (answers a probe at the configured baud) first.

## 5a. QL package contract (acceptance checklist)

Every item is program-generated from the plan, the record, the F´ dictionary and the FPP
model — no LLM prose anywhere in the package. The tooling's `tests/test_ql_contract.py`
asserts each item; a package missing one is not deliverable.

Package files: `index.md`; one `<test id>.md` per plan test; `sdd.md` (the UUT's SDD with
`<a id="REQ-…"></a>` anchors, its Tests table derived from the plan — the renderer fails on
drift); `commands.md` (every command the plan uses: component.mnemonic, opcode hex and
decimal, each argument with type, enum values and meanings, FPP doc-comment; the events
whose doc-comment names the command; dictionary sha256); `helpers/<Component>.md` for every
plan `helpers` entry (FPP description, port diagram, commands → `commands.md`, events with
id/severity/format/args, channels with id/type, parameters, source files with sha256 linked
at the manifest's project sha); an HTML twin of every page (`index.html` entry point,
`<page>.html` beside each `.md`, one offline `style.css`; `.md` links rewritten to `.html`,
heading and `REQ-…` anchors preserved; ```mermaid blocks pre-rendered to inline SVG by
`mmdc` when the host has it, else the vendored `assets/mermaid.min.js` renders them from
`file://` — no CDN, no plugin, no timestamps; the `.md` stays the diffable source); `inputs/`
(the package is self-contained: `manifest.json`, `plan.json`, `faults-menu.json`, `bench.json`,
the pinned independent checker and the sibling modules it imports (legacy plans only), the image(s) the
manifest names with its pinned sha256 and the flash read-back sha, the F´ dictionary, and
`inputs/README.md` listing each file, its sha256, the manifest pin and the stage-3 row that
consumes it — and, as DATA-ONLY rows (`repo@sha path sha256`, "pinned, not shipped"), the integration tests,
their checks and the planted-fault module the manifest pins as `stimulus` (`fth manifest --tests`): the auditor
replays exactly those sources or FAILS; they are never copied into the package — a sha256 that disagrees with a pin is a loud failure; `fth unseal` adds `unseal.md`
and `seed.txt` (verified to reproduce the draw) — the seed enters the package only after the
unseal, never before; tests, stimulus and tester sources stay in the pinned repositories); `ql.zip` of
all of it — stage-3 row 8 "Deliver" is `rfr/<run>/auditor/ql/ql.zip` and nothing else. **No coverage of any kind** is in an RfR package: an instrumented image is not
the qualified image, and the record is a pure record of the pinned build; `fth render-ql` /
`fth audit` refuse `--ut-coverage` / `--system-coverage` without `--dev`, which stamps the
package DEVELOPMENT EVIDENCE — NOT RfR. Coverage (UT and on-target) is development evidence,
produced by `fth dev-campaign` (harness skill §3a) and delivered beside the RfR inputs as a
review input — never as an RfR input or in this package.

`index.md`: run metadata (manifest sha8, record sha256, seal sha8, dictionary sha256, SDD
sha256, repo-check state); the per-test table (test, title, checker verdict, unseal
expectation, unseal verdict, page); ONE VnV matrix requirement × test with cells
P / E / F / U / · and an "Overall" column (any F → F; any P and no F → P; only E/U → U;
waiver quoted when no test covers) and the legend stating the cell meanings and the
roll-up rule; a "Test helper components" section; tests natural-sorted by id everywhere
(execution order is the plan's business, not the index's).

Each test page, in this order: `← back to [index](index.md)` under the H1 and as the last
line; header table — engine verdict, Unseal expectation, Unseal verdict (stamped by
`fth unseal`), "What this test does" (the plan's `purpose`), Covers (each REQ linked to
`sdd.md#REQ-…`), Harness (link to each helper page), Window start / Window end / Window
duration as separate rows per window with epoch; §1 Pass rule — one check per line
(#, window, group, check, plain-words description) with a Rationale line for every value a
rule deliberately ignores; §2 Procedure — plan steps aligned to sent commands with K lines,
every command name linked to `commands.md`, non-command actions explained ("not a
command — reboot visible as FSW clock restart …"); §3 "Possible Injected Faults" — sealed
draw line and the planted fault | drawn | checks-it-may-break table (a `— none —` row when no planted fault
applies, never an empty table, no "acted" column) filled by `fth unseal`, the italic note
says so; §4 Rule verdict (#, window, check, value seen, holds), the verdict line, the
unseal callout (⚠ when more checks failed than the drawn planted fault allows, a planted test
PASSED or an unplanted test FAILED; otherwise "consistent with draw"), the footnote that
counters are boot-cumulative and only Δ is judged, the caption that a handler's event
precedes its OpCodeCompleted; §5 Events — complete, nothing elided, Δ from the window
START marker, reboot rows marked; §6 Telemetry — one row per channel (samples, first/last
with C line and FSW, "all = v" / "changed: a → b" / "monotonic a → b (n distinct)");
§7 Commands last, with a "vs plan" column flagging arg differences and commands not in the
plan as OBSERVATIONS only ("Observation: command K14 differs …" singular when one). Hex
beside decimal wherever a rule uses hex (0x3FFC0 vs 262080).

## 6. Unseal (human action)

```bash
fth unseal --run-dir rfr/<run-id> --manifest rfr/manifest-<sha8>.json \
    -o rfr/<run-id>/unseal.md
```

Verifies the integrity chain (contract §6), then writes the check-off
table `☐ | test | expected | runner | auditor | quick-look | outcome`
for every test on the manifest plus one `<test>/dump` row per entry in
`auditor/final-state.json` (expected from the drawn planted fault's class, not
the test's expectation), flags planted or ripple tests whose failed
predicates exceed the planted fault's `fails` entry for that test, shows any
skipped image version check, and lists the human triggers first with
the seal id to compare with the value printed by `draw`. It also stamps
every QL page with `Unseal expectation` / `Unseal verdict`, inserts the
§3 planted fault table (name, class, kind, mechanism; `— none —` when nothing
applies) and a callout (⚠ WARNING for uncovered checks, a planted PASS
or an unplanted FAIL; else "Consistent with draw") into `index.md`, and
rebuilds `ql.zip`. The human reads trigger rows, spot-checks two or
three nominal rows against the raw record, ticks the boxes, and records
the decision in the plan's decision log.

## 7. Campaign close (human + agent, after unseal)

1. Update the watch ledger: append one evidence row per harness feature the campaign exercised to
   `docs/upstream-watch.md` in fprime-testing-harness (`<campaign> <run id> (<date>): <finding>`; move `status`
   to `earned` when the row's criterion is met). The QL index already links the ledger rows this run relied on
   (`watch ledger` field) at the harness sha the manifest pins — a ledger edit lands on main and is picked up by
   the next manifest, never by re-rendering a sealed run.
2. Archive `ql.zip` + `unseal.md` with the campaign record; note any `DIVERGENCE`, integrity finding or
   `UNEXPLAINED FAIL` as a tracked item.

## Anti-patterns

- A second run "to confirm" without a new run ID and a stated reason.
- Quick-looks written, edited or summarised by an agent, or built from
  the runner's report or harness output.
- `fth audit --skip-repo-check` on a campaign run.
- An auditor that announces which failures it believes were planted.
- A seed typed into a chat, a prompt, or a command line.
- Replaying the archive in place.
- Running pytest, the exporter or the engine by hand in stage 3 instead
  of `fth run` / `fth audit`.
