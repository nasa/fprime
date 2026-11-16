# Code Review Findings — Svc::PrmDb

Review scope: `Svc/PrmDb/PrmDbImpl.cpp` (PRM_REQUEST_DP data product generation).

## 1. Database mutex held across synchronous port invocations during DP generation — FIXED

**File:** `Svc/PrmDb/PrmDbImpl.cpp` (`PRM_REQUEST_DP_cmdHandler`)

**Was:** The handler acquired the guarded-port mutex once and held it for the entire database
scan, including every synchronous `dpGet_PrmEntryContainer` / `dpSend` call in the loop, until a
single `unLock()` at the end. A large or slow snapshot (many entries, or a slow/contended
`Svc::BufferManager` behind `productGetOut`) would block any other guarded-port caller (e.g.
`getPrm`) for the whole duration.

**Fix applied:** The handler now re-acquires the lock per entry: it locks just long enough to read
one entry's key/value and copy its bytes into a local scratch buffer, then unlocks before making
any `dpGet`/`serializeRecord`/`dpSend` port calls. The lock is never held across a port invocation.
This is safe because this active component dispatches one command at a time on its own thread, so
no other handler of this component can mutate the active database while the loop runs — the only
other accessor, `getPrm`, only reads.

## 2. PrmDpBufferFailed(0) conflated "ports unconnected"/"not configured" with a runtime failure — FIXED

**File:** `Svc/PrmDb/PrmDbEventDict.fppi`, `Svc/PrmDb/PrmDbImpl.cpp`

**Was:** The precondition check (ports not connected, or `configureDp()` never called) emitted
`PrmDpBufferFailed(0)` — the same event used for a genuine runtime `dpGet()` allocation failure,
always reporting a hardcoded size of `0` regardless of the real configured size.

**Fix applied:** Added a new event `PrmDpNotConfigured()` (id 18, severity warning high) dedicated
to the structural precondition failure. `PrmDpBufferFailed` is now reserved for actual `dpGet()`
runtime failures, where it reports the real requested size (`this->m_dpMaxDataSize`). The
`PrmRequestDpNotConfigured` unit test was updated to assert `PrmDpNotConfigured`. The SDD doc
(`docs/sdd.md`) was updated to describe both events and the lock-scope behavior.

## 3. PrmDpBufferFailed emitted from two sites with the same argument, not uniquely traceable (CPP-36) — FIXED

**File:** `Svc/PrmDb/PrmDbEventDict.fppi`, `Svc/PrmDb/PrmDbImpl.cpp`, `Svc/PrmDb/test/ut/PrmDbTester.cpp`

**Was:** `PRM_REQUEST_DP_cmdHandler` emits `PrmDpBufferFailed(this->m_dpMaxDataSize)` from two
`dpGet_PrmEntryContainer()` call sites — the first-container allocation and the
replacement-container allocation after a container fills up — with an indistinguishable argument
set (both report the same configured size). An operator reading the downlinked event cannot tell
which call site failed.

**Fix applied:** Added a `PrmDpBufferFailStage` enum (`FIRST_CONTAINER`, `NEXT_CONTAINER`) and a
second argument of that type to `PrmDpBufferFailed`. Each call site now passes its own stage
value, making the event uniquely traceable to its call site. Both `PrmRequestDpBufferFailure` and
`PrmRequestDpMidStreamBufferFailure` unit tests were updated to assert the stage argument. The SDD
doc was updated to describe the new argument.

---

All three findings have been addressed in code, tests, and documentation. Full unit test suite
(23/23) passes after the fix.

## PR #6392 review — Severity reconciliation items

The items below come from the "Severity reconciliation" table on
[nasa/fprime#6392](https://github.com/nasa/fprime/pull/6392). One item was promoted to must-fix;
the rest stayed at their original (lower) severity and were not promoted, but are listed here with
proposed fixes for visibility.

### 4. SDD "every `PRM_REQUEST_DP` fails with `EXECUTION_ERROR`" did not hold for an inverted range (must fix) — ALREADY FIXED

**File:** `Svc/PrmDb/docs/sdd.md`

**Was:** The SDD said every `PRM_REQUEST_DP` command failed with `EXECUTION_ERROR` while DP
generation is unconfigured, but an inverted range (`startId > endId`) actually short-circuits
first with `VALIDATION_ERROR` / `PrmDpInvalidRange`, regardless of configuration — making the
documented claim false.

**Status:** Already fixed in commit `cd350c0532` ("update must-fixes from CI"). The doc now reads:
"every `PRM_REQUEST_DP` command with a valid `[startId, endId]` range fails with
`EXECUTION_ERROR` ... An inverted range (`startId > endId`) is still rejected with
`VALIDATION_ERROR` and `PrmDpInvalidRange` first, regardless of DP configuration." No further
action needed.

### 5. Hidden AI-targeting HTML comment in PR body — N/A (not a code issue)

Flagged in the PR body template, not in any file in this repo. No code or doc change applies.

### 6. CPP-34: `while (true)` database traversal (suggestion, not promoted) — FIXED

**File:** `Svc/PrmDb/PrmDbImpl.cpp` (`PRM_REQUEST_DP_cmdHandler`, line 412)

**Was:** The snapshot loop is written as `while (true) { ...; if (!it.isInRange()) { break; } ...}`.
The reviewer rationale already confirms the loop terminates (the iterator always reaches the end
of a bounded `ArrayMap`), so this is a style-only nit, not a correctness issue.

**Fix applied:** Added a comment directly above the loop stating the termination condition
explicitly: the iterator advances once per pass and the underlying `ArrayMap` holds at most
`PRMDB_NUM_DB_ENTRIES` entries, so `it.isInRange()` becomes false once the iterator reaches the
end of that bounded range.

### 7. CPP-28: `FwSizeType` narrowed to `U32` event argument (could fix, not promoted)

**File:** `Svc/PrmDb/PrmDbEventDict.fppi` (`PrmDpBufferFailed`, line 147), `PrmDbImpl.cpp` (lines
452, 470)

**Was:** `PrmDpBufferFailed`'s `$size` argument is `U32`, but the value passed in
(`Fw::DpContainer::getPacketSizeForDataSize(this->m_dpMaxDataSize)`) is an `FwSizeType`, cast down
with `static_cast<U32>`. On a platform where `FwSizeType` is wider than 32 bits, a sufficiently
large configured buffer size would truncate silently in the event.

**Proposed fix:** Change the `$size` argument type to `FwSizeType` in `PrmDpBufferFailed` (other
events in the codebase, e.g. `Svc::BufferManager`'s size event, already use `FwSizeType` for this
purpose) and drop the `static_cast<U32>` at both call sites. Update the two tester assertions
(`ASSERT_EVENTS_PrmDpBufferFailed`) accordingly.

### 8. CPP-19: `DecodedRecord::id` uninitialized (suggestion, not promoted — test-only) — FIXED

**File:** `Svc/PrmDb/test/ut/PrmDbTester.hpp` (lines 107-110)

**Was:** `struct DecodedRecord { FwPrmIdType id; Fw::ParamBuffer value; };` leaves `id` with
indeterminate value until `productSend_handler` deserializes into it. Harmless in practice since
every slot is written before being read, but it reads as uninitialized to static analysis.

**Fix applied:** Gave `id` a default member initializer: `FwPrmIdType id = 0;`.

### 9. CPP-19: uninitialized test locals in the new DP decode path (suggestion, not promoted — test-only) — FIXED

**File:** `Svc/PrmDb/test/ut/PrmDbTester.cpp` (lines 1652, 1657, 1803)

**Was:** `decodedVal1`, `decodedVal2`, and `decodedVal` are declared without an initializer
immediately before being filled via `deserializeTo`, e.g. `U32 decodedVal1; EXPECT_EQ(...,
value.deserializeTo(decodedVal1), ...);`. Always written before read, but flagged by static
analysis.

**Fix applied:** Initialized each to `0` at declaration (`U32 decodedVal1 = 0;`), matching the
pattern already used elsewhere in the file (e.g. line 1217's `U32 testVal = 0x1234;`).

### 10. `Svc.PrmDbCfg.DEFAULT_DP_PRIORITY` not documented in the SDD (suggestion, not promoted) — FIXED

**File:** `Svc/PrmDb/docs/sdd.md`, `Svc/PrmDb/PrmDb.fpp` (line 89), `default/config/PrmDbCfg.fpp`

**Was:** `PrmEntryContainer`'s `default priority PrmDbCfg.DEFAULT_DP_PRIORITY` is set in `PrmDb.fpp`
but never mentioned in the SDD, even though the compile-time config knob is project-adjustable.
Not promoted because it's a config constant, not a command/event/channel/parameter the SDD
dictionary is required to cover.

**Fix applied:** Added a sentence to SDD §3.2 (directly after the existing `PRM_REQUEST_DP`
warning box) noting that `PrmEntryContainer`'s default priority is
`Svc::PrmDbCfg::DEFAULT_DP_PRIORITY`, a compile-time constant in `config/PrmDbCfg.fpp` that
projects can adjust — matching how `Svc::FileManager`'s SDD documents its analogous
`DEFAULT_DP_PRIORITY`.

### 11. `configureDp()` doc comment omits its asserted precondition (suggestion, not promoted) — FIXED

**File:** `Svc/PrmDb/PrmDbImpl.hpp` (lines 70-77)

**Was:** The Doxygen comment for `configureDp(FwSizeType bufferSize)` documents what the call does
but not that it asserts `bufferSize > Fw::DpContainer::MIN_PACKET_SIZE` (see
`PrmDbImpl.cpp:59`). Not promoted since this is a doc omission, not a false claim, and the assert
only fires at setup time with a misconfigured constant.

**Fix applied:** Added "\\note Asserts bufferSize > Fw::DpContainer::MIN_PACKET_SIZE." to the
comment.

### 12. Per-entry lock released by hand on two exit paths (could fix, not promoted) — FIXED

**File:** `Svc/PrmDb/PrmDbImpl.cpp` (`writeSaveFileChunk`, `PRM_SAVE_FILE_cmdHandler`),
`Svc/PrmDb/PrmDbImpl.hpp`

**Was:** `writeSaveFileChunk` called `this->unLock()` itself on each of its two failure paths,
relying on the caller (`PRM_SAVE_FILE_cmdHandler`'s per-entry loop) having the lock held at call
time, and also called `log_WARNING_HI_PrmFileWriteError`/`cmdResponse_out` directly from inside
the helper. Flagged as a hazard only for a future edit that adds another call site without
matching the convention — not a bug today, since both existing paths are correct.

**Fix applied:** `writeSaveFileChunk` no longer touches the lock or calls any port. It now only
reports success/failure plus the failure's `PrmWriteError` stage and error value via output
parameters. `PRM_SAVE_FILE_cmdHandler`'s loop breaks (instead of returning) on a failure, so the
loop's single `unLock()` call after the loop always runs, then the handler emits
`PrmFileWriteError` and responds `EXECUTION_ERROR` itself. This preserves the existing lock
scope exactly — the lock is still released before any port call, matching the discipline from
finding #1 — while centralizing the lock/unlock pairing at one call site instead of splitting it
across the helper and loop.

Note: an earlier version of this fix would have left `writeSaveFileChunk` emitting the event and
calling `cmdResponse_out` itself, with the caller's `unLock()` only happening after the loop broke
— that would have *widened* the lock to cover those two port calls, undoing finding #1. The
applied fix avoids that by making the helper lock-agnostic and port-call-agnostic entirely.

### 13. Container-allocation block duplicated (could fix, not promoted) — FIXED

**File:** `Svc/PrmDb/PrmDbImpl.cpp` (`PRM_REQUEST_DP_cmdHandler`, lines 448-459 and 466-476),
`Svc/PrmDb/PrmDbImpl.hpp`

**Was:** The "allocate a container, log `PrmDpBufferFailed` with the right stage on failure, mark
`haveContainer`/increment `numContainers` on success" block appears twice — once for the first
container, once for a replacement container after the current one fills — identical apart from
the `PrmDpBufferFailStage` value passed in.

**Fix applied:** Factored a private helper, `bool allocateDpContainer(DpContainer& container,
PrmDb_PrmDpBufferFailStage stage)`, that does the `dpGet_PrmEntryContainer` call and emits
`PrmDpBufferFailed` with the given stage on failure. Both call sites now just check the helper's
return value and handle `haveContainer`/`numContainers` bookkeeping themselves.

### 14. `//!<` doc comment placed before its member instead of after (suggestion, not promoted) — FIXED

**File:** `Svc/PrmDb/PrmDbImpl.hpp` (lines 111-113)

**Was:**
```cpp
//!< Maximum data product payload size per container, derived from configureDp()'s bufferSize.
//!< Zero (the default) means data product generation has not been configured.
FwSizeType m_dpMaxDataSize = 0;
```
`//!<` is the Doxygen marker for a comment that documents the *previous* declaration; placed
*before* the member it looks like a dangling trailing-comment marker rather than a leading one.
Framed as a comment-form convention issue, not a false documented claim.

**Fix applied:** Changed the two leading lines to plain `//` (non-doc) comments, which keep the
fuller explanation (derivation from `configureDp()`, zero meaning unconfigured) without being
mistaken for Doxygen markup. Added a brief `//!<` trailing doc comment on the declaration line
itself, matching the trailing-`//!<` style used elsewhere in this class (e.g. `m_fileName`,
`m_activeDb`):
```cpp
// Maximum data product payload size per container, derived from configureDp()'s bufferSize.
// Zero (the default) means data product generation has not been configured.
FwSizeType m_dpMaxDataSize = 0;  //!< Maximum DP payload size per container; 0 = not configured
```

### 15. `PrmDpNotConfigured` (id 18) declared out of id order (could fix, not promoted)

**File:** `Svc/PrmDb/PrmDbEventDict.fppi` (lines 154-159, between `PrmDpBufferFailed` id 15 and
`PrmDpRecordTooLarge` id 16 / `PrmDpComplete` id 17)

**Was:** Event ids 0-14 are declared in increasing order; `PrmDpBufferFailed` is 15, then
`PrmDpNotConfigured` is declared next but is id **18**, followed by `PrmDpRecordTooLarge` (16) and
`PrmDpComplete` (17) declared *after* it. No id collision exists today; the concern is only that
the gap (16, 17 declared after 18) could tempt a future editor to reuse 18 for a new event without
noticing it's already taken.

**Proposed fix:** Do **not** renumber `PrmDpNotConfigured` — its id is part of the wire-level event
dictionary and changing it is a compatibility break for no benefit. Instead, add a short comment
above it noting it was assigned id 18 after ids 16-17 were already in use by events declared later
in the file, so the declaration order and numeric id order intentionally differ.

### 16. `productGetOut` container ID never asserted in tests (suggestion, not promoted — test gap) — FIXED

**File:** `Svc/PrmDb/test/ut/PrmDbTester.cpp` (`productGet_handler`, lines 1545-1557)

**Was:** `productGet_handler(FwDpIdType id, FwSizeType dataSize, Fw::Buffer& buffer)` records the
call and checks `dataSize`, but never checks that `id` is the expected
`PrmDbImpl::ContainerId::PrmEntryContainer`. A regression that requested the wrong container type
would go undetected.

**Fix applied:** Added `EXPECT_EQ(id, PrmDbImpl::ContainerId::PrmEntryContainer);` at the top of
`productGet_handler`.

### 17. `PrmDpRecordTooLarge` throttle behavior untested (suggestion, not promoted — test gap) — FIXED

**File:** `Svc/PrmDb/test/ut/PrmDbTester.{hpp,cpp}`, `Svc/PrmDb/test/ut/PrmDbTestMain.cpp`

**Was:** The existing test (`PrmRequestDpRecordTooLarge`) triggers the event exactly once. The
`throttle 5` behavior — that a 6th+ occurrence in the same command is suppressed — is never
exercised.

**Fix applied:** Added `runPrmRequestDpRecordTooLargeThrottle()`, a new test with 6 oversized
entries in range. Asserts `ASSERT_EVENTS_PrmDpRecordTooLarge_SIZE(5)` (the throttle cap) while
`PrmDpComplete`'s new `numDropped` argument (see item 21) still reports all 6 drops, confirming the
event throttle doesn't affect the drop count itself. Wired up as
`ParameterDbTest.PrmRequestDpRecordTooLargeThrottle` in `PrmDbTestMain.cpp`.

### 18. Unconnected-port branches of `PrmDpNotConfigured` untested (suggestion, not promoted — test gap) — FIXED

**File:** `Svc/PrmDb/test/ut/PrmDbTester.{hpp,cpp}`, `Svc/PrmDb/test/ut/PrmDbTestMain.cpp`

**Was:** The precondition check is `!isConnected_productGetOut_OutputPort(0) ||
!isConnected_productSendOut_OutputPort(0) || (m_dpMaxDataSize == 0)`, but the only existing test
(`PrmRequestDpNotConfigured`) exercises just the third disjunct (`configureDp()` never called).
The two unconnected-port branches have no test coverage.

**Fix applied:** Gave `connectPorts()` in `PrmDbTestMain.cpp` two optional bool parameters
(`connectProductGetOut`, `connectProductSendOut`, both defaulting to `true`) to selectively skip
connecting one data-product port. Added two new tests,
`runPrmRequestDpNotConfiguredProductGetOutUnconnected` and
`runPrmRequestDpNotConfiguredProductSendOutUnconnected`, each calling `configureDp()` but leaving
one port disconnected, asserting the same `EXECUTION_ERROR` / `PrmDpNotConfigured` outcome.

### 19. Eight near-identical `TEST` bodies for `PRM_REQUEST_DP` (could fix, not promoted)

**File:** `Svc/PrmDb/test/ut/PrmDbTestMain.cpp` (lines 317-452, the eight `PrmRequestDp*` `TEST`
blocks)

**Was:** Each of the eight `PRM_REQUEST_DP` `TEST()` blocks repeats the identical five lines
(`impl.init`, `impl.configure`, `impl.configureSandbox`, `tester.init`, `connectPorts`) before
calling its own `run*()` method. This matches the convention used by every other `TEST()` in this
file, so it's a pre-existing pattern, not something introduced uniquely by these eight.

**Proposed fix:** Since the duplication is file-wide and not specific to the DP tests, a narrow
fix just for these eight would be an inconsistent abstraction. If this is worth addressing, do it
file-wide with a single helper, e.g. `Svc::PrmDbTester makeConnectedTester(Svc::PrmDbImpl& impl)`
that performs the five setup lines and returns the constructed, connected tester — applied to all
22 `TEST()` blocks, not just the DP ones. Given the low severity, recommend deferring this as a
separate cleanup rather than bundling it into the DP feature PR.

### 20. SDD omits the downstream pool/queue budget implied by a multi-container snapshot (suggestion, not promoted)

**File:** `Svc/PrmDb/docs/sdd.md` (§3.3.4)

**Was:** A single `PRM_REQUEST_DP` command can send an arbitrary number of containers
(`numContainers` in `PrmDpComplete` is unbounded by the SDD). The SDD doesn't say anything about
the buffer-pool or downstream-queue depth a deployment needs to provision to absorb a worst-case
snapshot in one command; the reviewer notes stock project sizings happen to stay within budget, so
this is only a documentation gap, not a bug.

**Proposed fix:** Add a sentence to SDD §3.3.4 noting that each container is requested and sent
synchronously, one at a time, so the only pool/queue depth a deployment must provision is for a
*single* in-flight container (not the full snapshot) — the bound that actually matters is the
`Svc::BufferManager` bin size passed to `configureDp()`, already covered by the existing warning
box.

### 21. Dropped entries become invisible beyond the 5th `PrmDpRecordTooLarge` warning (could fix, not promoted) — FIXED

**File:** `Svc/PrmDb/PrmDbEventDict.fppi` (`PrmDpComplete`), `Svc/PrmDb/PrmDbImpl.cpp`
(`PRM_REQUEST_DP_cmdHandler`), `Svc/PrmDb/docs/sdd.md`, `Svc/PrmDb/test/ut/PrmDbTester.cpp`

**Was:** `PrmDpRecordTooLarge` is throttled to 5 occurrences per the framework's standard event
throttle. A range with many oversized entries silently drops the 6th+ one with no event at all,
and `PrmDpComplete` reports `numRecords`/`numContainers` but not how many entries were dropped —
so an operator can't tell from telemetry alone how many entries were skipped. Not promoted because
this is a by-design throttle on a warning, not a success status being misreported (the command
still correctly reports `OK` for what *was* sent).

**Fix applied:** Added a `numDropped: U32` argument to `PrmDpComplete`, incremented once per
too-large-for-container drop in the handler's loop (independent of, and unaffected by, the
`PrmDpRecordTooLarge` event's own throttle), so the total dropped count is always visible even
once the per-event throttle kicks in. Updated the SDD (§3.2 and §3.3.4) and all existing
`ASSERT_EVENTS_PrmDpComplete` call sites in the tester for the new argument. Covered by the new
throttle test in item 17.
