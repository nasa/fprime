/**
 * @name Floating-point loop control
 * @description Loops controlled by a floating-point variable or expression
 *              may iterate an unexpected number of times due to rounding
 *              error; use an integer counter instead.
 * @kind problem
 * @id cpp/fprime/float-loop-control
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

from Loop l, VariableAccess va
where
  l.fromSource() and
  va.getParent*() = l.getControllingExpr() and
  va.getUnspecifiedType() instanceof FloatingPointType
select l,
  "This loop's condition depends on the floating-point variable " + va.getTarget().getName() + "."
