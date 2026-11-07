/**
 * Shared notion of a "guard": a use of a variable in a condition or in an
 * FW_ASSERT that is evaluated before a given expression on every path.
 */

import cpp
import semmle.code.cpp.controlflow.Dominance
import FprimeAssertions

/** An access of `v` that constitutes a check: in a condition or FW_ASSERT. */
predicate checkingAccess(Variable v, VariableAccess va) {
  va = v.getAnAccess() and
  (
    exists(Expr cond | cond.isCondition() | cond.getAChild*() = va)
    or
    exists(FwAssert a | a.getAsserted().getAChild*() = va)
  )
}

/**
 * Holds if `check` is evaluated before `e` on every path to `e`: either its
 * basic block strictly dominates that of `e`, or both are in the same block
 * and `check` comes first.
 */
predicate guardPrecedes(Expr check, Expr e) {
  check.getEnclosingFunction() = e.getEnclosingFunction() and
  (
    bbStrictlyDominates(check.getBasicBlock(), e.getBasicBlock())
    or
    exists(BasicBlock bb, int i, int j |
      bb.getNode(i) = check and bb.getNode(j) = e and i < j
    )
  )
}

/** Holds if some check of `v` precedes `e`. */
predicate guardedBefore(Variable v, Expr e) {
  exists(VariableAccess check | checkingAccess(v, check) and guardPrecedes(check, e))
}
