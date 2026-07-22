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
 * Holds if `t` is a typedef declared outside the repository (a system or
 * toolchain header, e.g. `off_t`, `mode_t`, `ssize_t`, `size_t`). Such a
 * typedef is the type an external API dictates, so a declaration using it is
 * honoring the external interface contract; replacing it with an F Prime
 * sized type would be wrong on platforms where the sizes differ.
 */
predicate systemTypedef(TypedefType t) { not exists(t.getFile().getRelativePath()) }

/**
 * Gets a type which appears indirectly in `t`, stopping at allowed typedefs
 * and at typedefs declared by system headers.
 */
Type getAUsedType(Type t) {
  not allowedTypedefs(t) and
  not systemTypedef(t) and
  (
    result = t.(TypedefType).getBaseType() or
    result = t.(DerivedType).getBaseType()
  )
}

/**
 * Holds if `f` is declared outside the repository: a system, libc, or
 * toolchain function (e.g. `::open`, `::ioctl`, `::lseek`). Functions
 * declared anywhere in the repository are NOT external, so APIs introduced
 * in this repository that use basic integral types are still reported.
 */
predicate externalFunction(Function f) { not exists(f.getFile().getRelativePath()) }

/**
 * Gets an expression whose value `v` receives, either by initialization or
 * by assignment.
 */
Expr getAnAssignedExpr(Variable v) {
  result = v.getInitializer().getExpr()
  or
  exists(AssignExpr a | a.getLValue() = v.getAnAccess() and result = a.getRValue())
}

/**
 * Holds if `e` is a value dictated by an external (system/libc/toolchain) API:
 * the result of calling an external function (e.g. `::open(...)`,
 * `::pthread_create(...)`), or the `errno` macro, which is `int` by contract.
 */
predicate isExternalValueExpr(Expr e) {
  exists(FunctionCall c | externalFunction(c.getTarget()) and c = e)
  or
  exists(MacroInvocation mi | mi.getMacroName() = "errno" and mi.getExpr() = e)
}

/**
 * Holds if `e` conveys an externally-dictated value: either directly (see
 * `isExternalValueExpr`), or by reading a variable that itself receives such a
 * value (e.g. `int errno_store = errno; ...; f(errno_store)`, or
 * `int status = ::pthread_x(...); return status;`). This lets the external
 * contract be recognized one indirection away, which is how the Posix layer
 * captures `errno`/return codes into a local before translating them.
 */
predicate carriesExternalValue(Expr e) {
  isExternalValueExpr(e)
  or
  exists(Variable v | e = v.getAnAccess() and isExternalValueExpr(getAnAssignedExpr(v)))
}

/**
 * Holds if declaration `d` exists to interface with an external API, which
 * dictates its basic integral type. Unlike a value the repository chooses
 * freely, one that crosses a system/libc boundary must match the external
 * contract, so replacing it with an F Prime sized type would be wrong.
 *
 * This covers, symmetrically for local variables, parameters, members and the
 * return types they belong to:
 * - a variable that receives an externally-dictated value by initialization or
 *   assignment (e.g. `int descriptor = ::open(...)`, `fd = ::open(...)`);
 * - a variable that is passed to an external function, directly or by address
 *   (e.g. `::ioctl(chip_descriptor, ...)`, the flags built for `::open`);
 * - a parameter whose argument at a call site is externally dictated, i.e. the
 *   errno/status-conversion helpers invoked as `f(errno)` or `f(::pthread_*())`
 *   (their `int` parameter mirrors the external value they translate);
 * - a function whose return value forwards an external call (its `int` return
 *   mirrors the external API it wraps, e.g. thin wrappers around `pthread_*`);
 * - a variable initialized from a system/toolchain macro (e.g. `SCHED_RR`,
 *   `SOL_SOCKET`), whose integer type is fixed by that external definition.
 */
predicate externalApiDeclaration(Declaration d) {
  exists(Variable v | v = d | isExternalValueExpr(getAnAssignedExpr(v)))
  or
  exists(FunctionCall c, Variable v | v = d and externalFunction(c.getTarget()) |
    c.getAnArgument() = v.getAnAccess() or
    c.getAnArgument().(AddressOfExpr).getOperand() = v.getAnAccess()
  )
  or
  exists(Parameter p, Call call |
    p = d and call.getTarget() = p.getFunction() and
    carriesExternalValue(call.getArgument(p.getIndex()))
  )
  or
  exists(ReturnStmt rs | rs.getEnclosingFunction() = d and carriesExternalValue(rs.getExpr()))
  or
  exists(MacroInvocation mi, Variable v |
    v = d and
    not exists(mi.getMacro().getFile().getRelativePath()) and
    mi.getExpr() = v.getInitializer().getExpr()
  )
}

/**
 * Holds if `d` is the loop variable of a range-based `for` iterating a
 * `std::initializer_list`. The autocoder emits `for (const auto& e : il)` in
 * generated array classes; like a template instantiation, `auto` deduction
 * records the builtin element type (e.g. `unsigned int`) rather than the F Prime
 * typedef the element was declared with (e.g. `U32`), so the size/signedness
 * typedef is not observable at this declaration and the report is spurious.
 */
predicate autoInitializerListLoopVar(Declaration d) {
  exists(RangeBasedForStmt f, Class ilist |
    f.getVariable() = d and
    ilist.getSimpleName() = "initializer_list" and
    f.getRange().getType().refersTo(ilist)
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
  // F Prime: ignore declarations in template instantiations. Instantiating a
  // template parameter with an allowed typedef (e.g. `U32`) records the
  // underlying builtin type (`unsigned int`) on the instantiated declaration,
  // so such declarations would be misreported. The template itself is still
  // checked, so literal uses of basic types in template code are still flagged.
  not d.isFromTemplateInstantiation(_) and
  // F Prime: allow the plain `char` type when used as a C-string or character
  // array (i.e. reached through pointer or array indirection). A scalar `char`,
  // and `signed char`/`unsigned char` in any form, are still flagged.
  not (
    usedType instanceof PlainCharType and
    isPlainCharIndirection(getAnImmediateUsedType(d))
  ) and
  // F Prime: allow declarations whose basic integral type is dictated by an
  // external (system/libc) API. This covers local variables, parameters and
  // members that receive values from, are passed to, or (for parameters/return
  // types) mirror an external call, as well as values fixed by a system macro.
  // Repository-internal declarations that do not cross a system boundary are
  // still flagged.
  not externalApiDeclaration(d) and
  // F Prime: the autocoder's generated array classes iterate a
  // `std::initializer_list` with `for (const auto& e : il)`; `auto` erases the
  // element's F Prime typedef (as a template instantiation would), so the loop
  // variable is not actionable.
  not autoInitializerListLoopVar(d) and
  // F Prime: the language mandates a plain `int` dummy parameter to
  // distinguish the postfix increment/decrement operators.
  not exists(Operator op |
    op.getName() = ["operator++", "operator--"] and d = op.getAParameter()
  ) and
  // F Prime: exclude vendored third-party code, which is not maintained to the
  // F Prime coding standard, and the Python virtual environment (toolchain
  // files such as CMake's compiler ABI probes live inside it).
  not d.getFile().getRelativePath().matches(["Utils/Hash/libcrc/%", "%fprime-venv/%"])
select d,
  d.getName() + " uses the basic integral type " + usedType.getName() +
    " rather than a typedef with size and signedness."
