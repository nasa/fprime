/**
 * @name Floating-point division by unguarded divisor
 * @description A floating-point division whose divisor is a variable should
 *              be preceded by a check that the divisor is not zero or near
 *              zero (a condition or FW_ASSERT referencing the divisor).
 *              IEEE 754 defines the result (inf/NaN), but inf/NaN propagate
 *              silently into flight computations. Integer division is covered
 *              by cpp/fprime/division-guard.
 * @kind problem
 * @id cpp/fprime/float-division-guard
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeGuards

Expr divisorOf(Operation op) {
  result = op.(DivExpr).getRightOperand() or
  result = op.(AssignDivExpr).getRValue()
}

from Operation op, VariableAccess divisor, Variable v
where
  divisor = divisorOf(op) and
  op.fromSource() and
  divisor.getTarget() = v and
  not exists(divisor.getValue()) and
  divisor.getFullyConverted().getUnspecifiedType() instanceof FloatingPointType and
  not guardedBefore(v, op) and
  not (v.isConst() and v.getInitializer().getExpr().getValue().toFloat() != 0)
select op,
  "The floating-point divisor " + v.getName() +
    " is not checked against zero (or a minimum magnitude) before this division."
