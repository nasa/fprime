/**
 * @name Constant assertion
 * @description Assertions should check dynamic properties of pre-/post-conditions and invariants. Assertions that either always succeed or always fail are an error.
 * @kind problem
 * @id cpp/fprime/jpl-c/use-of-assertions-constant
 * @problem.severity warning
 * @tags maintainability
 *       reliability
 *       external/jpl
 */

import FprimeAssertions

from Assertion a, string value, string msg
where
  value = a.getAsserted().getValue() and
  if value.toInt() = 0
  then msg = "This assertion is always false."
  else msg = "This assertion is always true."
select a.getAsserted(), msg
