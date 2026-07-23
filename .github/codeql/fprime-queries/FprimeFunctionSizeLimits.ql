/**
 * @name Function too long
 * @description Function length should be limited to what can be printed on a single
 *              sheet of paper (60 lines of code). Number of parameters is limited to
 *              6 or fewer. This F Prime refinement of cpp/jpl-c/function-size-limits
 *              counts lines of code (comments and blank lines excluded), and exempts
 *              from the parameter limit the FW_ASSERT reporting contract (whose
 *              9-plus-argument signature is fixed by the assertion machinery) and
 *              functions overriding FPP-autocoded declarations (whose signatures are
 *              dictated by the model, e.g. command handlers).
 * @kind problem
 * @id cpp/fprime/jpl-c/function-size-limits
 * @problem.severity recommendation
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/**
 * A function implementing the FW_ASSERT reporting contract: its signature
 * (file, line, arg count, and six FwAssertArgType arguments) is fixed by the
 * assertion machinery and cannot be reduced.
 */
predicate fwAssertContract(Function f) {
  f.getName() = ["reportAssert", "retrieveAssert", "SwAssert", "defaultSwAssert", "defaultReportAssert"] and
  exists(Parameter p, TypedefType t | p = f.getAParameter() and t.getName() = "FwAssertArgType" |
    p.getType() = t or
    p.getType().(ReferenceType).getBaseType() = t
  )
}

/**
 * A function whose signature is dictated by the FPP model: it overrides a
 * declaration emitted by the autocoder (e.g. a command or port handler
 * declared pure virtual in a generated ComponentAc header).
 */
predicate fppModeled(Function f) {
  exists(MemberFunction base | f.(MemberFunction).overrides+(base) |
    base.getFile().getRelativePath().matches("%build-fprime%")
  )
}

string lengthWarning(Function f) {
  exists(int lines | lines = f.getMetrics().getNumberOfLinesOfCode() |
    lines > 60 and
    result = f.getName() + " has too many lines of code (" + lines + ", while 60 are allowed)."
  )
}

string paramWarning(Function f) {
  exists(int params | params = f.getMetrics().getNumberOfParameters() |
    params > 6 and
    not fwAssertContract(f) and
    not fppModeled(f) and
    result = f.getName() + " has too many parameters (" + params + ", while 6 are allowed)."
  )
}

from Function f, string msg
where
  msg = lengthWarning(f) or
  msg = paramWarning(f)
select f, msg
