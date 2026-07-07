/**
 * @name Non-boolean assertion
 * @description Assertions should be defined as Boolean tests, meaning "assert(p != NULL)" rather than "assert(p)".
 * @kind problem
 * @id cpp/fprime/jpl-c/use-of-assertions-non-boolean
 * @problem.severity warning
 * @tags correctness
 *       external/jpl
 */

import FprimeAssertions

from Assertion a
where a.getAsserted().getType() instanceof PointerType
select a.getAsserted(), "Assertions should be defined as Boolean tests."
