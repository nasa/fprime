/**
 * @name Assertion with side effects
 * @description Assertions should not have side-effects -- they may be disabled completely, changing program behavior.
 * @kind problem
 * @id cpp/fprime/jpl-c/use-of-assertions-side-effect
 * @problem.severity warning
 * @tags correctness
 *       external/jpl
 */

import cpp
import FprimeAssertions

/**
 * Holds if calling `call` may change program state.
 *
 * Calls to `const` member functions are treated as side-effect-free queries.
 * F Prime assertions routinely call `const` accessors (e.g. the autocoder's
 * `isConnected_*_OutputPort()` and `getDeserializeSizeLeft()`), which CodeQL's
 * default purity analysis conservatively reports as impure because they are
 * virtual or because their own body contains an `FW_ASSERT`. Disabling such an
 * assertion does not change observable object state, so those calls are not
 * real side effects and should not be flagged.
 */
predicate callMayHaveSideEffect(FunctionCall call) {
  not call.getTarget() instanceof ConstMemberFunction and
  (
    call.getTarget().mayHaveSideEffects()
    or
    call.isVirtual()
  )
}

/**
 * Holds if evaluating `e` itself (ignoring its sub-expressions) performs a
 * side effect: a write (assignment, increment/decrement), an allocation, a
 * `throw`, or a call to a function that may have side effects.
 */
predicate isSideEffecting(Expr e) {
  e instanceof Assignment
  or
  e instanceof CrementOperation
  or
  e instanceof NewOrNewArrayExpr
  or
  e instanceof DeleteOrDeleteArrayExpr
  or
  e instanceof ThrowExpr
  or
  exists(FunctionCall call | call = e | callMayHaveSideEffect(call))
}

from Assertion a, Expr asserted
where
  asserted = a.getAsserted() and
  isSideEffecting(asserted.getAChild*())
select asserted, "Assertions should not have side effects."
