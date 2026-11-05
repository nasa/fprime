/**
 * @name Loop or conditional body is not a compound statement
 * @description The bodies of if/else statements and for/while/do-while loops
 *              should be compound statements (enclosed in braces).
 * @kind problem
 * @id cpp/fprime/compound-statements
 * @problem.severity warning
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

from Stmt body, Stmt parent, string kind
where
  body.fromSource() and
  not body instanceof BlockStmt and
  (
    body = parent.(ForStmt).getStmt() and kind = "for loop"
    or
    body = parent.(WhileStmt).getStmt() and kind = "while loop"
    or
    body = parent.(DoStmt).getStmt() and kind = "do-while loop"
    or
    body = parent.(IfStmt).getThen() and kind = "if statement"
    or
    body = parent.(IfStmt).getElse() and
    // `else if` chains are idiomatic and permitted
    not body instanceof IfStmt and
    kind = "else clause"
  )
select body, "The body of this " + kind + " is not a compound statement (missing braces)."
