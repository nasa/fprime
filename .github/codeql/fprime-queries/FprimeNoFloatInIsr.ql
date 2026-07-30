/**
 * @name Floating-point operation in interrupt service routine
 * @description Interrupt service routines should not use floating-point
 *              arithmetic: the FPU context may not be saved/restored on
 *              interrupt entry on many flight platforms.
 * @kind problem
 * @id cpp/fprime/no-float-in-isr
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeBlocking

from Operation op, Function isr
where
  isr = op.getEnclosingFunction() and
  isIsrFunction(isr) and
  op.fromSource() and
  (
    op.getUnspecifiedType() instanceof FloatingPointType
    or
    op.getAnOperand().getUnspecifiedType() instanceof FloatingPointType
  )
select op, "Floating-point operation in interrupt service routine " + isr.getName() + "."
