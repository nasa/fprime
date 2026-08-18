/**
 * @name Long function without assertion
 * @description All functions of more than 10 lines should have at least one assertion.
 * @kind problem
 * @id cpp/fprime/jpl-c/use-of-assertions-density
 * @problem.severity recommendation
 * @tags maintainability
 *       reliability
 *       external/jpl
 */

import FprimeAssertions

/**
 * A googletest assertion macro (`ASSERT_*` / `EXPECT_*`). Unit-test support
 * code checks its expectations with these rather than with `FW_ASSERT`, which
 * satisfies the intent of JPL Rule 16 in test code.
 */
class GTestAssert extends MacroInvocation {
  GTestAssert() { this.getMacro().getName().matches(["ASSERT\\_%", "EXPECT\\_%"]) }
}

/** An FPP-modeled enum class: a C++ class wrapping a nested enum named `T`. */
predicate fppEnumClass(Type t) {
  exists(Enum e | e.getName() = "T" and e.getDeclaringType() = t.getUnspecifiedType())
}

/** A status-like type: a plain enum or an FPP enum class. */
predicate statusType(Type t) { t.getUnspecifiedType() instanceof Enum or fppEnumClass(t) }

/**
 * A function that reports failures through its status-enum return value.
 * This is the F Prime error-handling style for recoverable conditions
 * (`Fw::SerializeStatus`, `Os::File::Status`, `Drv::SocketIpStatus`, ...):
 * the callee validates and returns a status, and callers are required to
 * check it (enforced by the Rule 14 checking-return-values query).
 */
predicate statusReturning(Function f) { statusType(f.getType()) }

/**
 * A function that reports off-nominal conditions by emitting a WARNING or
 * FATAL event, the F Prime component style for handling runtime errors that
 * are not programming defects (bad input data, resource exhaustion, ...).
 */
predicate eventLogging(Function f) {
  exists(FunctionCall c |
    c.getEnclosingFunction() = f and
    c.getTarget().getName().matches(["log\\_WARNING%", "log\\_FATAL%"])
  )
}

/** A function checked with googletest assertions (unit-test support code). */
predicate testAsserted(Function f) {
  exists(GTestAssert a, Expr e | e = a.getAnExpandedElement() and e.getEnclosingFunction() = f)
}

/**
 * A function that is part of the assertion-reporting machinery itself: the
 * default assert reporter, the C-linkage assert entry points invoked by the
 * FW_ASSERT macros, and the members of `Fw::AssertHook` and its subclasses
 * (e.g. `Test::UnitTestAssert`). Asserting inside the assert handler would
 * recurse into the very handler being executed, so these functions cannot
 * satisfy the rule with FW_ASSERT.
 */
predicate assertInfrastructure(Function f) {
  f.getDeclaringType().getABaseClass*().hasQualifiedName("Fw", "AssertHook")
  or
  f.hasGlobalName(["CAssert0", "CAssert1"])
  or
  f.hasQualifiedName("Fw", "defaultReportAssert")
}

/**
 * A function that reports failures through a `bool` return value, the same
 * caller-checked error-reporting contract as a status enum (e.g. the sequence
 * loaders' `validateRecords`/`deserializeHeader` returning false on invalid
 * input, or `RateLimiter::trigger`).
 */
predicate boolReturning(Function f) { f.getType().getUnspecifiedType() instanceof BoolType }

from Function f
where
  f.getMetrics().getNumberOfLinesOfCode() > 10 and
  // The extractor does not expand macros in uninstantiated template bodies,
  // so FW_ASSERT cannot be detected there; the template's instantiations are
  // analyzed instead (macro expansions are visible in instantiations).
  not f.isFromUninstantiatedTemplate(_) and
  not exists(Assertion a | a.getAsserted().getEnclosingFunction() = f) and
  // A static_assert is an assertion too -- arguably a better one, since it
  // proves the invariant at compile time and can never fire in flight.
  not exists(StaticAssert sa | sa.getCondition().getEnclosingFunction() = f) and
  not statusReturning(f) and
  not boolReturning(f) and
  not eventLogging(f) and
  not testAsserted(f) and
  not assertInfrastructure(f) and
  // Constructors and destructors are exempt: components may be globally
  // instantiated, so a runtime assertion there can fire before main() — before
  // the assert hook, logging, or any output path exists — making the failure
  // undiagnosable. Compile-time checks (static_assert) are the sanctioned
  // ctor/dtor-time mechanism.
  not f instanceof Constructor and
  not f instanceof Destructor
select f,
  "All functions of more than 10 lines should have at least one assertion, " +
    "unless they handle errors by returning a status enum or bool, or by emitting a WARNING/FATAL event."
