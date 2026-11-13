---
name: fprime-checklist-implementation
description: >-
  Implementation-stage pass of the F Prime Component Development Checklist.
  Run after the C++ implementation and unit tests exist: re-runs
  fprime-auto-checklist against the previous checklist, decides the
  Implementation (section 3) and Unit Testing (section 4) review rows, and
  re-decides any earlier decision the tool flags STALE because the SDD or
  model changed since. Keywords: F Prime, checklist, implementation stage,
  unit test, ISR, units in names, boundary tests, fprime-auto-checklist.
---

# Checklist — implementation stage

Updates the component's `docs/checklist.md` after the implementation and unit
tests are written. The tool recomputes every tool-decided row (now with the
code present), carries the design-stage decisions, and queues what this stage
must decide. It performs no development step: unit-test runs, coverage and
leak checks are read from what `fprime-util check --coverage` already left
behind (`coverage/summary.txt`), never produced here.

## Procedure

0. The tool is [fprime-auto-checklist](https://github.com/LeStarch/fprime-auto-checklist)
   (`pip install git+https://github.com/LeStarch/fprime-auto-checklist`; needs
   `clang-tidy` for `--analyzer clang-tidy`). Run it from the project root.

1. Run against the previous checklist:

   ```bash
   fprime-auto-checklist run --module <Path/To/Component> --analyzer clang-tidy \
       --stage implementation --previous <Path/To/Component>/docs/checklist.md \
       -o <Path/To/Component>/docs/checklist.md
   ```

   The queue (`Hand-off` section, `checklist.json` → `queue`) contains:
   - `review`: section-3/4 rows the tool leaves to a reviewer — 3.21 (CL-1 ISR
     state before enable), 3.23 (CL-2 interrupt enables cleared at init), 3.33
     (CL-3 units in names), 3.38 (CL-4 implementation matches requirements and
     design), 4.3 (CL-5 boundary tests);
   - `stale`: design-stage decisions whose inputs changed — the SDD (section 1)
     or the SDD/FPP model (section 2) differ from what was reviewed. Their
     status is kept but flagged `STALE:`; re-decide them against the current
     text.

2. Apply each rule verbatim against the code (`*.cpp`/`*.hpp`), the FPP model
   and `test/ut/`. For CL-4 walk the requirements table row by row and name
   the handler that satisfies each; for CL-5 list the bounded quantities
   (array sizes, `FW_ASSERT` limits, config constants, argument ranges) and the
   test that exercises the limit and one past it.

3. Record with `decide` (`--status YES|NO|N/A`, `--note` citing file:line,
   requirement id or test name; `--from` for a batch). Stale rows are recorded
   the same way; the new decision replaces the old one and is stamped with the
   current fingerprint.

4. Re-run step 1 and confirm *Nothing to review at the implementation stage*.
   Report the totals line, every NO with its note, and the UNCHECKED rows that
   wait on a process step not yet performed (coverage, leak check, topology
   build, CI) — say which step, do not perform it.

## Rules

- Decide only queued rows; tool-decided rows are audited at close-out.
- A `STALE` row is re-read in full, not re-confirmed from its old note.
- Never run `fprime-util check`, coverage, topology builds or hardware steps
  from this skill; `--build` is reserved for an operator who asks for it.
