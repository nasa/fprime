/**
 * @name Unthrottled event in cyclic handler
 * @description Events emitted from a rate-group or other cyclic handler can
 *              flood the event log every cycle. Such events should be
 *              throttled in the FPP model (`throttle N`) or gated on a change
 *              of component state (a condition that reads a member variable).
 * @kind problem
 * @id cpp/fprime/unthrottled-cyclic-event
 * @problem.severity warning
 * @tags reliability
 *       external/jpl
 */

import cpp
import FprimeBlocking

/** A handler invoked periodically by a rate group or scheduler. */
predicate isCyclicHandler(Function f) {
  isSchedInHandler(f)
  or
  f.getName().toLowerCase().matches(["%sched%\\_handler", "run%\\_handler", "%cycle%\\_handler", "%tick%\\_handler"])
}

/** A call to an autocoded event logger `log_<SEVERITY>_<Event>`. */
class EventLogCall extends FunctionCall {
  EventLogCall() {
    this.getTarget() instanceof MemberFunction and
    this.getTarget().getName().regexpMatch("log_(DIAGNOSTIC|ACTIVITY_(LO|HI)|WARNING_(LO|HI)|COMMAND|FATAL)_.*") and
    not this.getTarget().getName().matches("%\\_ThrottleClear")
  }

  /** The logger's body manipulates a throttle counter (FPP `throttle` keyword). */
  predicate isThrottled() {
    exists(FieldAccess fa |
      fa.getEnclosingFunction() = this.getTarget() and fa.getTarget().getName().matches("%Throttle")
    )
  }
}

/** The call sits under a condition that reads component state. */
predicate gatedOnState(EventLogCall call) {
  exists(Expr cond |
    cond.isCondition() and
    cond.getEnclosingFunction() = call.getEnclosingFunction() and
    exists(FieldAccess fa | fa = cond.getAChild*()) and
    exists(Stmt s | s.getAChild*() = call.getEnclosingStmt() |
      s.(IfStmt).getCondition() = cond or s.(SwitchStmt).getExpr() = cond
    )
  )
}

from EventLogCall call, Function handler
where
  call.fromSource() and
  handler = call.getEnclosingFunction() and
  isCyclicHandler(handler) and
  not call.isThrottled() and
  not gatedOnState(call)
select call,
  "Event " + call.getTarget().getName() + " is emitted from cyclic handler " + handler.getName() +
    " without throttling or a change-of-state gate."
