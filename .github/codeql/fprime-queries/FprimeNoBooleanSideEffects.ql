/**
 * @name Side effect in a Boolean expression
 * @description The evaluation of a Boolean expression shall have no side effects.
 * @kind problem
 * @id cpp/fprime/jpl-c/no-boolean-side-effects
 * @problem.severity warning
 * @tags correctness
 *       readability
 *       external/jpl
 */

import cpp

/*
 * F Prime refinement of the stock JPL Rule 19 query
 * (cpp/jpl-c/no-boolean-side-effects). The stock query trips on three C++
 * constructs that are not side effects in a hand-written boolean expression:
 *
 * 1. Uninstantiated template code: calls whose target is unresolved (e.g.
 *    `e.getKey() == keyOrElement` in a class template) are represented as
 *    `ExprCall`s, which the stock query unconditionally treats as
 *    side-effecting. Instantiations are still checked.
 * 2. Range-based `for` statements: the compiler-generated condition
 *    `__begin != __end` calls iterator operators, which the purity analysis
 *    reports as unsafe. The programmer wrote no boolean expression at all.
 * 3. `constexpr` functions (e.g. `std::numeric_limits<T>::max()`): required
 *    by the language to be evaluable at compile time, hence side-effect free.
 * 4. Const member functions (and functions taking const arguments) whose
 *    bodies are provably pure: the stock argument-type heuristic rejects any
 *    argument whose class contains a non-const pointer member (e.g.
 *    `Fw::Buffer`'s `U8*`), even when the callee is a `const` accessor or
 *    comparison. Such calls are accepted when the callee's body writes only
 *    local variables and calls only functions that are themselves safe.
 */

/**
 * A whitelist of functions that should be considered
 * side-effect free.
 */
predicate safeFunctionWhitelist(Function f) {
  // constexpr functions are evaluable at compile time and side-effect free
  f.isConstexpr()
  or
  // The FW_ASSERT reporting path: an assertion only acts when the program is
  // already failing fatally, so it is not a side effect of normal evaluation
  f.getQualifiedName() = "Fw::SwAssert"
}

/**
 * Gets a "pointer type" contained in the given type. This
 * traverses typedefs and derived types, including types of
 * struct or union members, returning each "pointer to X"
 * type encountered on that traversal.
 */
PointerType getAPointerType(Type t) {
  result = t or
  result = getAPointerType(t.getUnderlyingType()) or
  result = getAPointerType(t.(DerivedType).getBaseType()) or
  result = getAPointerType(t.(Class).getAMemberVariable().getType())
}

/**
 * A function is "inherently unsafe" for side effects if it
 * writes a global or static variable, or if it calls another
 * inherently unsafe function.
 */
predicate inherentlyUnsafe(Function f) {
  exists(Variable v | v.getAnAssignedValue().getEnclosingFunction() = f |
    v instanceof GlobalVariable or
    v.isStatic()
  )
  or
  exists(FunctionCall c | c.getEnclosingFunction() = f | inherentlyUnsafe(c.getTarget()))
}

/**
 * Find functions that are "safe to call" without causing a side effect.
 * Being safe to call means that any "pointer type" in an argument type
 * actually refers to a "const" object, and, moreover, the function is
 * not inherently unsafe.
 */
predicate safeToCall(Function f) {
  forall(PointerType paramPointerType |
    paramPointerType = getAPointerType(f.getAParameter().getType())
  |
    paramPointerType.getBaseType().isConst()
  ) and
  not inherentlyUnsafe(f)
}

/**
 * Holds if the function contains a write that may reach beyond its
 * local variables. Writes through pointers, references, or member
 * variables are conservatively treated as non-local.
 */
predicate localNonReferenceVariable(Variable v) {
  v instanceof LocalScopeVariable and
  not v.getUnspecifiedType() instanceof ReferenceType
}

predicate writesNonLocal(Function f) {
  exists(Assignment a | a.getEnclosingFunction() = f |
    not localNonReferenceVariable(a.getLValue().(VariableAccess).getTarget())
  )
  or
  exists(CrementOperation c | c.getEnclosingFunction() = f |
    not localNonReferenceVariable(c.getOperand().(VariableAccess).getTarget())
  )
}

/**
 * Holds if a call to this function may be assumed safe from its signature
 * alone: the `safeToCall` pointer criterion holds, every reference parameter
 * refers to a const object, and the function is not virtual (a virtual
 * target's overrides are unknown).
 */
predicate safeCallTarget(Function f) {
  safeToCall(f) and
  not f.isVirtual() and
  forall(ReferenceType rt | rt = f.getAParameter().getType().getUnspecifiedType() |
    rt.getBaseType().isConst()
  ) and
  // A non-const member function may write through `this`
  (not f instanceof MemberFunction or f.isStatic() or f.(MemberFunction).hasSpecifier("const"))
}

/** Gets the function itself, or an override that may be the runtime target of a virtual call. */
Function anImplementation(Function f) {
  result = f or
  result.(MemberFunction).getAnOverriddenFunction+() = f
}

/**
 * Holds if the function may have a side effect: it has no body to inspect,
 * is virtual (the runtime target is unknown), writes a non-local variable,
 * allocates, contains assembly, or calls something that may itself be
 * impure. Phrased positively so the recursion is monotonic.
 */
predicate maybeImpure(Function f) {
  // No body available for any possible runtime target
  not exists(Function impl | impl = anImplementation(f) | impl.hasDefinition())
  or
  exists(Function impl | impl = anImplementation(f) and impl.hasDefinition() |
    writesNonLocal(impl)
    or
    exists(NewOrNewArrayExpr n | n.getEnclosingFunction() = impl)
    or
    exists(DeleteOrDeleteArrayExpr d | d.getEnclosingFunction() = impl)
    or
    exists(AsmStmt s | s.getEnclosingFunction() = impl)
    or
    exists(ExprCall ec | ec.getEnclosingFunction() = impl and not ec.isFromUninstantiatedTemplate(_))
    or
    exists(FunctionCall c | c.getEnclosingFunction() = impl and not c.isFromUninstantiatedTemplate(_) |
      not safeFunctionWhitelist(c.getTarget()) and
      maybeImpure(c.getTarget()) and
      // The signature shortcut applies only when no body is available to inspect:
      // a const signature does not preclude side effects (e.g. event logging)
      (
        exists(Function impl2 | impl2 = anImplementation(c.getTarget()) and impl2.hasDefinition()) or
        not safeCallTarget(c.getTarget())
      )
    )
  )
}

/**
 * Holds if the function's body is provably free of side effects: it writes
 * only local variables, performs no allocation, and every call it makes
 * targets a function that is itself known to be safe. This accepts const
 * member functions and comparisons that the argument-type heuristic of
 * `safeToCall` rejects (any argument whose class contains a non-const
 * pointer member).
 */
predicate provablyPure(Function f) { not maybeImpure(f) }

/**
 * A "Boolean expression" is an expression forbidden from having side effects
 * by this rule. Conditions generated by the compiler for range-based `for`
 * statements are not boolean expressions written by the programmer.
 */
class BooleanExpression extends Expr {
  BooleanExpression() {
    (
      exists(Loop l | l.getControllingExpr() = this) or
      exists(IfStmt i | i.getCondition() = this) or
      exists(ConditionalExpr e | e.getCondition() = this)
    ) and
    not exists(RangeBasedForStmt r | r.getCondition() = this)
  }
}

predicate hasSideEffect(Expr e) {
  e instanceof Assignment
  or
  e instanceof CrementOperation
  or
  e instanceof ExprCall and not e.isFromUninstantiatedTemplate(_)
  or
  exists(Function f | f = e.(FunctionCall).getTarget() and not safeFunctionWhitelist(f) |
    (inherentlyUnsafe(f) or not safeToCall(f)) and not provablyPure(f)
  )
  or
  hasSideEffect(e.getAChild())
}

from BooleanExpression b
where hasSideEffect(b) and not b.isFromUninstantiatedTemplate(_)
select b, "This Boolean expression is not side-effect free."
