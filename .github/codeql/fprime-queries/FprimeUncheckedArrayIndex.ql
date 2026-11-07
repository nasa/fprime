/**
 * @name Array index not checked against bounds
 * @description A non-constant array index should be checked (condition or
 *              FW_ASSERT) before the access it indexes. Loop-controlled
 *              indices are checked by the loop condition. Integer parameters
 *              used as indices are covered by
 *              cpp/fprime/checking-parameter-values.
 * @kind problem
 * @id cpp/fprime/unchecked-array-index
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeGuards

from ArrayExpr ae, VariableAccess idx, Variable v
where
  ae.fromSource() and
  idx = ae.getArrayOffset() and
  idx.getTarget() = v and
  not v instanceof Parameter and
  not exists(idx.getValue()) and
  not ae.getArrayBase().getUnspecifiedType() instanceof PointerType and
  not guardedBefore(v, ae) and
  not ae.getFile().getAbsolutePath().matches(["%/test/%", "%/ut/%"])
select ae, "Index " + v.getName() + " is not checked against the array bounds before this access."
