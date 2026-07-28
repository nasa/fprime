/**
 * @name Public data member
 * @description Non-static data members of classes with behavior (member
 *              functions) should be private or protected to preserve
 *              encapsulation. Plain aggregate types (no member functions)
 *              are not flagged.
 * @kind problem
 * @id cpp/fprime/public-data-member
 * @problem.severity recommendation
 * @tags maintainability
 *       external/jpl
 */

import cpp

from MemberVariable mv, Class c
where
  mv.getDeclaringType() = c and
  mv.fromSource() and
  mv.isPublic() and
  not mv.isStatic() and
  not mv.isConst() and
  not c instanceof Union and
  // Aggregates without behavior are acceptable as plain data carriers
  exists(MemberFunction f |
    f.getDeclaringType() = c and
    not f.isCompilerGenerated() and
    not f instanceof Constructor and
    not f instanceof Destructor
  )
select mv,
  "Data member " + mv.getName() + " of class " + c.getName() +
    " is public; consider making it private or protected."
