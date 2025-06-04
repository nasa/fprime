/**
 * @name Use of basic integral type
 * @description Typedefs that indicate size and signedness should be used in place of the basic types.
 * @kind problem
 * @id cpp/jpl-c/basic-int-types
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

 import cpp

 predicate allowedTypedefs(TypedefType t) {
   t.getName() = ["I64", "U64", "I32", "U32", "I16", "U16", "I8", "U8", "F64", "F32"]
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
 
 from Declaration d, Type usedType
 where
   usedType = getAUsedType*(getAnImmediateUsedType(d)) and
   problematic(usedType) and
   // Ignore violations for which we do not have a valid location.
   not d.getLocation() instanceof UnknownLocation
 select d,
   d.getName() + " uses the basic integral type " + usedType.getName() +
     " rather than a typedef with size and signedness."
 