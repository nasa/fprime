/**
 * @name Magic number
 * @description Invariant numeric literals used inline or assigned to local
 *              variables should be given symbolic names via enum, static
 *              constexpr/const, or #define. Trivial values, powers of 2 up to
 *              4096, all-ones masks, powers of 10, multiples of 8 as shift
 *              amounts, and floating-point multiples of 10 are permitted.
 * @kind problem
 * @id cpp/fprime/magic-numbers
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/** A trivial or idiomatic literal value that does not need a name. */
predicate trivialValue(Literal l) {
  l.getValue() = ["0", "1", "-1"]
  or
  // Powers of 2 up to 4096
  l.getValue().toInt() = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
  or
  // All-ones bit masks
  l.getValue().toInt() =
    [
      3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535, 131071, 262143,
      524287, 1048575, 2097151, 4194303, 8388607, 16777215, 33554431, 67108863, 134217727,
      268435455, 536870911, 1073741823, 2147483647
    ]
  or
  l.getValue() = ["4294967295", "1099511627775", "281474976710655", "72057594037927935",
      "18446744073709551615"]
  or
  // Powers of 10
  l.getValue().toInt() = [10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000]
  or
  l.getValue() = ["10000000000", "100000000000", "1000000000000"]
  or
  // Multiples of 8 used as shift amounts (byte-oriented shifts)
  l.getValue().toInt() % 8 = 0 and
  exists(Expr shift |
    shift.(LShiftExpr).getRightOperand() = l or
    shift.(RShiftExpr).getRightOperand() = l or
    shift.(AssignLShiftExpr).getRValue() = l or
    shift.(AssignRShiftExpr).getRValue() = l
  )
  or
  // Floating-point multiples of 10 (unit conversions)
  l.getUnspecifiedType() instanceof FloatingPointType and
  exists(float f | f = l.getValue().toFloat() | (f / 10.0).floor() * 10.0 = f)
}

/**
 * Holds if `l` appears in a constant definition rather than an inline use or
 * local variable assignment: an initializer of a non-local variable, a
 * const/constexpr variable, or an enum constant.
 */
predicate constantDefinition(Literal l) {
  exists(Variable v | v.getInitializer().getExpr().getAChild*() = l |
    not v instanceof LocalScopeVariable or
    v.isConstexpr() or
    v.getType().isConst()
  )
  or
  exists(EnumConstant ec | ec.getInitializer().getExpr().getAChild*() = l)
}

from Literal l
where
  l.fromSource() and
  // Test code (test/ut directories) and the Crc32 module are exempt
  not l.getFile().getAbsolutePath().matches(["%/test/%", "%/ut/%", "%/Crc32/%"]) and
  (l.getUnspecifiedType() instanceof IntegralType or
    l.getUnspecifiedType() instanceof FloatingPointType) and
  not l instanceof CharLiteral and
  not l instanceof StringLiteral and
  not l.getUnspecifiedType() instanceof BoolType and
  not trivialValue(l) and
  not l.isInMacroExpansion() and
  // Template instantiations materialize template arguments as literals
  not l.isFromTemplateInstantiation(_) and
  // Only actual numeric tokens (not template parameters or folded expressions)
  l.getValueText().regexpMatch("[0-9+\\-.].*") and
  not constantDefinition(l)
select l,
  "Magic number " + l.getValueText() +
    "; consider a named constant (enum, static constexpr, or #define)."
