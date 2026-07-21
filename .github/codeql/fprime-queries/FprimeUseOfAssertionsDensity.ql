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

from Function f
where
  f.getMetrics().getNumberOfLinesOfCode() > 10 and
  // The extractor does not expand macros in uninstantiated template bodies,
  // so FW_ASSERT cannot be detected there; the template's instantiations are
  // analyzed instead (macro expansions are visible in instantiations).
  not f.isFromUninstantiatedTemplate(_) and
  not exists(Assertion a | a.getAsserted().getEnclosingFunction() = f) and
  not statusReturning(f) and
  not eventLogging(f) and
  not testAsserted(f)
select f,
  "All functions of more than 10 lines should have at least one assertion, " +
    "unless they handle errors by returning a status enum or by emitting a WARNING/FATAL event."
