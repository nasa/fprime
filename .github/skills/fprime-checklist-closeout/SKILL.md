---
name: fprime-checklist-closeout
description: >-
  Close-out pass of the F Prime Component Development Checklist. Run after
  system testing / run-for-record: re-runs fprime-auto-checklist with
  --stage closeout, decides the Close-Out (section 5) review rows, re-decides
  anything STALE from earlier stages, and reads every NO/UNCHECKED row the
  tool decided, overriding only where the tool is wrong.
  Keywords: F Prime, checklist, close-out, override, hardware run, CI,
  fprime-auto-checklist.
---

# Checklist — close-out

Finishes the component's `docs/checklist.md`. At this stage every section is
due and the tool additionally queues its own NO / UNCHECKED rows to **read**:
a reviewer opens the evidence behind the verdict and overrides it only when
the tool is wrong (a tool YES is accepted as is). The skill performs no development step: whether the component
was built into a topology, run on hardware, or passed CI is answered from the
artefacts the process left (`docs/hardware-run.md`, the CI status via
`GITHUB_TOKEN`, deployment build caches), truthfully — "not done" stays
UNCHECKED, it is never argued into a YES.

## Procedure

0. Install the tool from a clone (the clang-tidy plugin is built from the
   clone, it is not in the pip package); without the plugin every
   static-analysis row (31 of them) comes back UNCHECKED, so do not skip it:

   ```bash
   git clone https://github.com/LeStarch/fprime-auto-checklist && cd fprime-auto-checklist
   pip install -e .
   sudo apt-get install -y clang-tidy-15 llvm-15-dev libclang-15-dev   # LLVM 15 (plugin and clang-tidy must match)
   cmake -S native -B native/build -DClang_DIR=/usr/lib/llvm-15/lib/cmake/clang && cmake --build native/build
   cd -   # back to the project root; run the tool from there
   ```

   A prebuilt plugin is picked up from `$FPRIME_TIDY_PLUGIN` (`--tidy-plugin`),
   `clang-tidy` from `$CLANG_TIDY` (`--clang-tidy`).

1. Run against the previous checklist:

   ```bash
   fprime-auto-checklist run --module <Path/To/Component> --analyzer clang-tidy \
       --stage closeout --previous <Path/To/Component>/docs/checklist.md \
       -o <Path/To/Component>/docs/checklist.md
   ```

   (Add `--repo owner/name` with `GITHUB_TOKEN` set for 5.5; `--artifacts-dir`
   for 4.1 when the process published unit-test artifacts.)

   The queue contains `review` rows (section 5 and anything left undecided),
   `stale` rows (earlier decisions whose section inputs changed) and `read`
   rows — the tool's NO / UNCHECKED rows.

2. Decide the `review` and `stale` rows as in the earlier stages (rule text is
   in the queue). 5.3 is the hardware run: YES only when `docs/hardware-run.md`
   records platform, deployment, commit, date, result and tester for the
   current commit (check C2 validates it); otherwise NO with "not run" — never
   infer a run from a build.

3. Read every `read` row: open the evidence the Notes cite (the clang-tidy/CodeQL
   finding and file:line, the check's detail, the SDD table, the coverage
   figure). A correct verdict needs nothing — no stamp, no note. Where the tool
   is wrong, `decide --item <id> --status <status> --note "<why>"` renders
   `<status> @<commit> overridden by reviewer` with your one-clause reason in
   Notes pointing at the evidence (a false positive's file:line, the artefact
   the tool missed). An UNCHECKED row is overridden to YES/NO only with an
   artefact in hand; otherwise it stays and the report names the process step
   it waits on. Use `--from decisions.json` for a batch.

4. Re-run step 1 and confirm zero REVIEW and zero `stale` rows (the `read`
   list remains: it is the sign-off list). Report the totals, the NO rows and
   whether each is pre-existing or introduced, and the UNCHECKED rows with the
   process step each waits on. Commit `docs/checklist.md` with the component —
   it is the only record (no `checklist.json`; the evidence JSON stays in the
   tool's work dir).

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
- Every Status cell carries its stamp: `YES @<commit>` (tool), `… by review`
  (your decision on a row the tool left open), `… overridden by reviewer`
  (you replaced the tool's verdict — one-clause reason in Notes).

## Rules
- `docs/checklist.md` stays beside `docs/sdd.md`; the SDD's last line is
  `Component Development Checklist: [checklist.md](checklist.md)` — restore it if an SDD edit dropped it.

- Reading a row does not re-run the analysis to make a finding disappear. Fix
  the code or the document and re-run the tool; an override lapses by itself
  when its section's inputs change or the tool comes to agree.
- Hardware, topology and CI rows are answered from records of what was done.
  If the process has not done it, say so.
- Never hand-edit the Markdown table; `decide` rewrites it.
