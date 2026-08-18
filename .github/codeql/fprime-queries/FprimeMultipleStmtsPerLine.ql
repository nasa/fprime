/**
 * @name More than one statement per line
 * @description Putting more than one statement on a single line hinders program understanding.
 * @kind problem
 * @id cpp/fprime/jpl-c/multiple-stmts-per-line
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/**
 * F Prime refinement of the JPL Rule 24 query (cpp/jpl-c/multiple-stmts-per-line).
 *
 * The stock query was written for C and miscounts several C++ constructs:
 *
 * - Template instantiations: every instantiation of a template contributes a
 *   copy of each statement at the same source location, so a single statement
 *   written once in a template is counted once per instantiation and the line
 *   is falsely reported. Only statements from the uninstantiated template (or
 *   from non-template code) are counted here.
 * - Compiler-generated statements (e.g. in implicitly-defined member
 *   functions) carry the location of the enclosing declaration and are not
 *   statements the programmer wrote, so they are not counted.
 * - Braces: a block (`{ ... }`) is a grouping construct, not a statement in
 *   the sense of this rule, so blocks themselves are not counted. The stock
 *   query counts the block of a single-line inline member function
 *   (`FwSizeType getSize() const { return this->m_size; }`) as a second
 *   statement and flags an idiom that the repository's clang-format style
 *   (Chromium) itself produces.
 * - `case`/`default` labels are not counted as statements.
 * - Macros: a single macro invocation (e.g. `CPU_SET(cpu, &set);`) may expand
 *   to several statements that all carry the location of the invocation. The
 *   programmer wrote one statement, so statements sharing one location are
 *   counted once (distinct start columns are counted, not statements).
 *
 * The rule's intent is sequential statements sharing a line, so this query
 * flags a line only when two or more sibling statements (children of the same
 * parent) start on it. Sequential code on one line (`a(); b();`) is still
 * flagged - wherever it appears, including inside a single-line function -
 * while a lone nested statement (`if (valid) { count++; }` or a single-line
 * accessor body) is not.
 */
class CountedStmt extends Stmt {
  CountedStmt() {
    this.getLocation().getStartLine() = this.getLocation().getEndLine() and
    not exists(ForStmt for | this = for.getInitialization()) and
    not this instanceof BlockStmt and
    not this instanceof SwitchCase and
    not this.isCompilerGenerated() and
    not this.isFromTemplateInstantiation(_) and
    not this.getLocation() instanceof UnknownLocation
  }

  predicate onLine(File f, int line) {
    f = this.getFile() and line = this.getLocation().getStartLine()
  }
}

int numSiblingStmt(Stmt parent, File f, int line) {
  result =
    strictcount(int col |
      exists(CountedStmt o |
        o.getParent() = parent and o.onLine(f, line) and col = o.getLocation().getStartColumn()
      )
    )
}

from Stmt parent, File f, int line, CountedStmt o, int cnt
where
  numSiblingStmt(parent, f, line) = cnt and
  cnt > 1 and
  o.getParent() = parent and
  o.onLine(f, line) and
  o.getLocation().getStartColumn() =
    min(CountedStmt other, int toMin |
      other.getParent() = parent and
      other.onLine(f, line) and
      toMin = other.getLocation().getStartColumn()
    |
      toMin
    )
select o, "This line contains " + cnt + " statements; only one is allowed."
