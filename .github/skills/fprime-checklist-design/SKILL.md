---
name: fprime-checklist-design
description: >-
  Design-stage pass of the F Prime Component Development Checklist. Run after
  the SDD and FPP model are approved and before implementation: creates the
  checklist with fprime-auto-checklist, decides the Requirements (section 1)
  and Design (section 2) review rows from the SDD and model, records each
  decision with `decide`, and leaves the tool-decided rows alone. Keywords:
  F Prime, checklist, design stage, requirements review, SDD, FPP,
  fprime-auto-checklist.
---

# Checklist — design stage

Produces `docs/checklist.md` (+ `docs/checklist.json`) for a component whose
SDD and FPP model have just been approved. The tool decides everything it can;
this skill decides the rows the tool queues for the **design** stage and
nothing else. It performs no development step: it does not build unit tests,
run coverage, or touch the component.

## Procedure

0. The tool is [fprime-auto-checklist](https://github.com/LeStarch/fprime-auto-checklist)
   (`pip install git+https://github.com/LeStarch/fprime-auto-checklist`; needs
   `clang-tidy` for `--analyzer clang-tidy`). Run it from the project root.

1. Run the tool from the project root (it builds the module once for the
   clang-tidy analysis; that is the only build it performs):

   ```bash
   fprime-auto-checklist run --module <Path/To/Component> --analyzer clang-tidy \
       --stage design -o <Path/To/Component>/docs/checklist.md
   ```

   The Hand-off section and `checklist.json` → `queue` list the rows to decide,
   each with its review rule (`rule_text`). At this stage the queue is the
   section-1 rows 1.1–1.6 (rule CL-6) plus any section-2 rows the tool left at
   REVIEW. Rows of sections 3–5 are marked *deferred*; leave them.

2. For each queued row, read the SDD requirements table and the FPP model and
   apply the rule verbatim. For CL-6 (1.1–1.6): list every requirement, decide
   whether the aspect the item names (behaviour, input data, output data,
   memory, hardware I/O, runtime performance) is constrained by at least one of
   them, or is declared not applicable with a rationale in the SDD. Do not
   infer a convention (no `Kind` column, no keyword table) — judge the text.

3. Record every decision through the tool, never by editing the Markdown:

   ```bash
   fprime-auto-checklist decide --checklist <...>/docs/checklist.md \
       --item 1.4 --status YES --note "REQ-003/REQ-007 bound all buffers; no heap after init (SDD §3.4)."
   ```

   `--status` is YES, NO or N/A; NO and N/A take a one-line `--note`
   (see Notes discipline). `--from decisions.json` records a batch
   (`[{"id","status","notes"}]`). The tool stamps each decision with the
   commit and the section-1/2 fingerprint so later stages know when the SDD
   or model changed under it.

4. Link the checklist from the SDD: its very last line is
   `Component Development Checklist: [checklist.md](checklist.md)` — append it once, after Tests,
   nothing after it:

   ```bash
   grep -qxF 'Component Development Checklist: [checklist.md](checklist.md)' <Path/To/Component>/docs/sdd.md || \
       printf '\nComponent Development Checklist: [checklist.md](checklist.md)\n' >> <Path/To/Component>/docs/sdd.md
   ```

5. Re-run step 1 with `--previous <...>/docs/checklist.md` and confirm the
   Hand-off section says *Nothing to review at the design stage*. Report the
   totals line and every row that is NO, with its note, to the user; tool NOs
   (e.g. a missing SDD table) are fixed in the SDD/model, not in the checklist.

## Notes discipline

The rendered table is read by people; keep it sparse.

- **YES**: no note. The tool's own rule pointer (`R8 pass`, `` `cpp/fprime/no-printf` 0 findings ``)
  is the only text a YES row carries; your `--note` (one clause, the ids you
  checked) is stored in the JSON for traceability and is not rendered.
- **NO**: one line, what is wrong and where — `file:line`, requirement id or
  table — and the fix in a few words. No requirement-by-requirement walks, no
  restating the tool's finding, no "Confirmed:" prefixes.
- **N/A**: one clause stating why the item does not apply.
- **UNCHECKED**: name the process step that has not been performed.
- Audits that confirm the tool's verdict are silent in the table; an override
  renders as one `Audit (...)` line from your `--note`.

## Rules

- Decide only the rows in the queue. A tool-decided row is audited at
  close-out, not here.
- Every NO/N/A note names the requirement id(s) or SDD section it rests on.
- Never run `fprime-util check`, coverage, topology builds or hardware steps
  to make a row pass; rows that need them stay UNCHECKED "not done yet" until
  the process performs them.
