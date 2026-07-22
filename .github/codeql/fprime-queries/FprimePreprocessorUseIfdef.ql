/**
 * @name Conditional compilation
 * @description The use of conditional compilation directives must be kept to a minimum -- e.g. for header guards only.
 * @kind problem
 * @id cpp/fprime/jpl-c/preprocessor-use-ifdef
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/**
 * F Prime refinement of the JPL Rule 20 query (cpp/jpl-c/preprocessor-use-ifdef).
 *
 * The OS abstraction layers (Os/Posix, Os/Linux, Os/Darwin) must probe what
 * the target platform provides (e.g. `#ifdef _SC_PAGESIZE`, `#if
 * defined(O_SYNC)`, `#ifdef B3000000`), so feature detection of macros that
 * are not defined by this repository is permitted there. Everywhere else,
 * and for any repository-defined macro, conditional compilation is flagged.
 */

/** Holds if a macro with the given name is defined in repository sources. */
predicate repoDefinedMacro(string name) {
  exists(Macro m | m.getName() = name and exists(m.getFile().getRelativePath()))
}

/** An identifier appearing in the directive's condition text. */
string conditionIdentifier(PreprocessorBranch b) {
  result = b.getHead().regexpFind("[A-Za-z_][A-Za-z0-9_]*", _, _) and
  result != "defined"
}

/** Holds if the directive only tests macros not defined in this repository. */
predicate externalFeatureDetection(PreprocessorBranch b) {
  exists(conditionIdentifier(b)) and
  not repoDefinedMacro(conditionIdentifier(b))
}

/** Holds if the file is part of an OS platform abstraction layer. */
predicate inOsPlatformLayer(File f) {
  exists(string p | p = f.getRelativePath() |
    p.matches("Os/Posix/%") or
    p.matches("Os/Linux/%") or
    p.matches("Os/Darwin/%")
  )
}

from PreprocessorBranch i
where
  (i instanceof PreprocessorIf or i instanceof PreprocessorIfdef or i instanceof PreprocessorIfndef) and
  not i.getFile() instanceof HeaderFile and
  not (inOsPlatformLayer(i.getFile()) and externalFeatureDetection(i))
select i, "Use of conditional compilation must be kept to a minimum."
