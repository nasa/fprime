/**
 * @name Multiple variable declarations on one line
 * @description There should be no more than one variable declaration per line.
 * @kind problem
 * @id cpp/fprime/jpl-c/multiple-var-decls-per-line
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/*
 * F Prime refinement of the JPL Rule 24 query (cpp/jpl-c/multiple-var-decls-per-line).
 *
 * The stock query was written for C and misfires on C++ range-based for loops
 * (`for (const auto& x : container)`): the compiler desugars them into a
 * declaration statement containing several compiler-generated variables
 * (`__range`, `__begin`, `__end`) that all carry the location of the loop
 * header, so the single variable the programmer wrote is falsely reported as
 * multiple declarations on one line. Compiler-generated variables are not
 * counted here.
 */
from DeclStmt d
where
  exists(Variable v1, Variable v2 | v1 = d.getADeclaration() and v2 = d.getADeclaration() |
    v1 != v2 and
    not v1.isCompilerGenerated() and
    not v2.isCompilerGenerated() and
    v1.getLocation().getStartLine() = v2.getLocation().getStartLine()
  )
select d, "Multiple variable declarations on the same line."
