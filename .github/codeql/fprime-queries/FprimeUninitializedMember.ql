/**
 * @name Member not initialized by constructor
 * @description Every scalar, pointer, or enum data member should be given a
 *              value by each constructor (initializer list, in-class
 *              initializer, or assignment in the constructor body) so that no
 *              member is read before it is set.
 * @kind problem
 * @id cpp/fprime/uninitialized-member
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

/** A member whose value is indeterminate unless explicitly set. */
predicate isScalarLike(MemberVariable m) {
  exists(Type t |
    t = m.getUnspecifiedType() or
    t = m.getUnspecifiedType().(ArrayType).getBaseType+().getUnspecifiedType()
  |
    t instanceof ArithmeticType or t instanceof PointerType or t instanceof Enum
  )
}

/** `m` is written somewhere in `f` or in a member function `f` calls directly. */
predicate writtenIn(MemberVariable m, Function f) {
  exists(VariableAccess va | va.getTarget() = m |
    va.getEnclosingFunction() = f and
    (
      va.isModified() or
      va.isAddressOfAccess() or
      va.getParent*() instanceof Assignment or
      // Arrays decay to pointers when passed to a function that fills them
      va.getUnspecifiedType() instanceof ArrayType and exists(Call c | c.getAnArgument() = va)
    )
  )
  or
  exists(FunctionCall fc | fc.getEnclosingFunction() = f | writtenIn(m, fc.getTarget()))
}

from Class c, Constructor ctor, MemberVariable m
where
  c.fromSource() and
  not c instanceof Union and
  ctor = c.getAConstructor() and
  ctor.hasDefinition() and
  ctor.fromSource() and
  not ctor.isCompilerGenerated() and
  not ctor.isDeleted() and
  not ctor instanceof CopyConstructor and
  not ctor instanceof MoveConstructor and
  m = c.getAMemberVariable() and
  not m.isStatic() and
  isScalarLike(m) and
  not m.hasInitializer() and
  not exists(ConstructorFieldInit fi | fi.getEnclosingFunction() = ctor and fi.getTarget() = m) and
  not writtenIn(m, ctor)
select ctor,
  "Constructor of " + c.getName() + " does not initialize member $@.", m, m.getName()
