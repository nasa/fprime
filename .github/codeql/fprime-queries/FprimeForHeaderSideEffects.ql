/**
 * @name Side effect in for-statement header unrelated to flow control
 * @description The init/condition/update expressions of a for statement
 *              should only concern loop flow control; calls and mutations of
 *              non-loop state belong in the loop body.
 * @kind problem
 * @id cpp/fprime/for-header-flow-control
 * @problem.severity warning
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/** A variable read in the loop's controlling condition. */
predicate conditionVariable(ForStmt f, Variable v) {
  v.getAnAccess().getParent*() = f.getCondition()
}

/** The variable mutated by expression `e` in the for header. */
Variable mutatedVariable(Expr e) {
  result = e.(Assignment).getLValue().(VariableAccess).getTarget()
  or
  result = e.(CrementOperation).getOperand().(VariableAccess).getTarget()
}

/** A call to an overloaded increment/decrement on a loop-control variable (e.g. `++it` on an iterator). */
predicate iteratorCrement(ForStmt f, FunctionCall fc) {
  fc.getTarget().getName() = ["operator++", "operator--"] and
  exists(Variable v | conditionVariable(f, v) |
    fc.getQualifier().(VariableAccess).getTarget() = v or
    fc.getArgument(0).(VariableAccess).getTarget() = v
  )
}

from ForStmt f, Expr offender, string reason
where
  f.fromSource() and
  (
    // A call in the init or update expression (I/O or other side effects)
    offender.getParent*() = [f.getInitialization().(ExprStmt).getExpr(), f.getUpdate()] and
    offender instanceof FunctionCall and
    not iteratorCrement(f, offender) and
    reason = "calls the function " + offender.(FunctionCall).getTarget().getName()
    or
    // Mutation of a variable that does not control the loop
    exists(Variable v |
      offender.getParent*() = [f.getInitialization().(ExprStmt).getExpr(), f.getUpdate()] and
      v = mutatedVariable(offender) and
      not conditionVariable(f, v) and
      reason = "modifies the non-loop-control variable " + v.getName()
    )
  )
select offender, "This for-statement header expression " + reason + "."
