/**
 * @name Use of printf family
 * @description Flight code should not use printf-family functions; use the
 *              F Prime event/logging facilities instead.
 * @kind problem
 * @id cpp/fprime/no-printf
 * @problem.severity warning
 * @tags maintainability
 *       reliability
 *       external/jpl
 */

import cpp

from FunctionCall fc
where
  fc.fromSource() and
  fc.getTarget().getName() =
    [
      "printf", "fprintf", "sprintf", "snprintf", "vprintf", "vfprintf", "vsprintf", "vsnprintf",
      "puts", "putchar"
    ] and
  not fc.getTarget() instanceof MemberFunction
select fc, "Call to " + fc.getTarget().getName() + "; use F Prime events/logging instead."
