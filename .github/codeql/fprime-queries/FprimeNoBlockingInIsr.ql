/**
 * @name Blocking operation in interrupt service routine
 * @description Interrupt service routines must not perform blocking
 *              operations such as mutex locks, sleeps, semaphore waits, or
 *              blocking I/O.
 * @kind problem
 * @id cpp/fprime/no-blocking-in-isr
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeBlocking

from BlockingCall bc, Function isr
where
  isr = bc.getEnclosingFunction() and
  isIsrFunction(isr) and
  bc.fromSource()
select bc,
  "Blocking operation " + bc.blockingDescription() + " called in interrupt service routine " +
    isr.getName() + "."
