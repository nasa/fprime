/**
 * @name Incomplete-code marker
 * @description Comments containing TBD, TODO, or FIXME indicate incomplete
 *              code that should be resolved before flight. (Leftover printf
 *              calls are flagged by cpp/fprime/no-printf.)
 * @kind problem
 * @id cpp/fprime/incomplete-code
 * @problem.severity recommendation
 * @tags maintainability
 *       external/jpl
 */

import cpp

from Comment c, string marker
where
  c.fromSource() and
  marker = c.getContents().regexpCapture("(?s).*\\b(TBD|TODO|FIXME)\\b.*", 1)
select c, "Comment contains the incomplete-code marker " + marker + "."
