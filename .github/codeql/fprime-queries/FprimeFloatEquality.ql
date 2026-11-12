/**
 * @name Floating-point equality comparison
 * @description Floating-point values should not be compared with `==` or
 *              `!=`: rounding makes exact equality unreliable. Compare
 *              against a tolerance instead. The self-comparison NaN idiom
 *              (`x != x`) is not flagged.
 * @kind problem
 * @id cpp/fprime/float-equality
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

from EqualityOperation eq
where
  eq.fromSource() and
  eq.getAnOperand().getFullyConverted().getUnspecifiedType() instanceof FloatingPointType and
  // NaN check: x != x
  not exists(Variable v |
    eq.getLeftOperand().(VariableAccess).getTarget() = v and
    eq.getRightOperand().(VariableAccess).getTarget() = v
  )
select eq,
  "Floating-point values are compared with " + eq.getOperator() +
    "; compare against a tolerance instead."
