/**
 * @name Implicit narrowing of double to float
 * @description Implicitly converting a double-precision value to single
 *              precision silently discards precision. Decide the precision
 *              explicitly: compute in F32 end to end, or use an explicit
 *              `static_cast<F32>` where the loss is intended.
 * @kind problem
 * @id cpp/fprime/float-narrowing
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

from Conversion c, FloatingPointType src, FloatingPointType dst
where
  c.fromSource() and
  c.isImplicit() and
  src = c.getExpr().getUnspecifiedType() and
  dst = c.getUnspecifiedType() and
  dst.getSize() < src.getSize() and
  // Literals (1.0 vs 1.0f) are a style matter handled elsewhere
  not c.getExpr() instanceof Literal and
  not exists(c.getExpr().getValue())
select c.getExpr(),
  "Value of type " + src.getName() + " is implicitly narrowed to " + dst.getName() + "."
