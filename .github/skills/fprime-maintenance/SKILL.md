---
name: fprime-maintenance
description: >-
  Use when modifying pre-existing F Prime code — bug fixes, small
  behavior changes, API or dependency updates, review follow-ups, or
  cleanup — as opposed to creating new components, topologies, or
  build modules. Applies the doctrine of minimal effect: make the
  smallest change that completes the task, take only low-hanging
  technical-debt cleanup along the way, and obtain engineer approval
  before any rework of design or architecture. Keywords: F Prime,
  maintenance, bug fix, minimal change, scope, technical debt,
  refactor, rework.
---

# Skill: F Prime maintenance — the doctrine of minimal effect

When maintaining existing code the goal is the **minimal change
necessary to complete the task**. Focus explicitly on the task at hand
and on low-hanging-fruit technical-debt cleanup. Do not rework the
design, the architecture, or anything else. If a larger rework appears
necessary (for example, to avoid adding technical debt), propose it and
obtain an engineer's approval before doing it.

Creating new components, topologies, or build modules is not
maintenance; follow `.github/agents/fprime-development.agent.md` for
that work. This skill applies alongside the phase skills whenever the
task touches code that already exists.

---

## 1. Scope the task before touching code

- State the task in one sentence: the defect, behavior change, or
  update, and its acceptance criterion (the test that fails today, the
  warning that must go away, the API that must be adopted).
- Identify the smallest set of files and symbols that must change to
  meet it. That set is the scope; everything else is out of scope
  unless §3 or §4 admits it.
- Read the surrounding code and the component `docs/sdd.md` first. The
  change must fit the existing design, not the design you would have
  chosen.

## 2. Make the minimal change

- Change only what the task requires. Preserve existing interfaces
  (ports, commands, events, telemetry, parameters, public signatures),
  file layout, naming, and all behavior outside the task.
- Follow the surrounding conventions even where they differ from newer
  code elsewhere. `fprime-cpp-design` still governs every line you
  write.
- Do not rename, reorder, move, or reformat code the task does not
  touch. Run `clang-format` on changed files only.
- Do not modernize, generalize, or add abstraction, configuration, or
  features "while you are here".
- Update only the artifacts the change invalidates: unit tests for the
  changed behavior, the component `docs/sdd.md`, and the `docs/` pages
  that describe it.
- Fit the tests to the change: a regression test for the defect and one
  test per behavior the task added or changed. Do not add tests for
  behavior the task did not touch; record coverage gaps as future work
  (§5). Once the new tests pass, run the consolidation step in
  `fprime-unit-testing` §1 so shared sequences become helpers or rules.
- Keep the diff reviewable: every hunk must map to the task statement
  or to a cleanup item declared under §3.

## 3. Low-hanging fruit — permitted cleanup

Technical-debt cleanup is permitted only when it is **adjacent** to
the task (a function or file you are already editing), **mechanical
and behavior-preserving**, and small enough to verify by inspection.
Examples:

- a typo, stale comment, or outdated doc line the change made you read
- a magic literal on a touched line replaced with the existing named
  constant
- dead code or an unused include the change makes obviously dead
- a missing `override`, `const`, `nullptr`, or fixed-width type on a
  touched declaration
- an unchecked return code on a call you are already modifying

Anything larger is not low-hanging fruit, however beneficial. List
each cleanup item in the PR description so reviewers can separate it
from the task itself.

## 4. Rework requires engineer approval

Sometimes the minimal change would add technical debt, or the task
cannot be completed within the existing design. Do not decide this
alone:

1. Stop before starting the rework. Leave the minimal change, if one
   exists, in a clearly described state.
2. Write a short proposal: why the minimal approach is inadequate, the
   rework and its extent (files, interfaces, behavior affected),
   alternatives considered, and the risk.
3. Present it to the responsible engineer — the requesting user, the
   component maintainer (`maintainer-lookup`), or an issue for CCB
   review per [`GOVERNANCE.md`](../../../GOVERNANCE.md) — and **wait
   for explicit approval**. Silence is not approval.
4. Once approved, deliver the rework separately from the fix (its own
   PR, or clearly separated commits) so each can be reviewed and
   reverted independently.

Rework includes changing an FPP interface, restructuring a component
or its state, changing a framework type or OSAL contract, moving code
across modules, wide renames, replacing an algorithm or data
structure, and any change whose footprint exceeds the scope set in §1.

## 5. Out-of-scope findings

Record defects, debt, or design concerns you notice but do not fix
under §3 — in an issue, or under "Future work" in the PR description —
rather than fixing them silently. This mirrors the reviewer-side rule
in `pr-diff-scoping`: preexisting issues are **future work**, not part
of the current change.

## 6. Checklist before opening the PR

- [ ] Every hunk maps to the task statement or a declared §3 cleanup.
- [ ] No interface, layout, or behavior changed beyond the task.
- [ ] Any rework was approved by an engineer beforehand and is
      separated from the fix.
- [ ] Tests, `docs/sdd.md`, and `docs/` updated only where the change
      invalidated them; new tests trace to the task and were
      consolidated.
- [ ] Out-of-scope observations recorded, not fixed.
- [ ] PR description distinguishes the fix, declared cleanup, and
      future work.
