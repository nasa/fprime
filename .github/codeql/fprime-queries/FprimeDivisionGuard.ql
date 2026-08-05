/**
 * @name Division by unguarded divisor
 * @description Integer division and modulo operations whose divisor is a
 *              variable should be preceded by a check (condition or
 *              FW_ASSERT) that the divisor cannot be zero. Floating-point
 *              division is excluded: IEEE 754 defines division by zero
 *              (yielding infinity or NaN) and it is not undefined behavior.
 * @kind problem
 * @id cpp/fprime/division-guard
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import semmle.code.cpp.controlflow.Dominance
import FprimeAssertions

Expr divisorOf(Operation op) {
  result = op.(DivExpr).getRightOperand() or
  result = op.(RemExpr).getRightOperand() or
  result = op.(AssignDivExpr).getRValue() or
  result = op.(AssignRemExpr).getRValue()
}

/** An access of `v` that constitutes a zero check: in a condition or FW_ASSERT. */
predicate checkingAccess(Variable v, VariableAccess va) {
  va = v.getAnAccess() and
  (
    exists(Expr cond | cond.isCondition() | cond.getAChild*() = va)
    or
    exists(FwAssert a | a.getAsserted().getAChild*() = va)
  )
}

from Operation op, VariableAccess divisor, Variable v
where
  divisor = divisorOf(op) and
  op.fromSource() and
  divisor.getTarget() = v and
  not exists(divisor.getValue()) and
  // IEEE 754 floating-point division by zero is well-defined (inf/NaN)
  not divisor.getFullyConverted().getUnspecifiedType() instanceof FloatingPointType and
  // No check of the divisor dominates the division
  not exists(VariableAccess check |
    checkingAccess(v, check) and
    check.getEnclosingFunction() = op.getEnclosingFunction() and
    bbDominates(check.getBasicBlock(), op.getBasicBlock())
  ) and
  // Unsigned/enum constants assigned from a known nonzero literal are common;
  // only skip divisors that are const-qualified with a nonzero initializer
  not (v.isConst() and v.getInitializer().getExpr().getValue().toFloat() != 0)
select op,
  "The divisor " + v.getName() + " is not checked against zero before this " +
    op.getOperator() + " operation."
