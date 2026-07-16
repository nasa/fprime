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
where
  not a.getAsserted().getUnspecifiedType() instanceof BoolType and
  // In the uninstantiated copy of a template, the type of an expression
  // involving template parameters is unresolved, so a boolean comparison such
  // as `status == Success::SUCCESS` would be misreported as non-boolean.
  // Instantiations of the template are still checked.
  not a.getAsserted().isFromUninstantiatedTemplate(_)
select a.getAsserted(), "Assertions should be defined as Boolean tests."
