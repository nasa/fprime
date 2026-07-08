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
  // FW_ASSERT(0) is an allowed idiom for asserting an unreachable code path,
  // so a constant value of exactly 0 is not flagged.
  not value.toInt() = 0 and
  msg = "This assertion is always true."
select a.getAsserted(), msg
