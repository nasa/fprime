/**
 * @name Switch case falls through without annotation
 * @description Every non-empty switch case should end in a break (or other
 *              control transfer); intentional fall-through must be marked
 *              with a fall-through comment.
 * @kind problem
 * @id cpp/fprime/switch-case-fallthrough
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

/** The last case of a switch (nothing to fall into). */
predicate lastCase(SwitchCase sc) { not exists(sc.getNextSwitchCase()) }

Stmt lastStmtOf(SwitchCase sc) { result = sc.getLastStmt() }

/** Holds if `s` unconditionally transfers control out of the case. */
predicate transfersControl(Stmt s) {
  s instanceof BreakStmt or
  s instanceof ReturnStmt or
  s instanceof ContinueStmt or
  s instanceof GotoStmt or
  s.(ExprStmt).getExpr() instanceof ThrowExpr or
  transfersControl(s.(BlockStmt).getLastStmt())
}

/** A fall-through comment between `sc`'s statements and the next case label. */
predicate hasFallthroughAnnotation(SwitchCase sc) {
  exists(SwitchCase next, int lastLine, int nextLine |
    next = sc.getNextSwitchCase() and
    lastLine = lastStmtOf(sc).getLocation().getEndLine() and
    nextLine = next.getLocation().getStartLine()
  |
    exists(Comment c |
      c.getFile() = sc.getFile() and
      c.getLocation().getStartLine() >= lastLine and
      c.getLocation().getStartLine() <= nextLine and
      c.getContents().toLowerCase().regexpMatch("(?s).*fall[ -]?thr(ough|u).*")
    )
  )
}

from SwitchCase sc
where
  sc.fromSource() and
  exists(sc.getAStmt()) and
  not lastCase(sc) and
  not transfersControl(lastStmtOf(sc)) and
  not hasFallthroughAnnotation(sc)
select sc,
  "This non-empty switch case falls through to the next case without a fall-through annotation."
