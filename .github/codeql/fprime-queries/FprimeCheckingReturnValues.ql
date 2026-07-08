/**
 * @name Unchecked return value
 * @description The return value of each non-void function call should be checked for error conditions, or cast to (void) if irrelevant.
 * @kind problem
 * @id cpp/fprime/jpl-c/checking-return-values
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

/**
 * F Prime refinement of the JPL Rule 14 query (cpp/jpl-c/checking-return-values).
 *
 * Assignment and increment/decrement operators return a reference to the
 * modified object solely to support chaining (e.g. `a = b = c`). Ignoring that
 * reference is idiomatic C++ and is not an unchecked error condition, so these
 * operators are whitelisted here. All other non-void calls are still checked,
 * including `Fw::StringBase::format()` (see https://github.com/nasa/fpp/issues/1031).
 */
predicate whitelist(Function f) {
  f.getName() =
    [
      "operator=", "operator+=", "operator-=", "operator*=", "operator/=", "operator%=",
      "operator&=", "operator|=", "operator^=", "operator<<=", "operator>>=", "operator++",
      "operator--"
    ]
}

from FunctionCall c, string msg
where
  not c.getTarget().getType() instanceof VoidType and
  not whitelist(c.getTarget()) and
  (
    c instanceof ExprInVoidContext and
    msg = "The return value of non-void function $@ is not checked."
    or
    definition(_, c.getParent()) and
    not definitionUsePair(_, c.getParent(), _) and
    msg = "$@'s return value is stored but not checked."
  )
select c, msg, c.getTarget() as f, f.getName()
