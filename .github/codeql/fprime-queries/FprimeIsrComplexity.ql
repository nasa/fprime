/**
 * @name Interrupt service routine is not minimal
 * @description Interrupt service routines should do the minimum work
 *              (capture data, signal a task) and return. Loops and long
 *              bodies in an ISR extend interrupt latency for the whole
 *              system.
 * @kind problem
 * @id cpp/fprime/isr-complexity
 * @problem.severity warning
 * @tags reliability
 *       external/jpl
 */

import cpp
import FprimeBlocking

int maxIsrStatements() { result = 20 }

from Function isr, string reason
where
  isIsrFunction(isr) and
  isr.fromSource() and
  isr.hasDefinition() and
  (
    exists(Loop l | l.getEnclosingFunction() = isr) and reason = "contains a loop"
    or
    exists(int n | n = count(Stmt s | s.getEnclosingFunction() = isr and not s instanceof BlockStmt) |
      n > maxIsrStatements() and reason = "has " + n + " statements"
    )
  )
select isr, "Interrupt service routine " + isr.getName() + " " + reason + "; keep ISRs minimal."
