/**
 * @name Mutable variable with static storage
 * @description Variables should be declared local to the smallest scope that
 *              needs them. Namespace-scope and function-static variables that
 *              are not const hold hidden state, defeat reentrancy, and are
 *              shared across component instances. Prefer a member of the
 *              component or a local variable.
 * @kind problem
 * @id cpp/fprime/global-mutable-variable
 * @problem.severity warning
 * @tags maintainability
 *       reliability
 *       external/jpl
 */

import cpp

predicate isMutable(Variable v) {
  not v.isConst() and
  not v.isConstexpr() and
  not v.getType().isDeeplyConst()
}

from Variable v, string kind
where
  v.fromSource() and
  isMutable(v) and
  not v.getFile().getAbsolutePath().matches(["%/test/%", "%/ut/%"]) and
  (
    v instanceof GlobalOrNamespaceVariable and kind = "Namespace-scope"
    or
    v.(LocalVariable).isStatic() and kind = "Function-static"
  )
select v, kind + " variable " + v.getName() + " is mutable; declare it local or as a member."
