/**
 * @name Use of basic integral type
 * @description Typedefs that indicate size and signedness should be used in place of the basic types.
 * @kind problem
 * @id cpp/fprime/jpl-c/basic-int-types
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/**
 * F Prime refinement of the JPL Rule 17 query (cpp/jpl-c/basic-int-types).
 *
 * The stock query unwinds pointer and array types to their element type, so a
 * C-string or character array declared as `char*`, `const char*`, or `char[]`
 * is reported because it ultimately references the basic type `char`. The plain
 * `char` type is the language's character type and is the appropriate type for a
 * C-string, so uses of the plain `char` type reached through pointer/array
 * indirection are whitelisted here.
 *
 * Everything else is still flagged, including a scalar `char` used as a small
 * integer (which should be `I8`/`U8`) and `signed char`/`unsigned char` in any
 * form: only the plain (signed) `char` string/array is a valid C-string.
 */
predicate allowedTypedefs(TypedefType t) {
  t.getName() =
    [
      "I64", "U64", "I32", "U32", "I16", "U16", "I8", "U8", "F64", "F32", "int64_t", "uint64_t",
      "int32_t", "uint32_t", "int16_t", "uint16_t", "int8_t", "uint8_t"
    ]
}

/**
 * Gets a type which appears literally in the declaration of `d`.
 */
Type getAnImmediateUsedType(Declaration d) {
  d.hasDefinition() and
  (
    result = d.(Function).getType() or
    result = d.(Variable).getType()
  )
}

/**
 * Gets a type which appears indirectly in `t`, stopping at allowed typedefs.
 */
Type getAUsedType(Type t) {
  not allowedTypedefs(t) and
  (
    result = t.(TypedefType).getBaseType() or
    result = t.(DerivedType).getBaseType()
  )
}

predicate problematic(IntegralType t) {
  // List any exceptions that should be allowed.
  any()
}

/**
 * Holds if `t` is a pointer or array type whose ultimate element type -
 * ignoring typedefs, specifiers (e.g. `const`), and any number of pointer or
 * array levels - is the plain `char` type. This identifies C-strings and
 * character arrays/buffers such as `char*`, `const char*`, `char[]`, `char**`.
 */
predicate isPlainCharIndirection(Type t) {
  exists(Type base | base = t.getUnspecifiedType().(DerivedType).getBaseType() |
    base.getUnspecifiedType() instanceof PlainCharType
    or
    isPlainCharIndirection(base)
  )
}

from Declaration d, Type usedType
where
  usedType = getAUsedType*(getAnImmediateUsedType(d)) and
  problematic(usedType) and
  // Allow uses of boolean types where defined by the language.
  not usedType instanceof BoolType and
  // Ignore violations for which we do not have a valid location.
  not d.getLocation() instanceof UnknownLocation and
  // F Prime: allow the plain `char` type when used as a C-string or character
  // array (i.e. reached through pointer or array indirection). A scalar `char`,
  // and `signed char`/`unsigned char` in any form, are still flagged.
  not (
    usedType instanceof PlainCharType and
    isPlainCharIndirection(getAnImmediateUsedType(d))
  )
select d,
  d.getName() + " uses the basic integral type " + usedType.getName() +
    " rather than a typedef with size and signedness."
