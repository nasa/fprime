---
name: fprime-checklist-closeout
description: >-
  Close-out pass of the F Prime Component Development Checklist. Run after
  system testing / run-for-record: re-runs fprime-auto-checklist with
  --stage closeout, decides the Close-Out (section 5) review rows, re-decides
  anything STALE from earlier stages, and audits every tool-decided row
  (YES/NO/N/A/UNCHECKED) so the delivered checklist has no row nobody read.
  Keywords: F Prime, checklist, close-out, audit, hardware run, CI,
  fprime-auto-checklist.
---

# Checklist — close-out

Finishes the component's `docs/checklist.md`. At this stage every section is
due and the tool additionally queues each row it decided itself for an
**audit**: a reviewer reads the evidence behind the verdict and confirms or
overrides it. The skill performs no development step: whether the component
was built into a topology, run on hardware, or passed CI is answered from the
artefacts the process left (`docs/hardware-run.md`, the CI status via
`GITHUB_TOKEN`, deployment build caches), truthfully — "not done" stays
UNCHECKED, it is never argued into a YES.

## Procedure

0. The tool is [fprime-auto-checklist](https://github.com/LeStarch/fprime-auto-checklist)
   (`pip install git+https://github.com/LeStarch/fprime-auto-checklist`; needs
   `clang-tidy` for `--analyzer clang-tidy`). Run it from the project root.

1. Run against the previous checklist:

   ```bash
   fprime-auto-checklist run --module <Path/To/Component> --analyzer clang-tidy \
       --stage closeout --previous <Path/To/Component>/docs/checklist.md \
       -o <Path/To/Component>/docs/checklist.md
   ```

   (Add `--repo owner/name` with `GITHUB_TOKEN` set for 5.5; `--artifacts-dir`
   for 4.1 when the process published unit-test artifacts.)

   The queue contains `review` rows (section 5 and anything left undecided),
   `stale` rows (earlier decisions whose section inputs changed) and `audit`
   rows — every tool-decided row without a current audit.

2. Decide the `review` and `stale` rows as in the earlier stages (rule text is
   in the queue). 5.3 is the hardware run: YES only when `docs/hardware-run.md`
   records platform, deployment, commit, date, result and tester for the
   current commit (check C2 validates it); otherwise NO with "not run" — never
   infer a run from a build.

3. Audit every `audit` row. For each, open the evidence the Notes cite (the
   clang-tidy/CodeQL finding and file:line, the check's detail, the SDD table,
   the coverage figure) and record:
   - `decide --item <id> --status <same as tool> --note "<what you checked>"` —
     recorded as *Audited: confirmed*;
   - `decide --item <id> --status <different> --note "<why the tool is wrong>"`
     — recorded as *Audited: overridden*; the note must point at the evidence
     (a false positive's file:line, the artefact the tool missed).
   An UNCHECKED row is confirmed as UNCHECKED only by stating which process
   step has not been performed (`--status` is not needed for that: leave it and
   list it in the report); override it to YES/NO only with an artefact in hand.
   Use `--from decisions.json` for the batch.

4. Re-run step 1 and confirm *Nothing to review at the closeout stage*: zero
   REVIEW rows, every row carrying a decision or an audit stamp. Report the
   totals, the NO rows with notes, and the UNCHECKED rows with the process
   step each waits on. Commit `docs/checklist.md` and `docs/checklist.json`
   with the component.

## Rules

- An audit reads the evidence; it does not re-run the analysis to make a
  finding disappear. Fix the code or the document, re-run the tool, and the
  audit lapses by itself (the tool drops an audit whose verdict or section
  fingerprint changed).
- Hardware, topology and CI rows are answered from records of what was done.
  If the process has not done it, say so.
- Never hand-edit the Markdown table; `decide` re-renders it from the JSON.
