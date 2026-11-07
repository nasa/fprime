/**
 * @name Unguarded signed arithmetic that may overflow
 * @description Signed integer overflow is undefined behavior. Signed
 *              additions, subtractions, and multiplications whose range
 *              analysis cannot bound should be preceded by a check (condition
 *              or FW_ASSERT) on an operand, or performed in a wider or
 *              unsigned type.
 * @kind problem
 * @id cpp/fprime/signed-overflow
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis
import FprimeGuards

predicate guarded(BinaryArithmeticOperation op) {
  guardedBefore(op.getAnOperand().(VariableAccess).getTarget(), op)
}

from BinaryArithmeticOperation op, IntegralType t
where
  (op instanceof AddExpr or op instanceof SubExpr or op instanceof MulExpr) and
  op.fromSource() and
  t = op.getFullyConverted().getUnspecifiedType() and
  t.isSigned() and
  // Both operands variable: literal-and-variable cases (i + 1) are the
  // loop-counter idiom and are bounded by the loop condition when guarded.
  not op.getAnOperand() instanceof Literal and
  (exprMightOverflowPositively(op) or exprMightOverflowNegatively(op)) and
  not guarded(op) and
  not op.getFile().getAbsolutePath().matches(["%/test/%", "%/ut/%"])
select op,
  "Signed " + t.getName() + " " + op.getOperator() +
    " may overflow; its operands are not range-checked beforehand."
