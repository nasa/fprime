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

from Assertion a, Expr asserted, string value, string msg
where
  asserted = a.getAsserted() and
  value = asserted.getValue() and
  // A literal that is always false -- FW_ASSERT(0) or FW_ASSERT(false), since
  // `false` is a BoolLiteral with integer value 0 -- is an allowed idiom for
  // asserting an unreachable code path. A named constant or expression that
  // merely folds to 0 is still flagged, since that is more likely an
  // unintended always-false assertion.
  not (asserted instanceof Literal and value.toInt() = 0) and
  if value.toInt() = 0
  then msg = "This assertion is always false."
  else msg = "This assertion is always true."
select asserted, msg
