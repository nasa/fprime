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
 * Holds if `f` is a non-const member function whose declaring type also
 * declares a `const` overload with the same name and arity (e.g.
 * `SerializeBufferBase::getBuffAddr()` or `ExternalArray::operator[]`).
 * Overload resolution picks the non-const overload on a non-const object,
 * but the const overload's existence shows the operation itself is a
 * side-effect-free query.
 */
predicate hasConstOverload(MemberFunction f) {
  exists(ConstMemberFunction c |
    (
      c.getDeclaringType() = f.getDeclaringType()
      or
      // In a class template instantiation an unused const overload is not
      // itself instantiated, so also look it up on the template
      c.getDeclaringType() = f.getDeclaringType().(ClassTemplateInstantiation).getTemplate()
    ) and
    c.getName() = f.getName() and
    c.getNumberOfParameters() = f.getNumberOfParameters()
  )
}

/**
 * Holds if `call` is the expansion of the `errno` macro (e.g.
 * `*__errno_location()`), which merely reads thread-local error state.
 */
predicate isErrnoRead(FunctionCall call) {
  exists(MacroInvocation mi |
    mi.getMacro().getName() = "errno" and
    mi.getAnExpandedElement() = call
  )
}

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
  not hasConstOverload(call.getTarget()) and
  not isErrnoRead(call) and
  // F Prime's bounded strnlen equivalent; the stock purity analysis
  // whitelists strnlen by name but reports string_length as impure only
  // because its own body contains an FW_ASSERT
  not call.getTarget().hasQualifiedName("Fw::StringUtils", "string_length") and
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
