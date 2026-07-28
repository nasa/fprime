/**
 * @name Validation failure path does not return
 * @description In F Prime port and command handlers, a branch that reports a
 *              validation failure (warning event or failed command response)
 *              should return rather than continue executing.
 * @kind problem
 * @id cpp/fprime/validation-failure-return
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

/** An F Prime port, command, or internal-interface handler implementation. */
predicate isFprimeHandler(Function f) {
  f.getName().matches(["%\\_handler", "%\\_cmdHandler", "%\\_internalInterfaceHandler"])
}

/** A call that reports a failure: a warning event or a failed command response. */
predicate failureReport(FunctionCall fc) {
  fc.getTarget().getName().matches("log\\_WARNING\\_%")
  or
  fc.getTarget().getName() = "cmdResponse_out" and
  exists(Expr resp | resp = fc.getArgument(2) |
    not resp.toString().matches("%OK%")
  )
}

/** Holds if `s` unconditionally leaves the handler or loop iteration. */
predicate transfersControl(Stmt s) {
  s instanceof ReturnStmt or
  s instanceof BreakStmt or
  s instanceof ContinueStmt or
  transfersControl(s.(BlockStmt).getLastStmt())
}

from IfStmt guard, FunctionCall report, Function handler
where
  handler = guard.getEnclosingFunction() and
  isFprimeHandler(handler) and
  guard.fromSource() and
  // The then-branch reports a failure ...
  report.getEnclosingStmt().getParent*() = guard.getThen() and
  failureReport(report) and
  // ... but does not stop execution ...
  not transfersControl(guard.getThen()) and
  // ... and there is code after the guard that would run anyway
  exists(guard.getFollowingStmt())
select guard,
  "This validation-failure branch in handler " + handler.getName() +
    " reports a failure but does not return."
