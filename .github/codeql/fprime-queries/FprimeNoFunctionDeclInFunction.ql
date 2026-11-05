/**
 * @name Function declaration inside a function body
 * @description Functions should not be declared within the body of another
 *              function; declare them at namespace or class scope.
 * @kind problem
 * @id cpp/fprime/no-function-decl-in-function
 * @problem.severity warning
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

from DeclStmt ds, Function declared
where
  ds.fromSource() and
  ds.getADeclaration() = declared
select ds,
  "Function " + declared.getName() + " is declared inside the body of function " +
    ds.getEnclosingFunction().getName() + "."
