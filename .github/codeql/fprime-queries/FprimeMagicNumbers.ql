/**
 * @name Magic number
 * @description Invariant numeric literals (other than 0 and 1) should be
 *              given symbolic names via enum, static constexpr/const, or
 *              #define.
 * @kind problem
 * @id cpp/fprime/magic-numbers
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/** A trivial literal value that does not need a name. */
predicate trivialValue(Literal l) { l.getValue() = ["0", "1", "-1"] }

/** Holds if `l` initializes a named constant (const/constexpr variable or enum). */
predicate initializesConstant(Literal l) {
  exists(Variable v | v.getInitializer().getExpr().getAChild*() = l |
    v.isConstexpr() or v.getType().isConst()
  )
  or
  exists(EnumConstant ec | ec.getInitializer().getExpr().getAChild*() = l)
}

from Literal l
where
  l.fromSource() and
  (l.getUnspecifiedType() instanceof IntegralType or
    l.getUnspecifiedType() instanceof FloatingPointType) and
  not l instanceof CharLiteral and
  not l instanceof StringLiteral and
  not l.getUnspecifiedType() instanceof BoolType and
  not trivialValue(l) and
  not l.isInMacroExpansion() and
  not initializesConstant(l)
select l,
  "Magic number " + l.getValueText() +
    "; consider a named constant (enum, static constexpr, or #define)."
