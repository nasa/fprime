/**
 * @name Blocking operation in rate-group handler
 * @description Rate-group (schedIn) handlers run on the rate group's thread
 *              and must complete quickly; blocking operations such as locks,
 *              sleeps, or blocking I/O can overrun the cycle.
 * @kind problem
 * @id cpp/fprime/no-blocking-in-sched-handler
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeBlocking

from BlockingCall bc, Function handler
where
  handler = bc.getEnclosingFunction() and
  isSchedInHandler(handler) and
  bc.fromSource()
select bc,
  "Blocking operation " + bc.blockingDescription() + " called in rate-group handler " +
    handler.getName() + "."
