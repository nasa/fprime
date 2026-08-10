/**
 * @name Validation failure path does not return
 * @description In F Prime handlers, a branch that sends a failed command
 *              response should return rather than continue executing.
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

/** A call that sends a failed (non-OK) command response. */
predicate failureReport(FunctionCall fc) {
  fc.getTarget().getName() = "cmdResponse_out" and
  exists(EnumConstantAccess eca | eca.getParent*() = fc.getArgument(2) |
    eca.getTarget().getName() != "OK"
  ) and
  not exists(EnumConstantAccess eca | eca.getParent*() = fc.getArgument(2) |
    eca.getTarget().getName() = "OK"
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
  // ... the nominal path is not confined to an else branch ...
  not exists(guard.getElse()) and
  // ... and there is code after the guard that would run anyway
  exists(guard.getFollowingStmt())
select guard,
  "This validation-failure branch in handler " + handler.getName() +
    " sends a failed command response but does not return."
