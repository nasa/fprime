/**
 * @name Switch statement without a default case
 * @description Every switch statement should have a default case to handle
 *              unexpected values.
 * @kind problem
 * @id cpp/fprime/switch-default
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

from SwitchStmt s
where
  s.fromSource() and
  not s.hasDefaultCase()
select s, "This switch statement does not have a default case."
