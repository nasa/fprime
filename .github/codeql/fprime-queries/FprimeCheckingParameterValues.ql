/**
 * @name Unchecked pointer or integer parameter
 * @description Pointer parameters should be checked before being dereferenced,
 *              and integer parameters should be checked before being used as
 *              an array index or divisor. A check is a use of the parameter in
 *              a condition or in an FW_ASSERT. Parameters of other types
 *              (objects, references, enums, floats) are presumed valid across
 *              their full range and are not flagged. This is an F Prime
 *              refinement of JPL Coding Standard Rule 15
 *              (cpp/jpl-c/checking-parameter-values).
 * @kind problem
 * @id cpp/fprime/checking-parameter-values
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeAssertions

/**
 * A parameter whose value cannot be presumed valid across its full type
 * range: a pointer. References are excluded because they cannot be null in
 * well-formed C++. Integer parameters are handled separately, as they only
 * require checking when used in range-sensitive contexts.
 */
predicate pointerParameter(Parameter p) { p.getUnspecifiedType() instanceof PointerType }

predicate integerParameter(Parameter p) { p.getUnspecifiedType() instanceof IntegralType }

/**
 * An access of `p` that constitutes a validity check: an occurrence in the
 * condition of a control-flow construct, or in the asserted expression of an
 * FW_ASSERT.
 */
predicate checkingAccess(Parameter p, VariableAccess va) {
  va = p.getAnAccess() and
  (
    exists(Expr cond | cond.isCondition() | cond.getAChild*() = va)
    or
    exists(FwAssert a | a.getAsserted().getAChild*() = va)
  )
}

/**
 * A use of `p` that requires a prior validity check: dereferencing a pointer
 * parameter, or using an integer parameter as an array index or divisor.
 * Merely passing the parameter to another function is not flagged; the callee
 * is responsible for checking its own parameters.
 */
predicate unsafeUse(Parameter p, VariableAccess va) {
  va = p.getAnAccess() and
  (
    pointerParameter(p) and
    (
      exists(PointerDereferenceExpr deref | deref.getOperand() = va)
      or
      exists(ArrayExpr ae | ae.getArrayBase() = va)
      or
      exists(FieldAccess fa | fa.getQualifier() = va)
      or
      exists(Call c | c.getQualifier() = va)
      or
      exists(ExprCall ec | ec.getExpr() = va)
      or
      exists(PointerArithmeticOperation op | op.getAnOperand() = va)
    )
    or
    integerParameter(p) and
    (
      exists(ArrayExpr ae | ae.getArrayOffset() = va)
      or
      exists(DivExpr d | d.getRightOperand() = va)
      or
      exists(RemExpr r | r.getRightOperand() = va)
    )
  )
}

/** Holds if `node` (re)assigns a new value to `p`, ending its caller-supplied value. */
predicate reassignment(Parameter p, ControlFlowNode node) {
  node.(Assignment).getLValue() = p.getAnAccess()
  or
  node.(CrementOperation).getOperand() = p.getAnAccess()
}

/**
 * Control-flow nodes reachable from the entry of `p`'s function without
 * passing through a check or a reassignment of `p`.
 */
predicate uncheckedFlow(Parameter p, ControlFlowNode n) {
  (pointerParameter(p) or integerParameter(p)) and
  unsafeUse(p, _) and
  n = p.getFunction().getBlock()
  or
  exists(ControlFlowNode mid |
    uncheckedFlow(p, mid) and
    not checkingAccess(p, mid) and
    not reassignment(p, mid) and
    n = mid.getASuccessor()
  )
}

string useDescription(Parameter p) {
  pointerParameter(p) and result = "dereferenced"
  or
  integerParameter(p) and result = "used as an array index or divisor"
}

from Parameter p, VariableAccess va
where
  unsafeUse(p, va) and
  uncheckedFlow(p, va) and
  not checkingAccess(p, va) and
  // Skip internal-linkage (file-scope static) functions; static member
  // functions are part of a class's API and are still checked.
  (p.getFunction() instanceof MemberFunction or not p.getFunction().isStatic())
select va,
  "Parameter " + p.getName() + " is " + useDescription(p) +
    " without first being checked (e.g. with FW_ASSERT or a condition)."
